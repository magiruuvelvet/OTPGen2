#ifndef OTPTOKENWIDGET_HPP
#define OTPTOKENWIDGET_HPP

#include <QTableWidget>

#include "otptokenmodel.hpp"

class OTPTokenWidget : public QTableWidget
{
    Q_OBJECT

public:
    OTPTokenWidget(OTPTokenModel *model, QWidget *parent = nullptr);

    /**
     * Filters the shown tokens in the view.
     * The input string is parsed as a regular expression.
     *
     * To clear the filter pass an empty string to it.
     */
    bool setFilter(const QString &filter = QString());

private:
    OTPTokenModel *model = nullptr;

    void refresh();
    void makeAllRowsVisible();

    QString filterPattern;
};

#endif // OTPTOKENWIDGET_HPP
