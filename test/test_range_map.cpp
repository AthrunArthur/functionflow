#include "ff.h"
#include <map>
#include <iterator>
#include <type_traits>
#include <functional>
#include "common/any.h"

struct iterator_range
{
    ff::any_value begin;
    ff::any_value end;
    template<class Iterator_t>
    iterator_range(const Iterator_t & b, const Iterator_t e)
            : begin(b)
            , end(e){};
};
struct default_iterator_range_comparator{
    static bool cmp(const iterator_range & a, const iterator_range & b)
    {
        return (&a < &b);
    };
};
template <class T>
struct iterator_range_comparator{
    static bool cmp(const iterator_range & a, const iterator_range & b)
    {
        std::cout<<"cmp"<<std::endl;
        if(a.begin.get<T>() < b.begin.get<T>() && a.end.get<T>() <= b.begin.get<T>())
            return true;
        return false;
    };
};
typedef std::map<iterator_range, int,
        std::function<bool (const iterator_range & r1, const iterator_range & r2) > > RangeMap_t;

int main(int argc, char *argv[])
{
    //RangeMap_t rm(default_iterator_range_comparator::cmp);
    RangeMap_t rm (iterator_range_comparator<int>::cmp);

    rm = RangeMap_t(iterator_range_comparator<int>::cmp);

    for(int i = 0; i < 100; i+=10)
    {
        iterator_range ir(i, i+10);
        rm[ir] = 10;
    }

    std::cout<<"rm [10]"<<rm[iterator_range(10, 10)]<<std::endl;
    std::cout<<"rm [11]"<<rm[iterator_range(11, 11)]<<std::endl;
    std::cout<<"rm [35]"<<rm[iterator_range(35, 35)]<<std::endl;
    return 0;
}
