#ifndef FIELD_HPP
#define FIELD_HPP

#include <QObject>
#include <QSize>
#include <QPoint>
#include <QVariant>

#include "../engine/SquareBoard.hpp"
#include "../engine/PlayerBoardData.hpp"
#include "../engine/PrivateBoardData.hpp"
#include "../engine/Solver.hpp"

class Field : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSize size READ size NOTIFY sizeChanged)

public:
    using Topology = minesweeper::engine::square_board::Topology;
    using PlayerData = minesweeper::engine::solver::PlayerBoardData<Topology>;
    using PrivateData = minesweeper::engine::solver::PrivateBoardData<Topology>;
    using Solver = minesweeper::engine::solver::Solver<Topology>;

    struct Data {
        Topology topology;
        PlayerData player_data;
        PrivateData private_data;
        Solver solver;
        Solver::index_porapablities porapablities;

        Data(size_t w, size_t h, size_t bombs);
    };

private:
    std::unique_ptr<Data> _data;

public:
    explicit Field(QObject *parent = 0);
    QSize size() const;

    Q_INVOKABLE void init(const QSize& size, int bombCount);
    Q_INVOKABLE bool hasFlag(const QPoint& index) const;
    Q_INVOKABLE qreal minePorabablity(const QPoint& index) const;
    Q_INVOKABLE int bombsNearCount(const QPoint& index) const;
    Q_INVOKABLE void click(const QPoint& index);


signals:
    void sizeChanged(const QSize&);

};

#endif // FIELD_HPP
