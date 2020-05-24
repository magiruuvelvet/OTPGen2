#include "tokendelegate.hpp"

#include <otptoken.hpp>

TokenDelegate::TokenDelegate(const OTPToken *tokenObj, QWidget *parent)
    : OTPBaseWidget(parent),
      tokenObj(tokenObj)
{
    this->_layout = std::make_shared<QVBoxLayout>();
    this->_layout->setSpacing(0);
    this->_layout->setContentsMargins(0, 0, 0, 0);
    this->_layout->setSizeConstraint(QLayout::SetMaximumSize);

    // setup timer for non-HOTP tokens
    this->tokenTimer = std::make_shared<QTimer>();
    this->progressBarTimer = std::make_shared<QTimer>();
    if (tokenObj->type() != OTPToken::HOTP)
    {
        this->tokenTimer->setTimerType(Qt::VeryCoarseTimer);
        this->tokenTimer->setSingleShot(true);
        this->progressBarTimer->setTimerType(Qt::VeryCoarseTimer);
        this->progressBarTimer->setSingleShot(false);
        this->progressBarTimer->setInterval(1000); // 1 second
        connect(this->tokenTimer.get(), &QTimer::timeout, this, &TokenDelegate::restartTimer);
        connect(this->progressBarTimer.get(), &QTimer::timeout, this, &TokenDelegate::updateProgressBar);
    }

    // setup progress bar for token validity
    this->_timerBar = std::make_shared<QProgressBar>(this);
    this->_timerBar->setVisible(false);
    if (tokenObj->type() != OTPToken::HOTP)
    {
        this->_timerBar->setTextVisible(false);
        this->_timerBar->setMinimum(0);
        this->_timerBar->setMaximum(tokenObj->period());
        this->_timerBar->setFixedHeight(3);
        this->_timerBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        this->_timerBar->setAutoFillBackground(true);
        this->_timerBar->setOrientation(Qt::Horizontal);
        this->_timerBar->setValue(0);
        this->_timerBar->setFocusPolicy(Qt::NoFocus);
        this->_timerBar->setVisible(true);

        // make progress bar flat and elegant
        this->updateProgressBarColor("lightgray");

        this->progressBarTimer->start();

        this->_layout->addWidget(this->_timerBar.get(), 0, Qt::AlignTop);
    }

    // generated token
    this->_generatedToken = std::make_shared<QLineEdit>(this);
    this->_generatedToken->setReadOnly(true);
    this->_generatedToken->setFrame(false);
    this->_generatedToken->setAutoFillBackground(true);
    this->_generatedToken->setAlignment(Qt::AlignCenter);
    this->_generatedToken->setContentsMargins(0, 0, 0, 0);
    auto font = this->_generatedToken->font();
    font.setFamily("monospace");
    this->_generatedToken->setFont(font);
    this->_generatedToken->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->_layout->addWidget(this->_generatedToken.get());

    this->setLayout(this->_layout.get());

    // start timer
    if (tokenObj->type() != OTPToken::HOTP)
    {
        this->restartTimer();
    }

    // setup size policies for hidden widgets
    auto timerBarSizePolicy = this->_timerBar->sizePolicy();
    timerBarSizePolicy.setRetainSizeWhenHidden(true);
    this->_timerBar->setSizePolicy(timerBarSizePolicy);

    auto generatedTokenSizePolicy = this->_generatedToken->sizePolicy();
    generatedTokenSizePolicy.setRetainSizeWhenHidden(true);
    this->_generatedToken->setSizePolicy(generatedTokenSizePolicy);

    // hide generated tokens by default
    this->setGeneratedTokenVisible(false);
}

TokenDelegate::~TokenDelegate()
{
    this->tokenTimer->stop();
    this->progressBarTimer->stop();
}

void TokenDelegate::setGeneratedTokenVisible(bool visible)
{
    this->_timerBar->setVisible(visible);
    this->_generatedToken->setVisible(visible);
}

void TokenDelegate::restartTimer()
{
    this->generateToken();
    this->tokenTimer->setInterval(tokenObj->remainingTokenValidity() * 1000);
    this->tokenTimer->start();
}

void TokenDelegate::generateToken()
{
    this->_generatedToken->setText(QString::fromStdString(tokenObj->generate()));
}

void TokenDelegate::updateProgressBar()
{
    const auto remaining = this->tokenTimer->remainingTime() / 1000;
    this->_timerBar->setValue(remaining);

    // make progress bar red when remaining token validity goes below 5 seconds
    if (remaining <= 4)
    {
        this->updateProgressBarColor("red");
    }
    else
    {
        this->updateProgressBarColor("lightgray");
    }
}

void TokenDelegate::updateProgressBarColor(const QString &color)
{
    this->_timerBar->setStyleSheet(QString(
        "QProgressBar { background-color: transparent; border: 0; outline: 0; }"
        "QProgressBar::chunk { background-color: %1; border: 0; outline: 0; }"
    ).arg(color));
}
