#ifndef LABELWITHICONDELEGATE_HPP
#define LABELWITHICONDELEGATE_HPP

#include "otpbasewidget.hpp"

#include <QLayout>
#include <QLabel>
#include <QString>
#include <QByteArray>

#include <memory>

class LabelWithIconDelegate : public OTPBaseWidget
{
    Q_OBJECT

public:
    LabelWithIconDelegate(
            const QString &label,
            const QByteArray &iconData,
            const QSize &iconSize = QSize(),
            QWidget *parent = nullptr);

private:
    QString _label;
    QByteArray _icon;
    QSize _iconSize;

    std::shared_ptr<QHBoxLayout> _layout;
    std::shared_ptr<QLabel> _labelWidget;
    std::shared_ptr<QLabel> _iconWidget;
};

#endif // LABELWITHICONDELEGATE_HPP
