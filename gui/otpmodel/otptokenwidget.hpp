#ifndef OTPTOKENWIDGET_HPP
#define OTPTOKENWIDGET_HPP

#include <QTableWidget>

#include "otptokenmodel.hpp"

class OTPTokenWidget : public QTableWidget
{
    Q_OBJECT

public:
    OTPTokenWidget(OTPTokenModel *model, QWidget *parent = nullptr);

private:
    OTPTokenModel *model = nullptr;

    void refresh();
};

#endif // OTPTOKENWIDGET_HPP
