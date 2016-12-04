#pragma once

#include <vector>
#include <map>

namespace minesweeper {
namespace engine {
namespace solver {

template <typename Bound, typename VaraitionCount>
struct Specter : public std::vector<std::map<Bound, VaraitionCount>> {
    using bound_type = Bound;
    using variation_count_type = VaraitionCount;

    variation_count_type total;

    Specter(const size_t& s)
        : std::vector<std::map<bound_type, variation_count_type>>(s)
        , total(0)
    {

    }

    static Specter fromTrivialNone() {
        Specter r(0);
        r.total = 0;
        return r;
    }

    static Specter fromTrivialOne() {
        Specter r(0);
        r.total = 1;
        return r;
    }

    static Specter fromTrivialValueNone() {
        Specter r(1);
        r.total = 0;
        return r;
    }

    static Specter fromTrivialValue(const bound_type& value, const variation_count_type& count) {
        Specter r(1);
        r.total = count;
        if(count != 0)
            r[0].insert({value, count});
        return r;
    }
}; // specter[column][value] = variant_count

}}}
