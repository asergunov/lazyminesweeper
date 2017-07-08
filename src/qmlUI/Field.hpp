#ifndef FIELD_HPP
#define FIELD_HPP

#include "Cells.hpp"
#include "engineTypes.hpp"

#include <QObject>
#include <QSize>
#include <QPoint>
#include <QVariant>

#include <thread>

class Field : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSize size READ size NOTIFY sizeChanged)
    Q_PROPERTY(bool solverRunning READ isSolverRunning NOTIFY solverRunningChanged)
    Q_PROPERTY(Cells* cells READ cells CONSTANT)
    Q_PROPERTY(int bombRemains READ bombRemains NOTIFY bombRemainsChanged)

public:
    using Topology = ::Topology;
    using Data = ::GameEngine;



public:
    explicit Field(QObject *parent = 0);
    ~Field();
    QSize size() const;
    Cells* cells() const { return _cells; }

    Q_INVOKABLE void init(const QSize& size, int bombCount);
    Q_INVOKABLE void click(const QPoint& index);
    Q_INVOKABLE void douleClick(const QPoint& index);
    Q_INVOKABLE void makeBestTurn();


    bool isSolverRunning() const;

    int bombRemains() const
    {
        return m_bombRemains;
    }

signals:
    void sizeChanged(const QSize&);
    void probablitiesChanged();
    void solverRunningChanged(bool solverRunning);
    void bombRemainsChanged(int bombRemains);

private:
    void sceduleProbablityUpdate();
    void runNextScedule();
    void setSolverRunning(bool);
    void click(const Field::Topology::index_type& index);
    void setBombRemains(const int &arg);

private:
    std::shared_ptr<Data> _data;
    std::thread _worker_thread;
    bool m_solverRunning = false;
    Cells* _cells = nullptr;
    bool m_autoFlag = true;
    int m_bombRemains = 0;
};

#endif // FIELD_HPP
