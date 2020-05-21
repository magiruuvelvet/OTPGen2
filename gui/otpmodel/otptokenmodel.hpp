#ifndef OTPTOKENMODEL_HPP
#define OTPTOKENMODEL_HPP

#include <QAbstractTableModel>

#include <otptoken.hpp>

#include <vector>

class OTPTokenModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    OTPTokenModel(std::vector<OTPToken> *tokens, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    const std::vector<OTPToken> *tokens = nullptr;
};

#endif // OTPTOKENMODEL_HPP
