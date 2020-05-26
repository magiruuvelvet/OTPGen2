#ifndef LABELWITHICONDELEGATE_HPP
#define LABELWITHICONDELEGATE_HPP

#include "otpbasewidget.hpp"

#include <QLayout>
#include <QLabel>
#include <QString>
#include <QByteArray>
#include <QImage>

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

    LabelWithIconDelegate(
            const QString &label,
            const QString &iconPath,
            const QSize &iconSize = QSize(),
            QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QString _label;
    QByteArray _icon;
    QSize _iconSize;
    QImage _processedIcon;

    std::shared_ptr<QHBoxLayout> _layout;
    std::shared_ptr<QLabel> _labelWidget;
    std::shared_ptr<QLabel> _iconWidget;
};

#endif // LABELWITHICONDELEGATE_HPP
