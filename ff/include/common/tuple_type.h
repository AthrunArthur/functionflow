#ifndef FF_COMMON_TUPLE_TYPE_H_
#define FF_COMMON_TUPLE_TYPE_H_
#include "function_traits.h"

namespace ff {
namespace utils {
template<typename... Types>
struct merge_tuples {
    typedef std::tuple<Types...> type;
};

template<>
struct merge_tuples<> {
    typedef std::tuple< > type;
};

template<typename Type>
struct merge_tuples<Type> {
    typedef std::tuple<Type> type;
};

template<typename...Types>
struct merge_tuples<std::tuple<Types...>> {
    typedef std::tuple<Types...> type;
};

template<typename...Types1,typename...Types2>
struct merge_tuples<std::tuple<Types1...>,std::tuple<Types2...>> {
    typedef std::tuple<Types1...,Types2...> type;
};

template<typename Type1,typename Type2>
struct merge_tuples<Type1,Type2> {
    typedef std::tuple<Type1,Type2> type;
};

template<typename Type,typename... Types>
struct merge_tuples<Type,std::tuple<Types...>> {
    typedef std::tuple<Type,Types...> type;
};

template<typename... Types,typename Type>
struct merge_tuples<std::tuple<Types...>,Type> {
    typedef std::tuple<Types...,Type> type;
};

template<typename...Types,typename Type,typename... Rest>
struct merge_tuples<std::tuple<Types...>,Type,Rest...> {
    typedef typename merge_tuples<Rest...>::type temp;
    typedef typename merge_tuples<std::tuple<Types...,Type>,temp>::type type;
};

template<typename Type,typename... Types,typename... Rest>
struct merge_tuples<Type,std::tuple<Types...>,Rest...> {
    typedef typename merge_tuples<Rest...>::type temp;
    typedef typename merge_tuples<std::tuple<Type,Types...>,temp>::type type;
};

template<typename...Types1,typename... Types2,typename... Rest>
struct merge_tuples<std::tuple<Types1...>,std::tuple<Types2...>,Rest...> {
    typedef typename merge_tuples<Rest...>::type temp;
    typedef typename merge_tuples<std::tuple<Types1...,Types2...>,temp>::type type;
};

template<typename Type1,typename Type2,typename... Rest>
struct merge_tuples<Type1,Type2,Rest...> {
    typedef typename merge_tuples<Rest...>::type temp;
    typedef typename merge_tuples<std::tuple<Type1,Type2>,temp>::type type;
};

}//end namespace utils
}//end namespace ff
#endif