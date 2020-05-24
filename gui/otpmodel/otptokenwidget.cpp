#include "otptokenwidget.hpp"

#include "actionsdelegate.hpp"
#include "labelwithicondelegate.hpp"
#include "tokendelegate.hpp"

#include <QHeaderView>

#define COLUMN_ACTIONS  0
#define COLUMN_TYPE     1
#define COLUMN_LABEL    2
#define COLUMN_TOKEN    3

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
    this->verticalHeader()->setDefaultSectionSize(25);

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
    QStringList headerLabels;
    for (auto i = 0; i < model->columnCount(); ++i)
    {
        headerLabels.append(model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
    }
    this->setHorizontalHeaderLabels(headerLabels);

    // populate rows
    this->refresh();
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
                            new LabelWithIconDelegate(model->data(i, COLUMN_LABEL).toString(), icon, QSize(30, 30), this));

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
}
