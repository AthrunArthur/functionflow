#include "parameter_parser.h"

ParamParser::ParamParser()
  : m_desc("Allowed options:")
  , m_thrd_num(0){
  }

void ParamParser::parse(int argc, char * argv[])
{
  m_desc.add_options()
    ("help", "print the help message")
    ("thrd-num", bpo::value<int>(&m_thrd_num)->default_value(0),
       "thread number, when it's 0, the program is sequential, default is 0");
  bpo::store(bpo::parse_command_line(argc, argv, m_desc), m_vm);
  bpo::notify(m_vm);

  if(m_vm.count("help"))
  {
    std::cout<<m_desc<<"\n";
    exit(-1);
  }
}

