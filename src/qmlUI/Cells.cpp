#include "Cells.hpp"

Cells::Cells(QObject *parent) : QAbstractListModel(parent)
{
}

int Cells::rowCount(const QModelIndex &parent) const
{
    return 0;
}

QVariant Cells::data(const QModelIndex &index, int role) const
{
    return {};
}

QHash<int, QByteArray> Cells::roleNames() const
{
    return {
        {BombNearRole, "bombNearCount"},
        {ProbalityRole, "probablity"},
        {FlaggedRole, "flagged"},
        {OpenedRole, "opened"},
        {SafeRole, "safe"}
    };
}
