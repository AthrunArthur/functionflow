#include "matrix.h"
#include <chrono>
#include <iostream>
#include "ff.h"
#include "common/log.h"


using namespace std;
using namespace ff;

typedef matrix_impl<double, block, 32> Matrix;
typedef matrix_impl<double, row_major> GeneralMatrix;

#define MSIZE 2048


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
}

void parallel(Matrix & m)
{
    Matrix seq_m(m);

    int blocks = m.M()/Matrix::block_size;
    if(m.M()%Matrix::block_size != 0)
        blocks ++;

    GeneralMatrix linv(Matrix::block_size, Matrix::block_size);
    GeneralMatrix uinv(Matrix::block_size, Matrix::block_size);




    ff::para<> diagonals[blocks];
    for(int k = 0; k <blocks; k++)
    {
		//std::cout<<"dia k:"<<k<<std::endl;
        auto lut = get_block(seq_m, k, k);
        diagonals[k]([&lut]() {
            LUDecompose(lut, lut);
        });
        ff::para<void> il, iu;
        il[diagonals[k]]([&lut, &linv, k]() {
            invL(lut, linv);
        });
        iu[diagonals[k]]([&lut, &uinv, k]() {
            invU(lut, uinv);
        });

		if(k == blocks -1)
			ff_wait(il && iu);
        ff::paragroup ir, ic;
        for(int i = k+1; i < blocks; i++)
        {
            ff::para<> p1, p2;
            p1[il]([&seq_m, &linv, k, i]() {
                GeneralMatrix lmul(Matrix::block_size, Matrix::block_size);
                auto ltom = get_block(seq_m, k, i);
                mul(linv, ltom, lmul);
                set_block(seq_m,k, i, lmul);
            });

            p2[iu]([&seq_m, &uinv, i, k]() {
                GeneralMatrix umul(Matrix::block_size, Matrix::block_size);
                auto utom = get_block(seq_m, i, k);
                mul(utom, uinv, umul);
                set_block(seq_m, i, k, umul);
            });
            ir.add(p1);
            ic.add(p2);
        }

        ff::ff_wait(all(ir) && all(ic));
		
        ff::paragroup im;
        for(int i = k+1; i < blocks; i++)
            for(int j = k+1; j < blocks; j++)
            {
                ff::para<> p1;
                /*[ir[i-k] && ic[j-k]]*/ 
				p1([&seq_m, i, j, k]() {
                    GeneralMatrix rmul(Matrix::block_size, Matrix::block_size);
                    auto tm = get_block(seq_m, i, k);
                    auto tn = get_block(seq_m, k, j);

                    mul(tm, tn, rmul);
                    auto tt = get_block(seq_m, i, j);
                    sub(tt, rmul, tt);
                });
                im.add(p1);
            }
        ff::ff_wait(all(im));
    }
}
int main(int argc, char *argv[])
{
    Matrix m(MSIZE, MSIZE);
    //init m here!

    chrono::time_point<chrono::system_clock> start, end;
int elapsed_seconds;
    //!Start test standard version
    /*
	start = chrono::system_clock::now();
    standard(m);
    end = chrono::system_clock::now();
    elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                          (end-start).count();
    cout << "standard elapsed time: " << elapsed_seconds << "us" << endl;
	*/
    //!End test standard version

    //!Start test sequential version
    start = chrono::system_clock::now();
    sequential(m);
    end = chrono::system_clock::now();
    elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                      (end-start).count();
    cout << "sequential elapsed time: " << elapsed_seconds << "us" << endl;

    //warm up ff runtime
	_DEBUG(ff::fflog<>::init(ff::TRACE, "log.txt"))
	_DEBUG(LOG_INFO(main)<<"main start, id:"<<ff::rt::get_thrd_id());
	
    ff::para<int> a;
    int num = 10;
    a([&num]() {
        return num;
    }).then([](int x) {});
    ff::para<> b;
    b[a]([&num, &a]() {
        num + a.get();
    }).then([]() {});

    //!Start test parallel version
    start = chrono::system_clock::now();
    parallel(m);
    end = chrono::system_clock::now();
    elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                      (end-start).count();
    cout << "ff elapsed time: " << elapsed_seconds << "us" << endl;
_DEBUG(LOG_INFO(main)<<"main exit, id:"<<ff::rt::get_thrd_id());
    return 0;
}