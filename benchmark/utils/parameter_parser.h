#ifndef BENCHMARK_PARAMETER_PARSER_H_
#define BENCHMARK_PARAMETER_PARSER_H_
#include "boost/program_options.hpp" 
#include <iostream>
#include <string>
namespace bpo = boost::program_options;

class ParamParser
{
  public:
    ParamParser();

    void parse(int argc, char *argv[]);

    template<class Ty>
    void add_option(const std::string & name, const std::string & desc)
    {
      m_desc.add_options()(name.c_str(), bpo::value<Ty>(), desc.c_str());
    }

    template<class Ty>
    Ty get(const std::string & name)
    {
      return m_vm[name].as<Ty>();
    }

    inline bool is_set(const std::string & name){return m_vm.count(name);}

  protected:
    bpo::options_description m_desc;
    bpo::variables_map m_vm;
};//end class ParamParser
#endif
