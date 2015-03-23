#include "matrix.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ff.h>
using namespace std;


#define MSIZE 256
//#define MSIZE 2048

template <class M1, class M2>
bool check_Matrix(const M1 & m1, const M2& m2) {
  bool equal = true;
  for(int i = 0; i<m1.M(); i++)
    for(int j = 0; j< m1.N(); j++)
    {
      if(std::abs(m1(i, j) - m2(i, j) ) / std::abs(m2(i, j)) > 0.1)
      {
        equal = false;
      }
    }
  return equal;
}

  template <class M1, class M2>
void check_LU_res(const M1 & m, const M2& lu)
{
  GeneralMatrix l(m.M(), m.N());
  GeneralMatrix u(m.M(), m.N());
  for(int i = 0; i < lu.M(); i++)
  {
    for(int j = 0; j< lu.N(); j++)
    {
      if(i>j)
      {
        l(i, j) = lu(i, j);
        u(i, j) = 0;
      }
      else if(i == j)
      {
        l(i, j) = 1;
        u(i, j) = lu(i, j);
      }
      else
      {
        l(i, j) = 0;
        u(i, j) = lu(i, j);
      }
    }
  }

  bool equal = true;
  GeneralMatrix m1(m.M(), m.N());
  mul(l, u, m1);
  for(int i = 0; i<m.M(); i++)
    for(int j = 0; j< m.N(); j++)
    {
      if(std::abs(m1(i, j) - m(i, j) ) / m(i, j) > 0.1)
      {
        equal = false;
      }
    }
  if(!check_Matrix(m1,m))
  {
    std::cout<<"wrong answer!"<<std::endl;
  }
  else
  {
    std::cout<<"right answer, wahahahaha!"<<std::endl;
  }
}

GeneralMatrix  	standard(Matrix & m)
{
  GeneralMatrix gm1(m);

  GeneralMatrix gm_res(gm1.M(), gm1.N());
  LUDecompose(gm1, gm_res);
  return gm1;
}

void sequential(Matrix & m)
{
  Matrix seq_m(m);

  int blocks = m.M()/Matrix::block_size;
  if(m.M()%Matrix::block_size != 0)
    blocks ++;

  GeneralMatrix linv(Matrix::block_size, Matrix::block_size);
  GeneralMatrix uinv(Matrix::block_size, Matrix::block_size);

  GeneralMatrix lmul(Matrix::block_size, Matrix::block_size);
  GeneralMatrix umul(Matrix::block_size, Matrix::block_size);

  GeneralMatrix rmul(Matrix::block_size, Matrix::block_size);

  for(int k = 0; k <blocks; k++)
  {
    auto lut = get_block(seq_m, k, k);
    LUDecompose(lut, lut);
    invL(lut, linv);
    invU(lut, uinv);

    for(int i = k+1; i < blocks; i++)
    {
      auto ltom = get_block(seq_m, k, i);
      mul(linv, ltom, lmul);
      set_block(seq_m,k, i, lmul);
      auto utom = get_block(seq_m, i, k);
      mul(utom, uinv, umul);
      set_block(seq_m, i, k, umul);
    }

    for(int i = k+1; i < blocks; i++)
      for(int j = k+1; j < blocks; j++)
      {

        auto tm = get_block(seq_m, i, k);
        auto tn = get_block(seq_m, k, j);

        mul(tm, tn, rmul);
        auto tt = get_block(seq_m, i, j);
        sub(tt, rmul, tt);
      }
  }
  //     check_LU_res(m,seq_m);
}

extern void parallel_lu(Matrix & m);

int main(int argc, char *argv[])
{
int thrd_num = 8;
ff::rt::set_hardware_concurrency(thrd_num);//Set concurrency   
 _DEBUG(ff::fflog<>::init(ff::INFO, "log.txt")) 


  Matrix m(MSIZE, MSIZE);
  //init m here!
  string matrix_file_name = "matrix.txt";
  string out_file_name = "lu_matrix.txt";
  //     string time_file_name = "para_time.txt";
  fstream matrix_file;
  ofstream out_file,out_time_file;
  if(matrix_file_name.empty()) {
    cout << "File name is empty!" << endl;
    return -1;
  }
  else {
    matrix_file.open(matrix_file_name.c_str(),ios::in);
    // 	std::cout<<"trying open file: "<< matrix_file_name<<std::endl;
  }
  if(!matrix_file.is_open()) {
    std::cout<<"failed to open file: "<< matrix_file_name<<"! initing matrix now..."<<std::endl;
    // init matrix & write to file
    initMatrixForLU(m);
    std::cout<<"init matrix for LU... done!"<<std::endl;
    matrix_file.open(matrix_file_name.c_str(),ios::out);
    for(int i=0; i<m.M(); i++) {
      for(int j=0; j<m.N(); j++) {
        matrix_file << m(i,j) << '\t';
        //    cout << m(i,j) << '\t';
      }
      matrix_file << endl;
      //    cout << endl;
    }
    std::cout<<"init matrix success!"<<std::endl;
  }
  else {
    //       std::cout<<"reading matrix..."<<std::endl;
    // read file
    for(int i=0; !matrix_file.eof()&& i<m.M(); i++) {
      for(int j=0; !matrix_file.eof() && j<m.N(); j++) {
        matrix_file >> m(i,j);
      }
    }
  }
  matrix_file.close();

  //     std::cout<<"matrix initialized!"<<std::endl;

  parallel_lu(m);


  return 0;
}
