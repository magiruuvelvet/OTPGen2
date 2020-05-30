#ifndef ACTIONSDELEGATE_HPP
#define ACTIONSDELEGATE_HPP

#include "otpbasewidget.hpp"

#include <QLayout>
#include <QCheckBox>
#include <QPushButton>

#include <memory>

class OTPToken;

class ActionsDelegate : public OTPBaseWidget
{
    Q_OBJECT

public:
    ActionsDelegate(const OTPToken *tokenObj, QWidget *parent = nullptr);

    inline QCheckBox *visibilityCheckbox() const
    {
        return this->_visibilityCb.get();
    }

private:
    void visibilityChanged(int);
    void copyTokenToClipboard();

    // pointer to the OTPToken instance
    const OTPToken *tokenObj = nullptr;

    std::shared_ptr<QHBoxLayout> _layout;
    std::shared_ptr<QCheckBox> _visibilityCb;
    std::shared_ptr<QPushButton> _clipboardAction;
};

#endif // ACTIONSDELEGATE_HPP
