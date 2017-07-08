#include "Field.hpp"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

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

    emit solverRunningChanged(arg);

    if(arg)
        runNextScedule();

}

void Field::click(const minesweeper::engine::square_board::Topology::index_type &index)
{
    if(!_data->openField(index))
        return;

    sceduleProbablityUpdate();
    _cells->emitDataChange();
}

Field::Field(QObject *parent)
    : QObject(parent)
    , _data(new Data(24, 24, 99))
    , _cells(new Cells(_data.get(), this))
{

}

Field::~Field()
{
    if(_worker_thread.joinable())
        _worker_thread.join();
}

QSize Field::size() const
{
    return {_data->topology().m_dims.first, _data->topology().m_dims.second};
}

void Field::init(const QSize &size, int bombCount)
{
    _cells->clear();
    auto newData = std::make_shared<Data>(size.width(), size.height(), bombCount);
    _data = newData;
    emit sizeChanged(size);
    _cells->resetEngine(_data.get());
    setSolverRunning(false);
    runNextScedule();
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
