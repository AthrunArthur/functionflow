#include "parameter_parser.h"
#include <iostream>
int main(int argc, char *argv[])
{
  ParamParser pp;
  pp.parse(argc, argv);
  if(pp.is_set("thrd-num"))
    std::cout<<pp.get<int>("thrd-num")<<std::endl;
  return 0;
}
