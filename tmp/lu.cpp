#include "ff.h"
#include "matrix.h"
using namespace ff;
using namespace std;

void parallel_lu(Matrix & m)
{
    Matrix seq_m(m);

    int blocks = m.M()/Matrix::block_size;
    if(m.M()%Matrix::block_size != 0)
        blocks ++;

    GeneralMatrix linv(Matrix::block_size, Matrix::block_size);
    GeneralMatrix uinv(Matrix::block_size, Matrix::block_size);


    for(int k = 0; k <blocks; k++)
    {
        auto lut = get_block(seq_m, k, k);
        LUDecompose(lut, lut);
	if(k == blocks -1 )
	  continue;
        ff::para<void> il, iu;
        il([&lut, &linv, k]() {
            invL(lut, linv);
        });
        iu([&lut, &uinv, k]() {
            invU(lut, uinv);
        });
        //std::cout<<"1"<<std::endl;

        ff_wait(il && iu);

        ff::paragroup ir;
	ir.for_each(k+1 ,blocks,[&seq_m,&linv,&uinv,k](int i) {
            //ff::para<> p1, p2;
            //p1([&seq_m, &linv, k, i]() {
                GeneralMatrix lmul(Matrix::block_size, Matrix::block_size);
                auto ltom = get_block(seq_m, k, i);
                mul(linv, ltom, lmul);
                set_block(seq_m,k, i, lmul);
            //});
            //p2([&seq_m, &uinv, i, k]() {
                GeneralMatrix umul(Matrix::block_size, Matrix::block_size);
                auto utom = get_block(seq_m, i, k);
                mul(utom, uinv, umul);
                set_block(seq_m, i, k, umul);
            //});
            //ff_wait(p1&&p2);
        });
        //std::cout<<"2"<<std::endl;
        ff::ff_wait(all(ir));
        vector<tuple< int, int > > pos_vec;
        for(int i=k+1; i<blocks; i++) {
            for(int j=k+1; j<blocks; j++) {
                pos_vec.push_back(make_tuple(i,j));
            }
        }
        ff::paragroup im;
        im.for_each(pos_vec.begin(),pos_vec.end(),[&seq_m,k](tuple< int, int > pos) {

            int i=get<0>(pos),j=get<1>(pos);
            GeneralMatrix rmul(Matrix::block_size, Matrix::block_size);
            auto tm = get_block(seq_m, i, k);
            auto tn = get_block(seq_m, k, j);

            mul(tm, tn, rmul);
            auto tt = get_block(seq_m, i, j);
            sub(tt, rmul, tt);

        });
        //std::cout<<"3"<<std::endl;
        ff::ff_wait(all(im));
        //std::cout<<"3-end"<<std::endl;
    }
//     check_LU_res(m,seq_m);
}
