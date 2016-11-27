#include "Field.hpp"

namespace {
    Field::Topology::index_type toIndex(const QPoint& p) {
        return {p.x(), p.y()};
    }
}

Field::Field(QObject *parent)
    : QObject(parent)
    , _data(new Data(10, 10, 0))
{

}

QSize Field::size() const
{
    return {_data->topology.m_dims.first, _data->topology.m_dims.second};
}

void Field::init(const QSize &size, int bombCount)
{
    _data.reset(new Data(size.width(), size.height(), bombCount));
    _data->porapablities = _data->solver.probablities(_data->topology, _data->player_data);
    emit sizeChanged(size);
}

bool Field::hasFlag(const QPoint &index) const
{
    return _data->player_data.hasFlag(toIndex(index));
}

qreal Field::minePorabablity(const QPoint &index) const
{
    const auto i = _data->porapablities.find(toIndex(index));
    return i == _data->porapablities.end() ? -1.0 : qreal(i->second.numerator())/i->second.denominator();
}

int Field::bombsNearCount(const QPoint &index) const
{
    const auto& opened = _data->player_data.openedItems();
    const auto i = opened.find(toIndex(index));
    return i==opened.end() ? -1 : i->second;
}

void Field::click(const QPoint &index)
{
    _data->private_data.openField(_data->player_data, toIndex(index), _data->topology);
    _data->porapablities = _data->solver.probablities(_data->topology, _data->player_data);
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
