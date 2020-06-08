#ifndef OTPTOKENMODEL_HPP
#define OTPTOKENMODEL_HPP

#include <QAbstractTableModel>

#include <otptoken.hpp>

#include <vector>

class OTPTokenModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ViewMode
    {
        DisplayMode,
        EditMode,
    };

    OTPTokenModel(const std::vector<OTPToken> *tokens, ViewMode = DisplayMode, QObject *parent = nullptr);

    enum {
        // Display Columns
        ColActions  = 0,
        ColType     = 1,
        ColLabel    = 2,
        ColToken    = 3, // generated token

        // Edit Columns
        ColSecret   = 4,
        ColDigits   = 5,
        ColPeriod   = 6,
        ColCounter  = 7,
        ColAlgorithm = 8,
        ColDelete   = 9,
    };

    /**
     * Refreshes the model when data has changed externally.
     * This model doesn't contain everything and is only aimed for
     * displaying the underlying OTPToken list in the view.
     */
    void refresh();

    inline void setViewMode(ViewMode viewMode)
    {
        this->viewMode = viewMode;
        this->refresh();
    }

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
        return this->data(index.row(), index.column(), role);
    }

private:
    friend class OTPTokenWidget;

    // this model may not modify the token list
    const std::vector<OTPToken> *tokens = nullptr;

    ViewMode viewMode = DisplayMode;
};

#endif // OTPTOKENMODEL_HPP
