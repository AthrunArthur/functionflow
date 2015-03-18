#include "common.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "TreeMaker.h"
#include "utils.h"


using namespace std;


int main( int argc, char *argv[] ) {

  RRecord rr("time.json", "treesum");
  ParamParser pp;
  pp.parse(argc, argv);
  int thrd_num = pp.get<int>("thrd-num");
  rr.put("thrd-num", thrd_num);
  bool bIsPara = (thrd_num > 0);
  if(thrd_num > 0)
    initial_para_env(thrd_num);

  long number_of_nodes = 100000000;


  TreeNode* root = TreeMaker::create(number_of_nodes);

  // Warm up caches
  SerialSumTree(root);
  if(!bIsPara)
  {
    rr.run("elapsed-time", SerialSumTree, root);
  }
  else{
  rr.run("elapsed-time", ParaSumTree, root);
  }
  return 0;
}
