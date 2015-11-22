#ifndef OPTIONSMODEL_H
#define OPTIONSMODEL_H

#include <QAbstractListModel>

class CWallet;

/* Interface from QT to configuration data structure for bitcoin client.
   To QT, the options are presented as a list with the different options
   laid out vertically.
   This can be changed to a tree once the settings become sufficiently
   complex.
 */
class OptionsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit OptionsModel(CWallet *wallet, QObject *parent = 0);

    enum OptionID {
        StartAtStartup,
        MinimizeToTray,
        MapPortUPnP,
        MinimizeOnClose,
        ConnectSOCKS4,
        ProxyIP,
        ProxyPort,
        Fee,
        OptionIDRowCount
    };

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    /* Explicit getters */
    qint64 getTransactionFee();
    bool getMinimizeToTray();
    bool getMinimizeOnClose();
private:
    // Wallet stores persistent options
    CWallet *wallet;
signals:

public slots:

};

#endif // OPTIONSMODEL_H
