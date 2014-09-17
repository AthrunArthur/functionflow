#include <iostream>
#include <fstream>

#include <sstream>
#include <vector>
#include <string>
#include <cstdint>

using namespace std;
typedef int32_t thrd_id_t;


  struct record{
    enum{
      op_pop = 1,
      op_push = 2,
      op_steal = 3,
    };
    thrd_id_t   qid;
    thrd_id_t   op_tid;
    int op;
    int64_t h;
    int64_t t;
    int64_t rid;
    bool op_res;
    record(){}

    record(int64_t prid, thrd_id_t pqid, thrd_id_t pop_tid, int pp, int ph, int pt)
      : qid(pqid)
      , rid(prid)
      , op_tid(pop_tid)
      , op(pp)
      , h(ph)
      , t(pt){}
    
    string print()
    {
      stringstream ss;
      ss<<"queue_id:"<<qid;
      ss<<"\top_thrd_id:"<<op_tid;
      ss<<"\top:"<<op;
      ss<<"\th:"<<h;
      ss<<"\tt:"<<t;
      ss<<"\trid:"<<rid;
      ss<<"\top_res:"<<op_res;

      ///std::cout<<ss.str()<<std::endl;
      return ss.str();
    }
  };
const char *base_fp = "wsr.dat";
int thrd_num = 8;

vector<record> read_one_thrd(int i)
{
  stringstream ss;
  ss<<i<<base_fp;
  ifstream f(ss.str(), std::ios::in | std::ios::binary);
  vector<record> res;
  record r;
  while(f.read((char *) &r, sizeof(record)))
  {
    res.push_back(r);
  }
  f.close();
  return res;
}

void    dump()
{
  ofstream f("queue_acc.dat", std::ios::out);
  for(int i = 0; i < thrd_num; ++i)
  {
    vector<record> rs = read_one_thrd(i);
    for(int j = 0; j < rs.size(); ++j)
    {
      f<<rs[j].print()<<std::endl;
    }
  }
  f.close();
}



int main(int argc, char *argv[])
{
  dump();
  return 0;
}
