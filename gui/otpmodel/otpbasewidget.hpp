#ifndef OTPBASEWIDGET_HPP
#define OTPBASEWIDGET_HPP

#include <QWidget>

#include "otptokenrowcontainer.hpp"

class OTPBaseWidget : public QWidget
{
    Q_OBJECT

public:
    OTPBaseWidget(QWidget *parent = nullptr);

    inline void setRowContainer(OTPTokenRowContainer rowContainer)
    {
        this->rowContainer = rowContainer;
    }

protected:
    OTPTokenRowContainer rowContainer;
};

#endif // OTPBASEWIDGET_HPP
