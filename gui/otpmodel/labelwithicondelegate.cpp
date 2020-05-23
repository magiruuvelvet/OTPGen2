#include "labelwithicondelegate.hpp"

LabelWithIconDelegate::LabelWithIconDelegate(const QString &label, const QByteArray &iconData, const QSize &iconSize, QWidget *parent)
    : OTPBaseWidget(parent),
      _label(label),
      _icon(iconData),
      _iconSize(iconSize)
{
    if (!iconSize.isValid())
    {
        _iconSize = QSize(0, 0);
    }

    this->_layout = std::make_shared<QHBoxLayout>();
    this->_layout->setSpacing(10);
    this->_layout->setContentsMargins(3, 0, 3, 0);
    this->_layout->setSizeConstraint(QLayout::SetMaximumSize);

    this->_labelWidget = std::make_shared<QLabel>();
    this->_labelWidget->setText(label);
    this->_labelWidget->setWordWrap(false);
    this->_labelWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->_iconWidget = std::make_shared<QLabel>();
    this->_iconWidget->setFixedSize(_iconSize);
    this->_iconWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QImage icon;
    if (icon.loadFromData(iconData))
    {
        this->_iconWidget->setPixmap(QPixmap::fromImage(icon).scaled(_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    this->_layout->addWidget(this->_iconWidget.get(), 0, Qt::AlignLeft | Qt::AlignVCenter);
    this->_layout->addWidget(this->_labelWidget.get(), 1, Qt::AlignLeft | Qt::AlignVCenter);

    this->setLayout(this->_layout.get());
}
