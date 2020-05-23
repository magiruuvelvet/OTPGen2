#ifndef OTPTOKENMODEL_HPP
#define OTPTOKENMODEL_HPP

#include <QAbstractTableModel>

#include <otptoken.hpp>

#include <vector>

class OTPTokenModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    OTPTokenModel(const std::vector<OTPToken> *tokens, QObject *parent = nullptr);

    /**
     * Refreshes the model when data has changed externally.
     * This model doesn't contain everything and is only aimed for
     * displaying the underlying OTPToken list in the view.
     */
    void refresh();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(int row, int column, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // disable editing capabilities
    inline bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override
    {
        return false;
    }

    // QModelIndex is inaccessible without using an actual view class
    inline QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        return {};
    }

private:
    // this model may not modify the token list
    const std::vector<OTPToken> *tokens = nullptr;
};

#endif // OTPTOKENMODEL_HPP
