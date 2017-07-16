#ifndef CELLS_HPP
#define CELLS_HPP

#include <QAbstractListModel>

#include "engineTypes.hpp"

#include <QVector>
#include <QPoint>


class Field;

class Cells : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        BombNearRole = Qt::UserRole,
        ProbalityRole,
        FlaggedRole,
        OpenedRole,
        SafeRole,
        PositionRole,
        RowRole,
        ColumnRole,
        UserRole
    };

    using GameEngine = ::GameEngine;

public:
    explicit Cells(GameEngine* engine, QObject *parent = 0);
    void resetEngine(GameEngine* engine);
    void clear();
signals:

public slots:
    void emitDataChange(const QVector<int> roles = {});
    void emitPorabablitesChange();
    void emitFlagChange(const GameEngine::topology_type::index_type& cell);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    using CellsContainer = QVector<GameEngine::topology_type::index_type>;

    GameEngine* m_engine = nullptr;
    CellsContainer m_cells;
};

#endif // CELLS_HPP
