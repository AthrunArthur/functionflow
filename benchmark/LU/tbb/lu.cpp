#include "../matrix.h"
#include <tbb/parallel_invoke.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for_each.h>
#include <tbb/task_scheduler_init.h>
#include <tuple>
using namespace std;
using namespace tbb;

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
        tbb::parallel_invoke(
        [&lut, &linv]() {
            invL(lut, linv);
        },
        [&lut, &uinv]() {
            invU(lut, uinv);
        });
        concurrent_vector<int > index_vec;
        for(int i=k+1; i< blocks; i++) {
            index_vec.push_back(i);
        }
        tbb::parallel_for_each(index_vec.begin(),index_vec.end(),[&seq_m,&linv,&uinv,k](int i) {
            //tbb::parallel_invoke(
            //[&seq_m, &linv, k, i]() {
                GeneralMatrix lmul(Matrix::block_size, Matrix::block_size);
                auto ltom = get_block(seq_m, k, i);
                mul(linv, ltom, lmul);
                set_block(seq_m,k, i, lmul);
            //},
            //[&seq_m, &uinv, i, k]() {
                GeneralMatrix umul(Matrix::block_size, Matrix::block_size);
                auto utom = get_block(seq_m, i, k);
                mul(utom, uinv, umul);
                set_block(seq_m, i, k, umul);
            //});
        });
        index_vec.clear();
        concurrent_vector<tuple< int, int > > pos_vec;
        for(int i=k+1; i<blocks; i++) {
            for(int j=k+1; j<blocks; j++) {
                pos_vec.push_back(make_tuple(i,j));
            }
        }
        tbb::parallel_for_each(pos_vec.begin(),pos_vec.end(),[&seq_m,k](tuple< int, int > pos) {
            int i=get<0>(pos),j=get<1>(pos);
            GeneralMatrix rmul(Matrix::block_size, Matrix::block_size);
            auto tm = get_block(seq_m, i, k);
            auto tn = get_block(seq_m, k, j);

            mul(tm, tn, rmul);
            auto tt = get_block(seq_m, i, j);
            sub(tt, rmul, tt);
        });
        pos_vec.clear();
    }
//     check_LU_res(m,seq_m);
}
