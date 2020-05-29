#ifndef OTPTOKENWIDGET_HPP
#define OTPTOKENWIDGET_HPP

#include <QTableWidget>

#include "otptokenmodel.hpp"

class OTPTokenWidget : public QTableWidget
{
    Q_OBJECT

public:
    OTPTokenWidget(OTPTokenModel *model, QWidget *parent = nullptr);

    enum class RowHeight
    {
        Desktop   = 25,
        Mobile    = 80,
    };

    /**
     * Filters the shown tokens in the view.
     * The input string is parsed as a regular expression.
     *
     * To clear the filter pass an empty string to it.
     */
    bool setFilter(const QString &filter = QString());

    /**
     * Sets the height of rows. This also affects icon sizes.
     */
    void setRowHeight(RowHeight height);

    /**
     * Enables copying the generated token when clicking on the label cell.
     *
     * Recommended for touch screens.
     */
    void enableTokenCopyOnLabelClick();

protected:
    void changeEvent(QEvent *event);

private:
    OTPTokenModel *model = nullptr;

    void refresh();
    void makeAllRowsVisible();
    void updateHeaderLabels();

    QString filterPattern;
    RowHeight rowHeight = RowHeight::Desktop;
};

#endif // OTPTOKENWIDGET_HPP
