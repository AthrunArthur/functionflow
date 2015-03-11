#ifndef BENCHMARK_PARAMETER_PARSER_H_
#define BENCHMARK_PARAMETER_PARSER_H_
#include <string>
#include <sstream>
#include <cstdlib>
#include <map>
#include <vector>
#include <set>


class ParamParser
{
  public:
    ParamParser();

    void parse(int argc, char *argv[]);

    void add_option(const std::string & name, const std::string & desc);

    template<class Ty>
    Ty get(const std::string & name)
    {
      std::stringstream ss;
      Ty t;
      ss<<m_opt_vals[name];
      ss>>t;
      return t;
    }

    inline bool is_set(const std::string & name){
      return m_opt_vals.find(name) != m_opt_vals.end() || m_set_opts.find(name) != m_set_opts.end();
    }

  protected:
    std::string options_to_str();

  protected:
    typedef std::map<std::string, std::string> SMap_t;

    std::map<std::string, std::string > m_opt_desc;
    std::map<std::string, std::string> m_opt_vals;
    std::set<std::string> m_set_opts;
};//end class ParamParser
#endif
