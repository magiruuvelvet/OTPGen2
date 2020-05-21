#include "otptokenmodel.hpp"

OTPTokenModel::OTPTokenModel(std::vector<OTPToken> *tokens, QObject *parent)
    : QAbstractTableModel(parent),
      tokens(tokens)
{
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

QVariant OTPTokenModel::data(const QModelIndex &index, int role) const
{
    if (!tokens)
    {
        return {};
    }

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            // [Show/Copy]
            case 0:
                return {};

            // [Type]
            case 1:
                return QString::fromStdString(tokens->at(index.row()).typeName());

            // [Icon/Label]
            case 2:
                return QString::fromStdString(tokens->at(index.row()).label());

            // [Token]
            case 3:
                return {};
        }
    }

    return {};
}

QVariant OTPTokenModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case 0: return QString();
            case 1: return tr("Type", "otp");  // OTP type: TOTP, HOTP, Steam
            case 2: return tr("Label", "otp"); // OTP token label (example: GitHub)
            case 3: return tr("Token", "otp"); // generated OTP token code (example: 123456)
        }
    }

    return {};
}