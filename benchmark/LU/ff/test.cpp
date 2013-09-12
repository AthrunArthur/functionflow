#include "matrix.h"

#include <iostream>
#include <type_traits>

typedef matrix_impl<double, block, 4> Matrix;
typedef matrix_impl<double, row_major> GeneralMatrix;

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
            if(std::abs(m1(i, j) - m(i, j) )>0.1)
            {
                equal = false;
            }
        }
    if(!equal)
    {
        std::cout<<"wrong answer!"<<std::endl;
        std::cout<<"original matrix:";
        print(m);
        std::cout<<"\nres matrix:";
        print(m1);
        std::cout<<std::endl;
    }
    else
    {
        std::cout<<"right answer, wahahahaha!"<<std::endl;
    }
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
    check_LU_res(m, seq_m);

}


int main(int argc, char *argv[])
{

    GeneralMatrix m(16, 16);
    initMatrixForLU<GeneralMatrix>(m);

    GeneralMatrix res(m.M(), m.N());
    LUDecompose(m, res);
    check_LU_res(m, res);

    Matrix m1(m);

    sequential(m1);


    return 0;
}