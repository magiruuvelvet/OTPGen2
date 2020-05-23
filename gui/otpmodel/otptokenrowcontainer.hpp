#ifndef OTPTOKENROWCONTAINER_HPP
#define OTPTOKENROWCONTAINER_HPP

#include <QWidget>

class OTPToken;

struct OTPTokenRowContainer
{
    const OTPToken *obj = nullptr;
    QWidget *action = nullptr;
    QWidget *type = nullptr;
    QWidget *label = nullptr;
    QWidget *token = nullptr;
};

#endif // OTPTOKENROWCONTAINER_HPP
