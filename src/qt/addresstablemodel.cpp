#include "addresstablemodel.h"
#include "guiutil.h"

#include "headers.h"

#include <QFont>
#include <QColor>

const QString AddressTableModel::Send = "S";
const QString AddressTableModel::Receive = "R";

struct AddressTableEntry
{
    enum Type {
        Sending,
        Receiving
    };

    Type type;
    QString label;
    QString address;

    AddressTableEntry() {}
    AddressTableEntry(Type type, const QString &label, const QString &address):
        type(type), label(label), address(address) {}
};

// Private implementation
struct AddressTablePriv
{
    CWallet *wallet;
    QList<AddressTableEntry> cachedAddressTable;

    AddressTablePriv(CWallet *wallet):
            wallet(wallet) {}

    void refreshAddressTable()
    {
        cachedAddressTable.clear();

        CRITICAL_BLOCK(wallet->cs_mapKeys)
        CRITICAL_BLOCK(wallet->cs_mapAddressBook)
        {
            BOOST_FOREACH(const PAIRTYPE(std::string, std::string)& item, wallet->mapAddressBook)
            {
                std::string strAddress = item.first;
                std::string strName = item.second;
                uint160 hash160;
                bool fMine = (AddressToHash160(strAddress, hash160) && mapPubKeys.count(hash160));
                cachedAddressTable.append(AddressTableEntry(fMine ? AddressTableEntry::Receiving : AddressTableEntry::Sending,
                                  QString::fromStdString(strName),
                                  QString::fromStdString(strAddress)));
            }
        }
    }

    int size()
    {
        return cachedAddressTable.size();
    }

    AddressTableEntry *index(int idx)
    {
        if(idx >= 0 && idx < cachedAddressTable.size())
        {
            return &cachedAddressTable[idx];
        }
        else
        {
            return 0;
        }
    }
};

AddressTableModel::AddressTableModel(CWallet *wallet, QObject *parent) :
    QAbstractTableModel(parent),wallet(wallet),priv(0)
{
    columns << tr("Label") << tr("Address");
    priv = new AddressTablePriv(wallet);
    priv->refreshAddressTable();
}

AddressTableModel::~AddressTableModel()
{
    delete priv;
}

int AddressTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int AddressTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant AddressTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    AddressTableEntry *rec = static_cast<AddressTableEntry*>(index.internalPointer());

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case Label:
            if(rec->label.isEmpty() && role == Qt::DisplayRole)
            {
                return tr("(no label)");
            }
            else
            {
                return rec->label;
            }
        case Address:
            return rec->address;
        }
    }
    else if (role == Qt::FontRole)
    {
        QFont font;
        if(index.column() == Address)
        {
            font = GUIUtil::bitcoinAddressFont();
        }
        return font;
    }
    else if (role == TypeRole)
    {
        switch(rec->type)
        {
        case AddressTableEntry::Sending:
            return Send;
        case AddressTableEntry::Receiving:
            return Receive;
        default: break;
        }
    }
    return QVariant();
}

bool AddressTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if(!index.isValid())
        return false;
    AddressTableEntry *rec = static_cast<AddressTableEntry*>(index.internalPointer());

    if(role == Qt::EditRole)
    {
        switch(index.column())
        {
        case Label:
            wallet->SetAddressBookName(rec->address.toStdString(), value.toString().toStdString());
            rec->label = value.toString();
            break;
        case Address:
            // Refuse to set invalid address
            if(!validateAddress(value.toString()))
                return false;
            // Double-check that we're not overwriting receiving address
            if(rec->type == AddressTableEntry::Sending)
            {
                CRITICAL_BLOCK(wallet->cs_mapAddressBook)
                {
                    // Remove old entry
                    wallet->DelAddressBookName(rec->address.toStdString());
                    // Add new entry with new address
                    wallet->SetAddressBookName(value.toString().toStdString(), rec->label.toStdString());
                }

                rec->address = value.toString();
            }
            break;
        }
        emit dataChanged(index, index);

        return true;
    }
    return false;
}

QVariant AddressTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
        {
            return columns[section];
        }
    }
    return QVariant();
}

Qt::ItemFlags AddressTableModel::flags(const QModelIndex & index) const
{
    if(!index.isValid())
        return 0;
    AddressTableEntry *rec = static_cast<AddressTableEntry*>(index.internalPointer());

    Qt::ItemFlags retval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    // Can edit address and label for sending addresses,
    // and only label for receiving addresses.
    if(rec->type == AddressTableEntry::Sending ||
      (rec->type == AddressTableEntry::Receiving && index.column()==Label))
    {
        retval |= Qt::ItemIsEditable;
    }
    return retval;
}

QModelIndex AddressTableModel::index(int row, int column, const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    AddressTableEntry *data = priv->index(row);
    if(data)
    {
        return createIndex(row, column, priv->index(row));
    }
    else
    {
        return QModelIndex();
    }
}

void AddressTableModel::updateList()
{
    // Update internal model from Bitcoin core
    beginResetModel();
    priv->refreshAddressTable();
    endResetModel();
}

QString AddressTableModel::addRow(const QString &type, const QString &label, const QString &address)
{
    std::string strLabel = label.toStdString();
    std::string strAddress = address.toStdString();

    if(type == Send)
    {
        // Check for duplicate
        CRITICAL_BLOCK(wallet->cs_mapAddressBook)
        {
            if(wallet->mapAddressBook.count(strAddress))
            {
                return QString();
            }
        }
    }
    else if(type == Receive)
    {
        // Generate a new address to associate with given label, optionally
        // set as default receiving address.
        strAddress = PubKeyToAddress(wallet->GetKeyFromKeyPool());
    }
    else
    {
        return QString();
    }
    // Add entry and update list
    wallet->SetAddressBookName(strAddress, strLabel);
    updateList();
    return QString::fromStdString(strAddress);
}

bool AddressTableModel::removeRows(int row, int count, const QModelIndex & parent)
{
    Q_UNUSED(parent);
    AddressTableEntry *rec = priv->index(row);
    if(count != 1 || !rec || rec->type == AddressTableEntry::Receiving)
    {
        // Can only remove one row at a time, and cannot remove rows not in model.
        // Also refuse to remove receiving addresses.
        return false;
    }
    CRITICAL_BLOCK(wallet->cs_mapAddressBook)
    {
        wallet->DelAddressBookName(rec->address.toStdString());
    }
    updateList();
    return true;
}

void AddressTableModel::update()
{

}

bool AddressTableModel::validateAddress(const QString &address)
{
    uint160 hash160 = 0;

    return AddressToHash160(address.toStdString(), hash160);
}
