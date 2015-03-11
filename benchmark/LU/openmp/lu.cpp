#include <omp.h>
#include "../matrix.h"
using namespace std;

void parallel_lu(Matrix & m)
{
    Matrix seq_m(m);//output seq_m

    int blocks = m.M()/Matrix::block_size;
    if(m.M()%Matrix::block_size != 0)
        blocks ++;

    for(int k= 0; k< blocks; k++) {
        auto lut = get_block(seq_m, k, k);
        LUDecompose(lut,lut);
        GeneralMatrix linv(Matrix::block_size, Matrix::block_size);
        GeneralMatrix uinv(Matrix::block_size, Matrix::block_size);
        #pragma omp parallel
        {
            #pragma omp task
            {
                invL(lut, linv);
            }
            #pragma omp task
            {
                invU(lut, uinv);
            }
            #pragma omp taskwait
        }
        int i;
        #pragma omp parallel for
        for(i=k+1; i< blocks; i++) {
            #pragma omp task
            {
                GeneralMatrix lmul(Matrix::block_size, Matrix::block_size);
                auto ltom = get_block(seq_m, k, i);
                mul(linv, ltom, lmul);
                set_block(seq_m,k, i, lmul);
            }
            #pragma omp task
            {
                GeneralMatrix umul(Matrix::block_size, Matrix::block_size);
                auto utom = get_block(seq_m, i, k);
                mul(utom, uinv, umul);
                set_block(seq_m, i, k, umul);
            }
            #pragma omp taskwait
        }

        int j;
        for(i=k+1; i<blocks; i++) {
            #pragma omp parallel for
            for(j=k+1; j<blocks; j++) {
                GeneralMatrix rmul(Matrix::block_size, Matrix::block_size);
                auto tm = get_block(seq_m, i, k);
                auto tn = get_block(seq_m, k, j);

                mul(tm, tn, rmul);
                auto tt = get_block(seq_m, i, j);
                sub(tt, rmul, tt);
            }
        }
    }
    //check_LU_res(m,seq_m);
}
