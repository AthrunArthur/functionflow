#include "ff.h"

#include "common/any.h"


int main(int argc, char * argv[])
{
    int v1 = 10;
    double v2 = 20.56;
    std::vector<ff::any_value>   vs;
    vs.push_back(ff::any_value(v1));
    vs.push_back(ff::any_value(v2));

    std::cout<<vs[0].get<int>()<<std::endl;
    std::cout<<vs[1].get<double>()<<std::endl;
    return 0;
}