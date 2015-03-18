#include "../matrix.h"
#include "ff.h"

using namespace ff;

void para_mul(const Matrix & m1, const Matrix & m2, Matrix & res)
{
  int blocks = m1.M()/Matrix::block_size;
  if(m1.M() % Matrix::block_size != 0)
    blocks ++;

  std::vector<ff::spinlock * > ls;
  ff::paracontainer pc;
  for(int k = 0; k < blocks; k++)
  {
    for(int i = 0; i < blocks; i++)
    {
      ls.push_back(new ff::spinlock);
    }
  }
  for(int k = 0; k < blocks; k++){
    for(int i = 0; i < blocks; i++)
    {
      for(int j = 0; j < blocks; j++){
      para<> a;
      a([&ls, blocks, k, i, j, &m1, &m2, &res](){
          GeneralMatrix tmp(Matrix::block_size, Matrix::block_size);
          auto tm1 = get_block(m1, k, j);
          auto tm2 = get_block(m2, j, i);
          mul(tm1, tm2, tmp);
          auto sum = get_block(res, k, i);
          ls[k*blocks + i]->lock();
          add(sum, tmp, sum);
          ls[k*blocks + i]->unlock();
          });
      pc.add(a);
      }
    }
  }

  ff_wait(all(pc));
}
