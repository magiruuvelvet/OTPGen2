#include "otptokenwidget.hpp"

#include "actionsdelegate.hpp"
#include "labelwithicondelegate.hpp"
#include "tokendelegate.hpp"

#include <QApplication>
#include <QHeaderView>
#include <QRegularExpression>
#include <QEvent>
#include <QClipboard>

static const std::function<void(const OTPToken*)> copy_token_to_clipboard = [](const OTPToken *token){
    auto clipboard = QApplication::clipboard();
    clipboard->setText(QString::fromStdString(token->generate()));
};

OTPTokenWidget::OTPTokenWidget(OTPTokenModel *model, QWidget *parent)
    : QTableWidget(parent),
      model(model)
{
    // hide vertical header for cosmetics, useless clutter
    // TODO: required for changing the order using drag and drop later unless I figure out something better
    this->verticalHeader()->setDisabled(true);
    this->verticalHeader()->setHidden(true);

    // set fixed row height
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->verticalHeader()->setDefaultSectionSize(static_cast<int>(this->rowHeight));

    // drag and drop of entire rows to change the order
    this->verticalHeader()->setSectionsMovable(true);
    this->setDragEnabled(true);
    this->setDropIndicatorShown(true);
    this->setDragDropOverwriteMode(false);
    this->setDragDropMode(QTableView::InternalMove);

    // selection
    this->setSelectionBehavior(QTableView::SelectRows);
    this->setSelectionMode(QTableView::NoSelection);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // disable focus on widget itself, delegates should still be able to focus
    this->setFocusPolicy(Qt::NoFocus);

    // update view when model refreshes
    connect(model, &OTPTokenModel::modelReset, this, &OTPTokenWidget::refresh);

    // setup the model properties
    this->updateHeaderLabels();

    // setup minimum width constraints for columns
    static const std::vector<int> minSizeContraints = {
        0 /*70*/, 90, 150, 100,
    };
    connect(this->horizontalHeader(), &QHeaderView::sectionResized, this, [&](int logicalIndex, int oldSize, int newSize) {
        if (newSize < minSizeContraints.at(logicalIndex))
        {
            this->setColumnWidth(logicalIndex, minSizeContraints.at(logicalIndex));
        }
    });

    // set initial column widths
    for (auto i = 0; i < this->columnCount(); ++i)
    {
        this->setColumnWidth(i, minSizeContraints.at(i));
    }
    this->setColumnWidth(OTPTokenModel::ColActions, 70);

    // populate rows
    this->refresh();
}

bool OTPTokenWidget::setFilter(const QString &filter)
{
    this->filterPattern = filter.simplified();

    if (this->filterPattern.isEmpty())
    {
        this->makeAllRowsVisible();
        return true;
    }

    const auto filterRegex = QRegularExpression(this->filterPattern, QRegularExpression::CaseInsensitiveOption);

    if (!filterRegex.isValid())
    {
        this->makeAllRowsVisible();
        return false;
    }

    for (auto i = 0; i < this->rowCount(); ++i)
    {
        bool match = false;
        const auto type = model->data(i, OTPTokenModel::ColType).toString();
        const auto label = model->data(i, OTPTokenModel::ColLabel).toString();
        if (filterRegex.match(type).hasMatch() ||
            filterRegex.match(label).hasMatch())
        {
            match = true;
        }
        this->setRowHidden(i, !match);
    }

    return true;
}

void OTPTokenWidget::setRowHeight(RowHeight height)
{
    this->rowHeight = height;
    this->verticalHeader()->setDefaultSectionSize(static_cast<int>(this->rowHeight));
    this->refresh();
}

void OTPTokenWidget::setTouchScreenMode(bool enabled)
{
    this->touchScreenMode = enabled;
    this->setColumnHidden(0, enabled);
    this->refresh();
}

void OTPTokenWidget::refresh()
{
    this->updateHeaderLabels();

    // clear previous content
    this->clearContents();
    this->setRowCount(0);

    // reserve space for new content
    this->setRowCount(model->rowCount());

    // clear vertical header contents
    QStringList headerLabels;
    for (auto i = 0; i < model->rowCount(); ++i)
    {
        headerLabels.append(QString());
    }
    this->setVerticalHeaderLabels(headerLabels);

    // icon size
    if (this->rowHeight == RowHeight::Mobile)
    {
        this->iconSize = 68;
    }
    else
    {
        this->iconSize = 30;
    }

    if (model->viewMode == OTPTokenModel::DisplayMode)
    {
        this->setupDisplayMode();
    }
    else
    {
        this->setupEditMode();
    }

    // reapply the current filter
    this->setFilter(this->filterPattern);
}

void OTPTokenWidget::setupDisplayMode()
{
    // create cell widgets
    for (auto i = 0; i < model->rowCount(); ++i)
    {
        const OTPToken *token = reinterpret_cast<const OTPToken*>(model->data(i, 0, Qt::UserRole).value<std::uintptr_t>());

        OTPTokenRowContainer rowContainer;
        rowContainer.obj = token;

        // token actions
        this->setCellWidget(i, OTPTokenModel::ColActions,
                            new ActionsDelegate(token, this));

        // token display type
        QString typeIcon;
        switch (token->type())
        {
            case OTPToken::TOTP: typeIcon = ":/clock.svgz"; break;
            case OTPToken::Steam: typeIcon = ":/logos/steam.svgz"; break;
        }

        this->setCellWidget(i, OTPTokenModel::ColType,
                            new LabelWithIconDelegate(model->data(i, OTPTokenModel::ColType).toString(), typeIcon, QSize(16, 16), this));
        this->cellWidget(i, OTPTokenModel::ColType)->layout()->setSpacing(8);
        this->cellWidget(i, OTPTokenModel::ColType)->layout()->setContentsMargins(8, 0, 8, 0);

        // token label and icon
        const QByteArray icon(token->icon().data(), token->icon().size());
        this->setCellWidget(i, OTPTokenModel::ColLabel,
                            new LabelWithIconDelegate(model->data(i, OTPTokenModel::ColLabel).toString(), icon, QSize(iconSize, iconSize), this));

        // generated token
        this->setCellWidget(i, OTPTokenModel::ColToken,
                            new TokenDelegate(token, this));

        if (this->touchScreenMode)
        {
            qobject_cast<LabelWithIconDelegate*>(this->cellWidget(i, OTPTokenModel::ColLabel))->setClickCallback(&copy_token_to_clipboard);
            qobject_cast<TokenDelegate*>(this->cellWidget(i, OTPTokenModel::ColToken))->setGeneratedTokenVisibilityOnClick(true);
        }

        // setup row pointers for easy access across the entire row
        auto waction = qobject_cast<OTPBaseWidget*>(this->cellWidget(i, OTPTokenModel::ColActions));
        auto wtype = qobject_cast<OTPBaseWidget*>(this->cellWidget(i, OTPTokenModel::ColType));
        auto wlabel = qobject_cast<OTPBaseWidget*>(this->cellWidget(i, OTPTokenModel::ColLabel));
        auto wtoken = qobject_cast<OTPBaseWidget*>(this->cellWidget(i, OTPTokenModel::ColToken));
        rowContainer.action = waction;
        rowContainer.type = wtype;
        rowContainer.label = wlabel;
        rowContainer.token = wtoken;
        waction->setRowContainer(rowContainer);
        wtype->setRowContainer(rowContainer);
        wlabel->setRowContainer(rowContainer);
        wtoken->setRowContainer(rowContainer);
    }
}

void OTPTokenWidget::setupEditMode()
{
    // TODO
}

void OTPTokenWidget::makeAllRowsVisible()
{
    for (auto i = 0; i < this->rowCount(); ++i)
    {
        this->setRowHidden(i, false);
    }
}

void OTPTokenWidget::updateHeaderLabels()
{
    this->setColumnCount(model->columnCount());

    QStringList headerLabels;
    for (auto i = 0; i < model->columnCount(); ++i)
    {
        headerLabels.append(model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
    }
    this->setHorizontalHeaderLabels(headerLabels);
}

void OTPTokenWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        model->refresh();
    }

    QTableWidget::changeEvent(event);
}
