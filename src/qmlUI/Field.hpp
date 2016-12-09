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

#include <thread>

class Field : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSize size READ size NOTIFY sizeChanged)
    Q_PROPERTY(bool solverRunning READ isSolverRunning NOTIFY solverRunningChanged)

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
        Solver::IntermediateData intermediate;

        Solver::index_porapablities porapablities;

        size_t porapablitiesVersion = 0;
        size_t dataVersion = 1;

        Data(size_t w, size_t h, size_t bombs);
    };

private:
    std::shared_ptr<Data> _data;
    std::thread _worker_thread;
    bool m_solverRunning = false;

private:
    void sceduleProbablityUpdate();
    void runNextScedule();
    void setSolverRunning(bool);

    void click(const Field::Topology::index_type& index);

public:
    explicit Field(QObject *parent = 0);
    ~Field();
    QSize size() const;

    Q_INVOKABLE void init(const QSize& size, int bombCount);
    Q_INVOKABLE bool hasFlag(const QPoint& index) const;
    Q_INVOKABLE qreal minePorabablity(const QPoint& index) const;
    Q_INVOKABLE int bombsNearCount(const QPoint& index) const;
    Q_INVOKABLE void click(const QPoint& index);
    Q_INVOKABLE void douleClick(const QPoint& index);
    Q_INVOKABLE void makeBestTurn();


    bool isSolverRunning() const;

signals:
    void sizeChanged(const QSize&);
    void probablitiesChanged();
    void valuesChanged(const QPoint& index);

    void solverRunningChanged(bool solverRunning);
};

#endif // FIELD_HPP
