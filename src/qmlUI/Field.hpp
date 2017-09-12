#ifndef FIELD_HPP
#define FIELD_HPP

#include "Cells.hpp"
#include "engineTypes.hpp"

#include <QObject>
#include <QSize>
#include <QTime>

#include <thread>
#include <chrono>

class QSettings;
class QPoint;

class Field : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSize size READ size NOTIFY sizeChanged)
    Q_PROPERTY(bool solverRunning READ isSolverRunning NOTIFY solverRunningChanged)
    Q_PROPERTY(Cells* cells READ cells CONSTANT)
    Q_PROPERTY(int bombRemains READ bombRemains NOTIFY bombRemainsChanged)
    Q_PROPERTY(int bombCount READ bombCount NOTIFY bombCountChanged)
    Q_PROPERTY(bool isGameOver READ isGameOver NOTIFY gameOver)

    // stats
    Q_PROPERTY(qreal riskTaken READ riskTaken NOTIFY riskTakenChanged)
    Q_PROPERTY(int timeSpentByHuman READ timeSpentByHuman NOTIFY timeSpentByHumanChanged)
    Q_PROPERTY(int timeSpentByMachine READ timeSpentByMachine NOTIFY timeSpentByMachineChanged)
    Q_PROPERTY(qlonglong timeSpentByMachineMS READ timeSpentByMachineMS NOTIFY timeSpentByMachineChanged)

public:
    using Topology = ::Topology;
    using Data = ::GameEngine;

public:
    explicit Field(QObject *parent = 0);
    ~Field();
    QSize size() const;
    Cells* cells() { return _cells; }
    const Cells* cells() const { return _cells; }

    Q_INVOKABLE void init(const QSize& size, int bombCount);
    Q_INVOKABLE void click(const QPoint& index);
    Q_INVOKABLE void douleClick(const QPoint& index);
    Q_INVOKABLE void makeBestTurn();


    bool isSolverRunning() const;

    const int& bombRemains() const { return m_bombRemains; }
    int bombCount() const;
    bool isGameOver() const;

    const qreal& riskTaken() const
    {
        return m_stats.riskTaken;
    }

    int timeSpentByHuman() const;
    int timeSpentByMachine() const;
    qlonglong timeSpentByMachineMS() const;

signals:
    void sizeChanged(const QSize&);
    void probablitiesChanged();
    void solverRunningChanged(bool solverRunning);
    void bombRemainsChanged(int bombRemains);
    void bombCountChanged(int bombCount);
    void gameOver(bool win);

    void riskTakenChanged();
    void timeSpentByHumanChanged();
    void timeSpentByMachineChanged();

    // QObject interface
protected:
    void timerEvent(QTimerEvent *event) override;

private:
    void scheduleProbablityUpdate();
    void runNextschedule();
    void setSolverRunning(bool);
    void click(const Field::Topology::index_type& index);
    void setBombRemains(const int &arg);
    void setData(std::shared_ptr<Data> data);
    void setGameOver(bool win);

    void writeState(QSettings& s) const;
    void restoreState(QSettings& s);

private:
    std::shared_ptr<Data> _data;
    std::thread _worker_thread;
    bool m_solverRunning = false;
    Cells* _cells = nullptr;
    bool m_autoFlag = true;
    int m_bombRemains = 0;

    struct Stats {
        using clock = std::chrono::steady_clock;
        using duration = clock::duration;

        qreal riskTaken;

        void clear();
        void reset(const duration& humanTime, const duration& machineTime, const qreal riskTaken);

        void machineStarted();
        void machineStoped();

        void pause();
        void resume();

        duration timeSpentByMachine() const;
        duration timeSpentByHuman() const;


    private:
        bool machineWorking = false;
        bool humanCounting = true;

        duration _timeSpentByHuman;
        duration _timeSpentByMachine;

        clock::time_point machineStartTimetamp;
        clock::time_point userStartTimetamp;
    } m_stats;
    int m_timer;

};



#endif // FIELD_HPP
