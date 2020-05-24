#include "actionsdelegate.hpp"

#include "tokendelegate.hpp"

#include "otptoken.hpp"

#include <QApplication>
#include <QClipboard>

ActionsDelegate::ActionsDelegate(const OTPToken *tokenObj, QWidget *parent)
    : OTPBaseWidget(parent),
      tokenObj(tokenObj)
{
    this->_layout = std::make_shared<QHBoxLayout>();
    this->_layout->setSpacing(3);
    this->_layout->setContentsMargins(8, 0, 8, 0);
    this->_layout->setSizeConstraint(QLayout::SetMaximumSize);

    this->_visibilityCb = std::make_shared<QCheckBox>();
    this->_visibilityCb->setToolTip(tr("Make token visible", "otp"));
    this->_visibilityCb->setCheckable(true);
    this->_visibilityCb->setChecked(false);
    this->_visibilityCb->setFocusPolicy(Qt::NoFocus);
    connect(this->_visibilityCb.get(), &QCheckBox::stateChanged, this, &ActionsDelegate::visibilityChanged);

    this->_clipboardAction = std::make_shared<QPushButton>();
    this->_clipboardAction->setToolTip(tr("Copy token to clipboard", "otp"));
    this->_clipboardAction->setFixedSize(18, 18);
    this->_clipboardAction->setIconSize(QSize(18, 18));
    this->_clipboardAction->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->_clipboardAction->setFlat(true);
    this->_clipboardAction->setIcon(QIcon(QPixmap(":/copy-content.svgz").scaled(18, 18, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    this->_clipboardAction->setFocusPolicy(Qt::NoFocus);
    connect(this->_clipboardAction.get(), &QPushButton::clicked, this, &ActionsDelegate::copyTokenToClipboard);

    this->_layout->addWidget(this->_visibilityCb.get(), 0, Qt::AlignLeft | Qt::AlignVCenter);
    this->_layout->addWidget(this->_clipboardAction.get(), 0, Qt::AlignLeft | Qt::AlignVCenter);
    this->_layout->addSpacerItem(new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    this->setLayout(this->_layout.get());
}

void ActionsDelegate::visibilityChanged(int state)
{
    if (state == Qt::Checked)
    {
        qobject_cast<TokenDelegate*>(this->rowContainer.token)->setGeneratedTokenVisible(true);
    }
    else
    {
        qobject_cast<TokenDelegate*>(this->rowContainer.token)->setGeneratedTokenVisible(false);
    }
}

void ActionsDelegate::copyTokenToClipboard()
{
    auto clipboard = QApplication::clipboard();
    clipboard->setText(QString::fromStdString(tokenObj->generate()));
}
