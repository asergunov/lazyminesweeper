#include "Field.hpp"

#include "Cells.hpp"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QApplication>
#include <QSettings>
#include <QPoint>

namespace {
static const auto fieldSuspendStateGroup = "fieldSuspendState";
}

namespace {
    Field::Topology::index_type toIndex(const QPoint& p) {
        return {
            static_cast<Field::Topology::index_type::coord_type>(p.x()),
            static_cast<Field::Topology::index_type::coord_type>(p.y())
        };
    }

    QPoint toPoint(const Field::Topology::index_type& p) {
        return {p.first, p.second};
    }

    void run_in_thread(QThread* thread, std::function<void()> func) {
      Q_ASSERT(thread->isRunning());
      if(thread == QThread::currentThread())
        func(); // it's the same thread
      else {
        auto obj = new QObject(); ///< temporary object to trigger event in thread
        QWaitCondition done;      ///< wait condition which unlocks this thread on done
        QMutex mutex;             ///< the guard mutex to avoid run condition

        // lock the mutex
        QMutexLocker locker(&mutex);

        // scedule object delete
        obj->deleteLater();

        // connect to destroyed signal
        QObject::connect(obj, &QObject::destroyed, [&]{
          // lock the mutex
          QMutexLocker locker(&mutex);
          // call the function
          func();
          // stop waiting
          done.wakeAll();
        });
        // move object to another thread
        obj->moveToThread(thread);

        // unlock mutex and wait for done
        done.wait(&mutex);
      }
    }

}

void Field::sceduleProbablityUpdate()
{
    this->runNextScedule();
}

void Field::runNextScedule()
{
    if(isSolverRunning())
        return;

    if(_data->isPorabablitiesCalculated()) {
        if(_worker_thread.joinable())
            _worker_thread.detach(); // let it die

        setSolverRunning(true);

        _worker_thread =  Data::start_worker(_data, [this](const std::shared_ptr<Data>& data,
                                              size_t version,
                                              const Data::IntermediateData& intermediate,
                                              const Data::index_porapablities& porapablities) {
            run_in_thread(this->thread(), [&]{
                if(_data != data) {
                    // field was changed
                    return;
                }
                
                if(_data->merge_solution(version, intermediate, porapablities)) {
                    if(m_autoFlag) {
                        auto& player = _data->player();
                        for(const auto& prob_pair: _data->probablities()) {
                            if(prob_pair.second == 1) {
                                if(player.setFlag(prob_pair.first))
                                    _cells->emitFlagChange(prob_pair.first);
                            }
                        }
                        setBombRemains(_data->bombRemains());
                    }

                    _cells->emitPorabablitesChange();
                    setSolverRunning(false);
                    runNextScedule();
                }
            });
        });
    }
}

void Field::setSolverRunning(bool arg)
{
    if(arg == m_solverRunning)
        return;

    m_solverRunning = arg;

    if(arg)
        m_stats.machineStarted();
    else
        m_stats.machineStoped();


    emit solverRunningChanged(arg);

    if(arg)
        runNextScedule();

}

void Field::click(const minesweeper::engine::square_board::Topology::index_type &index)
{
    const auto& probablity = _data->porabablityDouble(index);

    if(!_data->openField(index))
        return;

    if(probablity > 0) {
        m_stats.riskTaken = 1.0 - (1.0 - m_stats.riskTaken) * (1.0 - probablity);
        emit riskTakenChanged();
    }

    if(_data->player().isGameOver()) {
        setGameOver(false);
        return;
    }

    sceduleProbablityUpdate();
    _cells->emitDataChange();
}

void Field::setBombRemains(const int& arg)
{
    if(arg == m_bombRemains)
        return;

    m_bombRemains = arg;
    emit bombRemainsChanged(arg);
    if(arg == 0)
        setGameOver(true);
}


Field::Field(QObject *parent)
    : QObject(parent)
    , _data(new Data(0, 0, 0))
    , _cells(new Cells(_data.get(), this))
    , m_timer(startTimer(500, Qt::CoarseTimer))
{
    auto writeSuspendState = [](const Field* field, QSettings& s) {
        Q_ASSERT(s.isWritable());
        s.beginGroup(fieldSuspendStateGroup);
        field->writeState(s);
        s.endGroup(); // fieldSuspendStateGroup
    };

    auto readSuspendState = [](Field* field, QSettings& s) {
        if(!s.childGroups().contains(fieldSuspendStateGroup))
            return;

        s.beginGroup(fieldSuspendStateGroup);
        field->restoreState(s);

        s.endGroup(); // fieldSuspendState
        s.remove("fieldSuspendState");
    };

    connect(qApp, &QApplication::applicationStateChanged, this, [this, writeSuspendState](Qt::ApplicationState state){
        if(state == Qt::ApplicationState::ApplicationSuspended) {
            QSettings s;
            writeSuspendState(this, s);
        } else if(state == Qt::ApplicationState::ApplicationHidden) {
            m_stats.pause();
        } else if(state == Qt::ApplicationState::ApplicationActive) {
            m_stats.resume();
        }
    });

    QSettings s;
    readSuspendState(this, s);
}

Field::~Field()
{
    QSettings s;
    s.beginGroup(fieldSuspendStateGroup);
    writeState(s);
    s.endGroup(); // fieldSuspendStateGroup

    if(_worker_thread.joinable())
        _worker_thread.join();
}

QSize Field::size() const
{
    return {_data->topology().m_dims.first, _data->topology().m_dims.second};
}

void Field::init(const QSize &size, int bombCount)
{
    setData(std::make_shared<Data>(size.width(), size.height(), bombCount));
    m_stats.clear();
    emit riskTakenChanged();
}


void Field::setData(std::shared_ptr<Field::Data> data)
{
    _data = std::move(data);
    emit sizeChanged(this->size());
    emit bombCountChanged(this->bombCount());
    _cells->resetEngine(_data.get());
    setSolverRunning(false);
    setBombRemains(_data->bombRemains());
    runNextScedule();
}

void Field::setGameOver(bool win)
{
    m_stats.pause();
    emit gameOver(win);
}

namespace  {
static const auto keySize = "size";
static const auto keyLoseIndex = "loseIndex";
static const auto keyPosition = "position";
static const auto keyBombs = "bombs";
static const auto keyFlags = "flags";
static const auto keyOpened = "opened";

static const auto keyTimeSpentByUser = "timeSpentByUser";
static const auto keyTimeSpentByMachine = "timeSpentByMachine";
static const auto keyRiskTaken = "riskTaken";
}

void Field::restoreState(QSettings &s)
{
    const auto size = s.value(keySize).toSize();

    auto readIndexSet = [&](const QString& name) {
        Data::index_set_type indexSet;
        const int count = s.beginReadArray(name);
        for(int i = 0; i<count; ++i) {
            s.setArrayIndex(i);
            indexSet.insert(toIndex( s.value(keyPosition).toPoint() ));
        }
        s.endArray();
        return indexSet;
    };

    const auto& bombs = readIndexSet(keyBombs);
    const auto& flags = readIndexSet(keyFlags);
    const auto& opened = readIndexSet(keyOpened);


    auto sharedData = std::make_shared<Data>(size.width(), size.height(), bombs);

    auto& data = *sharedData;
    auto& player = data.player();

    for(const auto& index: opened)
        data.openField(index);
    for(const auto& index: flags)
        player.setFlag(index);

    m_stats.reset(Stats::duration(s.value(keyTimeSpentByUser).toLongLong()),
                  Stats::duration(s.value(keyTimeSpentByMachine).toLongLong()),
                  s.value(keyRiskTaken).toReal());

    emit riskTakenChanged();
    if(s.contains(keyLoseIndex)) {
        data.openField(toIndex(s.value(keyLoseIndex).toPoint()));
        m_stats.pause();
    } else {
        m_stats.resume();
    }
    setData(sharedData);
}

void Field::writeState(QSettings& s) const
{
    const auto& data = *_data;
    const auto& player = data.player();

    if(player.isGameOver()) {
        s.setValue(keyLoseIndex, toPoint(player.looseIndex()));
    } else {
        s.remove(keyLoseIndex);
    }

    s.setValue(keySize, size());
    auto writeIndexSet = [&](const QString& name, const Data::index_set_type& indexSet) {
        int i = 0;
        s.beginWriteArray(name, static_cast<int>(indexSet.size()));
        for(const auto& index : indexSet) {
            s.setArrayIndex(i++);
            s.setValue(keyPosition, toPoint(index));
        }
        s.endArray();
    };

    auto keySet = [](const Data::PlayerData::opened_items_type& opened_map) {
        Data::index_set_type result;
        for(const auto& pair : opened_map) {
            result.insert(pair.first);
        }
        return result;
    };

    writeIndexSet(keyBombs, data.privateData().bombs);
    writeIndexSet(keyFlags, player.flags);
    writeIndexSet(keyOpened, keySet(player.openedItems()));

    s.setValue(keyTimeSpentByUser, QVariant::fromValue<qlonglong>(m_stats.timeSpentByHuman().count()));
    s.setValue(keyTimeSpentByMachine, QVariant::fromValue<qlonglong>(m_stats.timeSpentByMachine().count()));
    s.setValue(keyRiskTaken, m_stats.riskTaken);
}

void Field::click(const QPoint &index)
{
    click(toIndex(index));
    _cells->emitDataChange();
}

void Field::douleClick(const QPoint &_index)
{
    const auto& index = toIndex(_index);
    _data->openAllNear(index);
    sceduleProbablityUpdate();
    _cells->emitDataChange();
}

void Field::makeBestTurn()
{
    _data->makeBestTurn();
    _cells->emitDataChange();
    sceduleProbablityUpdate();
}

bool Field::isSolverRunning() const
{
    return m_solverRunning;
}

int Field::bombCount() const
{
    return _data->player().totalBombCount;
}

bool Field::isGameOver() const {
    return _data->player().isGameOver();
}

int Field::timeSpentByHuman() const
{
    using namespace std::chrono;
    return duration_cast<seconds>(m_stats.timeSpentByHuman()).count();
}

int Field::timeSpentByMachine() const
{
    using namespace std::chrono;
    return duration_cast<seconds>(m_stats.timeSpentByMachine()).count();
}

qlonglong Field::timeSpentByMachineMS() const
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(m_stats.timeSpentByMachine()).count();
}

void Field::Stats::clear()
{
    riskTaken = 0;
    using namespace std::chrono;
    _timeSpentByHuman = duration::zero();
    _timeSpentByMachine = duration::zero();
    userStartTimetamp = clock::now();
    machineWorking = false;
    humanCounting = true;
}

void Field::Stats::reset(const Field::Stats::duration &humanTime, const Field::Stats::duration &machineTime, const qreal riskTaken)
{
    _timeSpentByHuman = humanTime;
    _timeSpentByMachine = machineTime;
    this->riskTaken = riskTaken;
    machineStartTimetamp = userStartTimetamp = clock::now();
    humanCounting = true;
}

void Field::Stats::machineStarted() {
    if(machineWorking)
        return;

    machineWorking = true;
    const auto& now = clock::now();
    machineStartTimetamp = now;
    if(humanCounting)
        _timeSpentByHuman += now - userStartTimetamp;
}

void Field::Stats::machineStoped() {
    if(!machineWorking)
        return;

    machineWorking = false;
    const auto& now = clock::now();
    userStartTimetamp = now;
    _timeSpentByMachine += now - machineStartTimetamp;
}

void Field::Stats::resume()
{
    if(humanCounting)
        return;

    userStartTimetamp = clock::now();
}

void Field::Stats::pause()
{
    if(!humanCounting)
        return;

    humanCounting = false;

    const auto& now = clock::now();
    _timeSpentByHuman += now - userStartTimetamp;
}

Field::Stats::duration Field::Stats::timeSpentByMachine() const {
    auto result = _timeSpentByMachine;

    if(machineWorking) {
        result += clock::now() - machineStartTimetamp;
    }
    return result;
}

Field::Stats::duration Field::Stats::timeSpentByHuman() const {
    auto result = _timeSpentByHuman;

    if(humanCounting) {
        result += clock::now() - userStartTimetamp;
    }
    return result;
}

void Field::timerEvent(QTimerEvent *event)
{
    if(m_timer != event->timerId())
        return QObject::timerEvent(event);

    emit timeSpentByHumanChanged();
    emit timeSpentByMachineChanged();
}
