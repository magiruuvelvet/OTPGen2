#include "otptokenmodel.hpp"

namespace
{

static int realHeaderColumn(int column, OTPTokenModel::ViewMode viewMode)
{
    if (viewMode == OTPTokenModel::EditMode)
    {
        switch (column)
        {
            case 0: return OTPTokenModel::ColType;
            case 1: return OTPTokenModel::ColLabel;
            case 2: return OTPTokenModel::ColSecret;
            case 3: return OTPTokenModel::ColDigits;
            case 4: return OTPTokenModel::ColPeriod;
            case 5: return OTPTokenModel::ColCounter;
            case 6: return OTPTokenModel::ColAlgorithm;
            case 7: return OTPTokenModel::ColDelete;
        }
    }
    else
    {
        return column;
    }

    return -1;
}

} // anonymous namespace

OTPTokenModel::OTPTokenModel(const std::vector<OTPToken> *tokens, ViewMode viewMode, QObject *parent)
    : QAbstractTableModel(parent),
      tokens(tokens),
      viewMode(viewMode)
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
    if (this->viewMode == DisplayMode)
    {
        // [Show/Copy] [Type] [Icon/Label] [Token]
        // (all columns have a custom view delegate)
        return 4;
    }
    else
    {
        // [Type] [Icon/Label] [Secret] [Digits] [Period] [Counter] [Algorithm] [Delete]
        return 8;
    }
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

            // [Secret]
            case ColSecret:
                return QString::fromStdString(tokens->at(row).secret());

            // [Digits]
            case ColDigits:
                return tokens->at(row).digits();

            // [Period]
            case ColPeriod:
                return tokens->at(row).period();

            // [Counter]
            case ColCounter:
                return tokens->at(row).counter();

            // [Algorithm]
            case ColAlgorithm:
                return QString::fromStdString(tokens->at(row).algorithmName());

            // [Delete]
            case ColDelete:
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
        switch (realHeaderColumn(section, this->viewMode))
        {
            case ColActions: return tr("Actions", "otp"); // actions (show token, copy to clipboard) for current token
            case ColType:    return tr("Type", "otp");    // OTP type: TOTP, HOTP, Steam
            case ColLabel:   return tr("Label", "otp");   // OTP token label (example: GitHub)
            case ColToken:   return tr("Token", "otp");   // generated OTP token code (example: 123456)
            case ColSecret:  return tr("Secret", "otp");  // token secret
            case ColDigits:  return tr("Digits", "otp");  // token digit length/count
            case ColPeriod:  return tr("Period", "otp");  // token validity in seconds
            case ColCounter: return tr("Counter", "otp"); // usage counter for HOTP tokens
            case ColAlgorithm: return tr("Algorithm", "otp"); // algorithm type of token (SHA-1, SHA-256, etc.)
            case ColDelete:  return tr("Delete", "otp");  // delete action in edit widget
        }
    }

    return {};
}
