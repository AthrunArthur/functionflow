#include "utils.h"
#include "matrix.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;


#define MSIZE 1024
//#define MSIZE 4

extern void para_mul(const Matrix & m1, const Matrix & m2, Matrix &res);

void serial_mul(const Matrix & m1, const Matrix & m2, Matrix & res)
{
  int blocks = m1.M()/Matrix::block_size;
  if(m1.M()%Matrix::block_size != 0)
    blocks ++;
  GeneralMatrix tmp(Matrix::block_size, Matrix::block_size);
  for(int k = 0; k < blocks; k++)
  {
    for(int i = 0; i < blocks; i++)
    {
      auto sum = get_block(res, k, i);
      for(int j=0; j<blocks; j++)
      {
        auto tm1 = get_block(m1, k, j);
        auto tm2 = get_block(m2, j, i);
        mul(tm1, tm2, tmp);
        add(sum, tmp, sum);
      }
    }
  }
}

void init_matrix(Matrix & m)
{
  for(int i = 0; i < m.M(); i++)
    for(int j = 0; j < m.N(); j++)
    {
        m(i, j) = 1.0*(rand()%(m.M()*m.N()));
    }
}
void read_matrix_from_fp(Matrix & m, const std::string & fp)
{
 fstream fs;
 fs.open(fp);

    for(int i=0; !fs.eof()&& i<m.M(); i++) {
      for(int j=0; !fs.eof() && j<m.N(); j++) {
        fs >> m(i,j);
      }
    }
}

int main(int argc, char *argv[])
{
  RRecord rr("time.json", "quad");
  ParamParser pp;
  pp.parse(argc, argv);
  int thrd_num = pp.get<int>("thrd-num");
  rr.put("thrd-num", thrd_num);
  bool bIsPara = (thrd_num > 0);
  if(thrd_num > 0)
    initial_para_env(thrd_num);

  Matrix m1(MSIZE, MSIZE), m2(MSIZE, MSIZE);
  Matrix res(MSIZE, MSIZE);
  res.reset();
  string matrix_file_name = "matrix.txt";
  string out_file_name = "res_matrix.txt";
  fstream matrix_file;
  ofstream out_file;
  matrix_file.open(matrix_file_name);
  if(!matrix_file.is_open()) {
    std::cout<<"failed to open file: "<< matrix_file_name<<"! initing matrix now..."<<std::endl;
    matrix_file.open(matrix_file_name, ios::out);
    Matrix tm(MSIZE, MSIZE);
    init_matrix(tm);
    for(int i = 0; i < tm.M(); ++i){
      for(int j = 0; j < tm.N(); ++j)
      {
        matrix_file << tm(i,j) << '\t';
      }
      matrix_file<<endl;
    }
    matrix_file.close();
  }
  read_matrix_from_fp(m1, matrix_file_name);
  read_matrix_from_fp(m2, matrix_file_name);
  if(!bIsPara)
  {
    rr.run("elapsed-time", serial_mul, m1, m2, res);
    //serial_mul(m1, m2, res);
    //mul(m1, m2, res);
  }
  else
  {
    rr.run("elapsed-time", para_mul, m1, m2, res);
  }
  out_file.open(out_file_name);
  for(int i = 0; i < res.M(); ++i){
    for(int j = 0; j < res.N(); ++j)
    {
      out_file<<res(i, j) <<"\t";
    }
    out_file<<endl;
  }
  out_file.close();
  return 0;
}
