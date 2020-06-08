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
     * Enables touch screen specific features for mobile devices.
     */
    void setTouchScreenMode(bool);

protected:
    void changeEvent(QEvent *event);

private:
    OTPTokenModel *model = nullptr;

    void setupDisplayMode();
    void setupEditMode();

    void refresh();
    void makeAllRowsVisible();
    void updateHeaderLabels();

    QString filterPattern;
    RowHeight rowHeight = RowHeight::Desktop;
    bool touchScreenMode = false;
    unsigned int iconSize = 30;
};

#endif // OTPTOKENWIDGET_HPP
