#include "labelwithicondelegate.hpp"

#include <QFile>
#include <QResizeEvent>

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
    this->_layout->setSpacing(12);
    this->_layout->setContentsMargins(3, 0, 3, 0);
    this->_layout->setSizeConstraint(QLayout::SetMaximumSize);

    this->_labelWidget = std::make_shared<QLabel>();
    this->_labelWidget->setText(label);
    this->_labelWidget->setWordWrap(false);
    this->_labelWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->_iconWidget = std::make_shared<QLabel>();
    this->_iconWidget->setMaximumHeight(_iconSize.height());
    this->_iconWidget->setFixedWidth(_iconSize.width());
    this->_iconWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QImage icon;
    if (icon.loadFromData(iconData))
    {
        this->_processedIcon = icon;
        this->_iconWidget->setPixmap(QPixmap::fromImage(icon).scaled(_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    this->_layout->addWidget(this->_iconWidget.get(), 0, Qt::AlignLeft | Qt::AlignVCenter);
    this->_layout->addWidget(this->_labelWidget.get(), 1, Qt::AlignLeft | Qt::AlignVCenter);

    this->setLayout(this->_layout.get());
}

LabelWithIconDelegate::LabelWithIconDelegate(const QString &label, const QString &iconPath, const QSize &iconSize, QWidget *parent)
    : LabelWithIconDelegate(label, QByteArray(), iconSize, parent)
{
    if (!iconPath.isEmpty())
    {
        QFile iconFile(iconPath);
        if (iconFile.open(QIODevice::ReadOnly))
        {
            QImage icon;
            if (icon.loadFromData(iconFile.readAll()))
            {
                this->_iconWidget->setPixmap(QPixmap::fromImage(icon).scaled(_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
    }
}

void LabelWithIconDelegate::resizeEvent(QResizeEvent *event)
{
//    if (!this->_processedIcon.isNull())
//    {
//        auto newHeight = this->height();
//        this->_iconWidget->setPixmap(QPixmap::fromImage(this->_processedIcon).scaled(QSize(newHeight, newHeight), Qt::KeepAspectRatio, Qt::SmoothTransformation));
//    }

    OTPBaseWidget::resizeEvent(event);
}
