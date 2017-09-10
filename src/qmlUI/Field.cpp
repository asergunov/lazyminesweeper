#include "Field.hpp"

#include "Cells.hpp"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QApplication>
#include <QSettings>

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

    emit solverRunningChanged(arg);

    if(arg)
        runNextScedule();

}

void Field::click(const minesweeper::engine::square_board::Topology::index_type &index)
{
    if(!_data->openField(index))
        return;

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
    emit gameOver(win);
}

namespace  {
static const auto keySize = "size";
static const auto keyLoseIndex = "loseIndex";
static const auto keyPosition = "position";
static const auto keyBombs = "bombs";
static const auto keyFlags = "flags";
static const auto keyOpened = "opened";
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

    if(s.contains(keyLoseIndex)) {
        data.openField(toIndex(s.value(keyLoseIndex).toPoint()));
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
