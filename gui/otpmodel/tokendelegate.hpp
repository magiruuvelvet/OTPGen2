#ifndef TOKENDELEGATE_HPP
#define TOKENDELEGATE_HPP

#include "otpbasewidget.hpp"

#include <QLayout>
#include <QProgressBar>
#include <QLineEdit>
#include <QTimer>

#include <string>

class OTPToken;

class TokenDelegate : public OTPBaseWidget
{
    Q_OBJECT

public:
    TokenDelegate(const OTPToken *tokenObj, QWidget *parent = nullptr);
    ~TokenDelegate();

    /**
     * Sets the visibility of the generated token and the timer bar.
     */
    void setGeneratedTokenVisible(bool);

private:
    void restartTimer();
    void generateToken();
    void updateProgressBar();

    void updateProgressBarColor(const QString &color);

    // pointer to the OTPToken instance
    const OTPToken *tokenObj = nullptr;

    std::shared_ptr<QTimer> tokenTimer;
    std::shared_ptr<QTimer> progressBarTimer;

    std::shared_ptr<QVBoxLayout> _layout;
    std::shared_ptr<QLineEdit> _generatedToken;
    std::shared_ptr<QProgressBar> _timerBar;
};

#endif // TOKENDELEGATE_HPP
