#include "otptokenwidget.hpp"

#include <QHeaderView>

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
        // TODO: create cell widgets here
    }
}
