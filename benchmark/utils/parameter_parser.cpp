#include "parameter_parser.h"
#include <iostream>

ParamParser::ParamParser(){

  add_option("thrd-num", "[default 0] thread number, when the value is 0, the program is sequential");
  m_opt_vals["thrd-num"] = std::string("0");
}

void ParamParser::add_option(const std::string & name, const std::string &desc)
{
  m_opt_desc.insert(std::make_pair(name, desc));
}

std::string ParamParser::options_to_str()
{
  std::stringstream ss;
  ss<<"Usage : \n";
  for(SMap_t::iterator it = m_opt_desc.begin(); it != m_opt_desc.end(); ++it)
  {
    ss<<"\t--"<<it->first<<"\t\t"<<it->second<<"\n";
  }
  return ss.str();
}

void ParamParser::parse(int argc, char * argv[])
{
  std::vector<std::string> args;
  for(int i = 0; i < argc; i++)
    args.push_back(std::string(argv[i]));


  std::string ls;
  bool contain_error = false;
  for(int i = 1; i < args.size(); ++i)
  {
    std::string s = args[i];
    ls = s;
    size_t p1 = s.find("--");
    if(p1 != 0)
    {
      contain_error = true;
      break;
    }
    std::string k = s.substr(p1 +2);
    if(m_opt_desc.find(k) == m_opt_desc.end())
    {
      contain_error = true;
      break;
    }
    if(k == std::string("help"))
    {
      std::cout<<options_to_str()<<std::endl;
      return ;
    }
    else if(i == args.size() -1)
    {
      m_set_opts.insert(k);
    }
    else
    {
      s = args[i+1];
      ls = s;
      p1 = s.find("--");
      if(p1 == 0)
      {
        k = s.substr(p1 +2);
        if(m_opt_desc.find(k) == m_opt_desc.end())
        {
          contain_error = true;
          break;
        }
        m_set_opts.insert(k);
        continue;
      }
      else
      {
        m_opt_vals[k] = s;
        i++;
        continue;
      }
    }
  }

  if(contain_error)
  {
    std::cout<<"Unknow parameter "<<ls<<std::endl;
    std::cout<<options_to_str()<<std::endl;
  }
}


