#include "Cells.hpp"

Cells::Cells(GameEngine *engine, QObject *parent)
    : QAbstractListModel(parent)
    , m_engine(nullptr)
{
    resetEngine(engine);
}

void Cells::resetEngine(Cells::GameEngine *engine)
{
    auto createCellList = [this]{
        CellsContainer result;
        const auto& topology = m_engine->topology();
        for(const auto& index : topology) {
            result.push_back(index);
        }
        return result;
    };

    beginResetModel();
    m_engine = engine;
    m_cells = createCellList();
    endResetModel();
}

void Cells::clear()
{
    if(m_cells.empty())
        return;
    beginRemoveRows({}, 0, m_cells.size()-1);
    m_cells.clear();
    endRemoveRows();
}

void Cells::emitDataChange(const QVector<int> roles)
{
    if(!m_cells.empty())
        emit dataChanged(index(0,0), index(m_cells.size()-1, 0), roles);
}

void Cells::emitPorabablitesChange()
{
    emitDataChange(QVector<int>() << ProbalityRole << SafeRole);
}

void Cells::emitFlagChange(const GameEngine::topology_type::index_type &cell)
{
    auto row = m_cells.indexOf(cell);
    if(row >= 0)
        emit dataChanged(index(row, 0), index(row, 0), {FlaggedRole});
}

int Cells::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return m_cells.size();
}

QVariant Cells::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return {};

    const auto &cell = m_cells[index.row()];

    switch(static_cast<Roles>(role)) {
    case Cells::PositionRole:
        return QPoint(cell.first, cell.second);
    case Cells::RowRole:
        return static_cast<int>(cell.second);
    case Cells::ColumnRole:
        return static_cast<int>(cell.first);
    case Cells::BombNearRole: {
        const auto& opened = m_engine->player().openedItems();
        const auto i = opened.find(cell);
        return i==opened.end() ? QVariant() : QVariant::fromValue<int>(i->second);
    }
    case Cells::ProbalityRole:
        return m_engine->porabablityDouble(cell);
    case Cells::FlaggedRole:
        return m_engine->player().hasFlag(cell);
    case Cells::OpenedRole: {
        const auto& opened = m_engine->player().openedItems();
        const auto i = opened.find(cell);
        return i != opened.end();
    }
    case Cells::SafeRole:
        return m_engine->porabablity(cell) == 0;
    case Cells::ExplodedRole:
        return m_engine->player().isExploded(cell);
    }

    return {};
}

QHash<int, QByteArray> Cells::roleNames() const
{
    return {
        {BombNearRole, "nearBombCount"},
        {ProbalityRole, "probablity"},
        {FlaggedRole, "flagged"},
        {OpenedRole, "opened"},
        {SafeRole, "safe"},
        {PositionRole, "position"},
        {RowRole, "row"},
        {ColumnRole, "column"},
        {ExplodedRole, "exploded"}
    };
}
