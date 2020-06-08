#include "otptokenmodel.hpp"

OTPTokenModel::OTPTokenModel(const std::vector<OTPToken> *tokens, QObject *parent)
    : QAbstractTableModel(parent),
      tokens(tokens)
{
}

void OTPTokenModel::refresh()
{
    this->beginResetModel();
    this->endResetModel();
}

int OTPTokenModel::rowCount(const QModelIndex &parent) const
{
    if (!tokens)
    {
        return 0;
    }
    else
    {
        return static_cast<int>(tokens->size());
    }
}

int OTPTokenModel::columnCount(const QModelIndex &parent) const
{
    // [Show/Copy] [Type] [Icon/Label] [Token]
    // (all columns have a custom view delegate)
    return 4;
}

QVariant OTPTokenModel::data(int row, int column, int role) const
{
    if (!tokens)
    {
        return {};
    }

    // return basic display data about the token
    if (role == Qt::DisplayRole)
    {
        switch (column)
        {
            // [Show/Copy]
            case ColActions:
                return {};

            // [Type]
            case ColType:
                return QString::fromStdString(tokens->at(row).typeName());

            // [Icon/Label]
            case ColLabel:
                return QString::fromStdString(tokens->at(row).label());

            // [Token]
            case ColToken:
                return {};
        }
    }

    // return pointer address to the OTPToken instance
    else if (role == Qt::UserRole)
    {
        return QVariant::fromValue(reinterpret_cast<std::uintptr_t>(&tokens->at(row)));
    }

    return {};
}

QVariant OTPTokenModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case ColActions: return tr("Actions", "otp"); // actions (show token, copy to clipboard) for current token
            case ColType:    return tr("Type", "otp");    // OTP type: TOTP, HOTP, Steam
            case ColLabel:   return tr("Label", "otp");   // OTP token label (example: GitHub)
            case ColToken:   return tr("Token", "otp");   // generated OTP token code (example: 123456)
        }
    }

    return {};
}
