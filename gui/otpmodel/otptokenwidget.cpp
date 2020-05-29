#include "otptokenwidget.hpp"

#include "actionsdelegate.hpp"
#include "labelwithicondelegate.hpp"
#include "tokendelegate.hpp"

#include <QApplication>
#include <QHeaderView>
#include <QRegularExpression>
#include <QEvent>
#include <QClipboard>

#define COLUMN_ACTIONS  0
#define COLUMN_TYPE     1
#define COLUMN_LABEL    2
#define COLUMN_TOKEN    3

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
    this->setColumnCount(model->columnCount());
    this->updateHeaderLabels();

    // setup minimum width constraints for columns
    static const std::vector<int> minSizeContraints = {
        70, 90, 150, 100,
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
        const auto type = model->data(i, COLUMN_TYPE).toString();
        const auto label = model->data(i, COLUMN_LABEL).toString();
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

void OTPTokenWidget::enableTokenCopyOnLabelClick()
{

}

void OTPTokenWidget::refresh()
{
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
    unsigned int iconSize = 30;

    if (this->rowHeight == RowHeight::Mobile)
    {
        iconSize = 75;
    }

    // create cell widgets
    for (auto i = 0; i < model->rowCount(); ++i)
    {
        const OTPToken *token = reinterpret_cast<const OTPToken*>(model->data(i, 0, Qt::UserRole).value<std::uintptr_t>());

        OTPTokenRowContainer rowContainer;
        rowContainer.obj = token;

        // token actions
        this->setCellWidget(i, COLUMN_ACTIONS,
                            new ActionsDelegate(token, this));

        // token display type
        QString typeIcon;
        switch (token->type())
        {
            case OTPToken::TOTP: typeIcon = ":/clock.svgz"; break;
            case OTPToken::Steam: typeIcon = ":/logos/steam.svgz"; break;
        }

        this->setCellWidget(i, COLUMN_TYPE,
                            new LabelWithIconDelegate(model->data(i, COLUMN_TYPE).toString(), typeIcon, QSize(16, 16), this));
        this->cellWidget(i, COLUMN_TYPE)->layout()->setSpacing(8);
        this->cellWidget(i, COLUMN_TYPE)->layout()->setContentsMargins(8, 0, 8, 0);

        // token label and icon
        const QByteArray icon(token->icon().data(), token->icon().size());
        this->setCellWidget(i, COLUMN_LABEL,
                            new LabelWithIconDelegate(model->data(i, COLUMN_LABEL).toString(), icon, QSize(iconSize, iconSize), this));
        qobject_cast<LabelWithIconDelegate*>(this->cellWidget(i, COLUMN_LABEL))->setClickCallback(&copy_token_to_clipboard);

        // generated token
        this->setCellWidget(i, COLUMN_TOKEN,
                            new TokenDelegate(token, this));

        // setup row pointers for easy access across the entire row
        auto waction = qobject_cast<OTPBaseWidget*>(this->cellWidget(i, COLUMN_ACTIONS));
        auto wtype = qobject_cast<OTPBaseWidget*>(this->cellWidget(i, COLUMN_TYPE));
        auto wlabel = qobject_cast<OTPBaseWidget*>(this->cellWidget(i, COLUMN_LABEL));
        auto wtoken = qobject_cast<OTPBaseWidget*>(this->cellWidget(i, COLUMN_TOKEN));
        rowContainer.action = waction;
        rowContainer.type = wtype;
        rowContainer.label = wlabel;
        rowContainer.token = wtoken;
        waction->setRowContainer(rowContainer);
        wtype->setRowContainer(rowContainer);
        wlabel->setRowContainer(rowContainer);
        wtoken->setRowContainer(rowContainer);
    }

    // reapply the current filter
    this->setFilter(this->filterPattern);
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
        this->updateHeaderLabels();
        model->refresh();
    }

    QTableWidget::changeEvent(event);
}
