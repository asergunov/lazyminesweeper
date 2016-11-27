#pragma once

#include <set>
#include <algorithm>
#include <ostream>

namespace minesweeper {
namespace engine {

namespace square_board {

using coord_type = int16_t;

struct Index: public std::pair<coord_type, coord_type> {
    Index() {}
    Index(const coord_type& f, const coord_type& s) :pair(f,s) {}

    Index operator+(const Index& x) const {
        return Index{first+x.first, second+x.second};
    }

    friend std::ostream& operator << (std::ostream& o, const Index& i) {
        return o << "(" << i.first << ", " << i.second <<")";
    }
};

struct IndexSet: public std::set<Index> {
    IndexSet() {}
    IndexSet(std::initializer_list<value_type> il,
             const key_compare& comp = key_compare(),
             const allocator_type& alloc = allocator_type()) : set(il, comp, alloc) {}

    IndexSet& operator-=(const IndexSet& x) {
        if(this == &x)
            clear();
        else {
            for(auto i:x) {
                iterator r = find(i);
                if(r != end())
                    erase(r);
            }
        }
        return *this;
    }


    IndexSet moved(const Index& x) const {
        IndexSet ret;
        for(iterator i=begin(); i!=end(); ++i)
            ret.insert((*i)+x);
        return ret;
    }

    friend std::ostream& operator << (std::ostream& o, const IndexSet& x) {
        if(!x.empty()) {
            auto  i = x.cbegin();
            o << *i;
            ++i;
            std::for_each(i, x.end(), [&](const Index& i){ o << ", " << i; });
        }
        return o;
    }
};

struct Topology {
    typedef u_int8_t neighbour_count_type;
    using index_type = Index;
    using index_set_type = IndexSet;
    static const neighbour_count_type max_neighbours = 8;

    Index m_dims;

    struct const_iterator {
        Index _index;
        const Topology& _topology;
        const_iterator(const Index& index, const Topology& topology)
            : _index(index)
            , _topology(topology) {

        }

        const_iterator operator+(const size_t& d) {
            const auto t = _index.first*_topology.m_dims.second + _index.second + d;
            return {{t/_topology.m_dims.second, t%_topology.m_dims.second}, _topology};
        }

        const_iterator& operator++() {
            ++_index.second;
            if(_index.second >= _topology.m_dims.second) {
                _index.second = 0;
                ++_index.first;
            }
            return *this;
        }

        const Index& operator*() const {
            return _index;
        }

        bool operator!=(const const_iterator& other) const {
            return _index != other._index;
        }
    };

    const_iterator begin() const {
        return {{0,0}, *this};
    }

    const_iterator end() const {
        return {{m_dims.first, 0}, *this};
    }

    size_t cellsCount() const {
        return m_dims.first*m_dims.second;
    }

    explicit Topology(const Index& dims): m_dims(dims) {

    }

    Topology(const coord_type& w, const coord_type& h): m_dims(w,h) {

    }

    IndexSet neighbours(const Index& idx) const {
        static const IndexSet whole {
            {-1, -1}, { 0, -1}, { 1, -1},
            {-1,  0},           { 1,  0},
            {-1,  1}, { 0,  1}, { 1,  1}};

        static const IndexSet dUp = {{-1, -1}, {-1, 0}, {-1, 1}};
        static const IndexSet dDown = {{1, -1}, {1, 0}, {1, 1}};
        static const IndexSet dLeft = {{-1, -1}, {0, -1}, {1, -1}};
        static const IndexSet dRight = {{-1, 1}, {0, 1}, {1, 1}};

        IndexSet ret = whole;

        if(idx.first <= 0)
            ret -= dUp;
        if(idx.first >= m_dims.first-1)
            ret -= dDown;

        if(idx.second <= 0)
            ret -= dLeft;
        if(idx.second >= m_dims.second-1)
            ret -= dRight;
        return ret.moved(idx);
    }

};

}


}}

