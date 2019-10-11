#include "dialogtableitemmodel.h"

#include "data/shareddata.h"

DialogTableItemModel::DialogTableItemModel()
{

}

int DialogTableItemModel::rowCount(const QModelIndex &parent) const
{
    if (SharedData::get_instance()->currentDialog >= SharedData::get_instance()->dialog_list.size()) {
        return 0;
    }
    return SharedData::get_instance()->dialog_list.at(SharedData::get_instance()->currentDialog).size() +1; // one extra empty row for "new" entries
}

int DialogTableItemModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant DialogTableItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
            return QVariant();
    }

    if (SharedData::get_instance()->currentDialog >= SharedData::get_instance()->dialog_list.size()) {
        return QVariant();
    }

    if (index.row() >= SharedData::get_instance()->dialog_list.at(SharedData::get_instance()->currentDialog).size() || index.row() < 0) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            return std::to_string(index.row()).c_str();
        case 1: {
            std::string text = SharedData::get_instance()->dialog_list.at(SharedData::get_instance()->currentDialog).at(index.row());
            return text.c_str();
        }
        default:
            break;
        }
    }
    return QVariant();
}

QVariant DialogTableItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
            return QVariant();

        if (orientation == Qt::Horizontal) {
            switch (section) {
                case 0:
                    return tr("#");
                case 1:
                    return tr("Text");
                default:
                    break;
            }
        }
        return QVariant();
}

Qt::ItemFlags DialogTableItemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }
    if (index.column() == 0) {
        return QAbstractTableModel::flags(index);
    }

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool DialogTableItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (SharedData::get_instance()->currentDialog >= SharedData::get_instance()->dialog_list.size()) {
        return false;
    }

    if (index.row() > SharedData::get_instance()->dialog_list.at(SharedData::get_instance()->currentDialog).size() || index.row() < 0) {
        return false;
    }



    if (index.isValid() && role == Qt::EditRole) {
            switch (index.column()) {
                case 1: {
                    // new entry (last row that is empty by default)
                    if (index.row() == SharedData::get_instance()->dialog_list.at(SharedData::get_instance()->currentDialog).size() || index.row() < 0) {
                        SharedData::get_instance()->dialog_list.at(SharedData::get_instance()->currentDialog).push_back(std::string(""));
                    }
                    SharedData::get_instance()->dialog_list.at(SharedData::get_instance()->currentDialog).at(index.row()) = value.toString().toStdString();
                    break;
                }
                default:
                    return false;
            }
            emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

            return true;
        }

        return false;
}

bool DialogTableItemModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    if (SharedData::get_instance()->currentDialog >= SharedData::get_instance()->dialog_list.size()) {
        return false;
    }


    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        SharedData::get_instance()->dialog_list.at(SharedData::get_instance()->currentDialog).push_back(std::string("NEW ROW"));
    }

    endInsertRows();
    return true;
}

bool DialogTableItemModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    if (SharedData::get_instance()->currentDialog >= SharedData::get_instance()->dialog_list.size()) {
        return false;
    }
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        SharedData::get_instance()->dialog_list.at(SharedData::get_instance()->currentDialog).erase(SharedData::get_instance()->dialog_list.at(SharedData::get_instance()->currentDialog).begin()+position);
    }

    endRemoveRows();
    return true;
}

void DialogTableItemModel::updateData()
{
    int rowCount = 0;
    int colCount = 0;
    if (SharedData::get_instance()->currentDialog < SharedData::get_instance()->dialog_list.size()) {
        rowCount = SharedData::get_instance()->dialog_list.at(SharedData::get_instance()->currentDialog).size();
    }
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(rowCount-1, 1);
    emit dataChanged ( topLeft, bottomRight );
    emit layoutChanged();
    // reload data from sharedData
}
