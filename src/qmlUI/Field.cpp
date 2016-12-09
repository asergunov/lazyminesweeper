#include "Field.hpp"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

namespace {
    Field::Topology::index_type toIndex(const QPoint& p) {
        return {p.x(), p.y()};
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
    ++_data->dataVersion;
    this->runNextScedule();
}

void Field::runNextScedule()
{
    if(isSolverRunning())
        return;

    if(_data->dataVersion > _data->porapablitiesVersion) {
        if(_worker_thread.joinable())
            _worker_thread.detach(); // let it die

        const auto data = _data;
        const auto player_data = _data->player_data;
        auto Intermediate = _data->intermediate;
        auto version = _data->dataVersion;
        auto scedule = [data, player_data, version, Intermediate, this]() mutable {
            auto porapablities = data->solver.probablities(data->topology, player_data, Intermediate);
            run_in_thread(this->thread(), [&]{
                if(_data != data) {
                    // field was changed
                    return;
                }

                if(_data->porapablitiesVersion < version) {
                    _data->porapablities = std::move(porapablities);
                    _data->porapablitiesVersion = version;
                    _data->intermediate.merge(Intermediate);

                    emit this->probablitiesChanged();

                    setSolverRunning(false);
                    runNextScedule();
                }
            });
        };

        setSolverRunning(true);
        _worker_thread =  std::thread(scedule);
    }
}

void Field::setSolverRunning(bool arg)
{
    if(arg == m_solverRunning)
        return;

    m_solverRunning = arg;

    emit solverRunningChanged(arg);

    if(arg)
        runNextScedule();

}

Field::Field(QObject *parent)
    : QObject(parent)
    , _data(new Data(10, 10, 0))
{

}

Field::~Field()
{
    if(_worker_thread.joinable())
        _worker_thread.join();
}

QSize Field::size() const
{
    return {_data->topology.m_dims.first, _data->topology.m_dims.second};
}

void Field::init(const QSize &size, int bombCount)
{
    _data.reset(new Data(size.width(), size.height(), bombCount));
    sceduleProbablityUpdate();
    emit sizeChanged(size);
}

bool Field::hasFlag(const QPoint &index) const
{
    return _data->player_data.hasFlag(toIndex(index));
}

qreal Field::minePorabablity(const QPoint &index) const
{
    const auto i = _data->porapablities.find(toIndex(index));
    return i == _data->porapablities.end() ? -1.0 : Solver::toDouble(i->second);
}

int Field::bombsNearCount(const QPoint &index) const
{
    const auto& opened = _data->player_data.openedItems();
    const auto i = opened.find(toIndex(index));
    return i==opened.end() ? -1 : i->second;
}

void Field::click(const QPoint &index)
{
    if(_data->player_data.isOpened(toIndex(index)))
        return;

    _data->private_data.openField(_data->player_data, toIndex(index), _data->topology);
    emit valuesChanged();

    sceduleProbablityUpdate();
}

void Field::douleClick(const QPoint &_index)
{
    const auto& index = toIndex(_index);
    const auto& opened = _data->player_data.openedItems();
    const auto i = opened.find(index);

    if(i==opened.end())
        return;

    bool changed = false;
    for(const auto& ni : _data->topology.neighbours(index)) {
        if(_data->intermediate.isClear(ni) && !_data->player_data.isOpened(ni)) {
            _data->private_data.openField(_data->player_data, ni, _data->topology);
            changed = true;
        }
    }

    if(changed) {
        emit valuesChanged();
        sceduleProbablityUpdate();
    }
}

bool Field::isSolverRunning() const
{
    return m_solverRunning;
}

Field::Data::Data(size_t w, size_t h, size_t bombs)
    : topology(w, h)
    , player_data()
    , private_data()
    , solver(topology)
{
    player_data.totalBombCount = bombs;
    private_data.generate(topology, bombs);
}
