#include "matrix.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tbb/parallel_invoke.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for_each.h>

using namespace std;
using namespace tbb;

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
    Matrix seq_m(m);//output seq_m

    int blocks = m.M()/Matrix::block_size;
    if(m.M()%Matrix::block_size != 0)
        blocks ++;



    concurrent_vector<int > diagonals_vec;
    for(int k= 0; k< blocks; k++) {
        diagonals_vec.push_back(k);
    }

    tbb::parallel_for_each(diagonals_vec.begin(), diagonals_vec.end(),[&seq_m,blocks](int k) {
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
        for(int i=k; i< blocks; i++) {
            index_vec.push_back(i);
        }
        tbb::parallel_for_each(index_vec.begin(),index_vec.end(),[&seq_m,&linv,&uinv,k](int i) {
            tbb::parallel_invoke(
            [&seq_m, &linv, k, i]() {
                GeneralMatrix lmul(Matrix::block_size, Matrix::block_size);
                auto ltom = get_block(seq_m, k, i);
                mul(linv, ltom, lmul);
                set_block(seq_m,k, i, lmul);
            },
            [&seq_m, &uinv, i, k]() {
                GeneralMatrix umul(Matrix::block_size, Matrix::block_size);
                auto utom = get_block(seq_m, i, k);
                mul(utom, uinv, umul);
                set_block(seq_m, i, k, umul);
            });
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
    });
    diagonals_vec.clear();



    /*
        ff::para<> diagonals[blocks];
        for(int k = 0; k <blocks; k++)
        {
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
        }*/
}
int main(int argc, char *argv[])
{
    bool bIsPara = false;//false;

    if(argc > 1) {
        stringstream ss_argv;
        int n;// n > 0 means parallel, otherwise serial.
        ss_argv << argv[1];
        ss_argv >> n;
        bIsPara = (n > 0)?true:false;
    }

    Matrix m(MSIZE, MSIZE);
    //init m here!
    string matrix_file_name = "../ff/matrix.txt";
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
    }
    if(!matrix_file.is_open()) {
        // init matrix & write to file
        initMatrixForLU(m);
        matrix_file.open(matrix_file_name.c_str(),ios::out);
        for(int i=0; i<m.M(); i++) {
            for(int j=0; j<m.N(); j++) {
                matrix_file << m(i,j) << '\t';
                //    cout << m(i,j) << '\t';
            }
            matrix_file << endl;
            //    cout << endl;
        }
    }
    else {
        // read file
        for(int i=0; !matrix_file.eof()&& i<m.M(); i++) {
            for(int j=0; !matrix_file.eof() && j<m.N(); j++) {
                matrix_file >> m(i,j);
            }
        }
    }
    matrix_file.close();



    chrono::time_point<chrono::system_clock> start, end;
    int elapsed_seconds;

    if(bIsPara) {
        //!Start test parallel version
        start = chrono::system_clock::now();
        parallel(m);
        end = chrono::system_clock::now();
        elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                          (end-start).count();
        cout << "ff elapsed time: " << elapsed_seconds << "us" << endl;
    }

    else {
        //!Start test sequential version
        start = chrono::system_clock::now();
        sequential(m);
        end = chrono::system_clock::now();
        elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                          (end-start).count();
        cout << "sequential elapsed time: " << elapsed_seconds << "us" << endl;
    }

    if(bIsPara) {
        // write para time file
        out_time_file.open("para_time.txt",ios::app);
        if(!out_time_file.is_open()) {
            cout << "Can't open the file para_time.txt" << endl;
            return -1;
        }
        out_time_file << elapsed_seconds << endl;
        out_time_file.close();
    }
    else {
        // write seq time file
        out_time_file.open("time.txt");
        if(!out_time_file.is_open()) {
            cout << "Can't open the file time.txt" << endl;
            return -1;
        }
        out_time_file << elapsed_seconds << endl;
        out_time_file.close();
    }

    out_file.open(out_file_name.c_str());
    if(!out_file.is_open()) {
        cout << "Can't open the file " << out_file_name << endl;
        return -1;
    }
    /*print the LU matrix -- wrong!*/
//     for(int i=0; i<m.M(); i++) {
//         for(int j=0; j<m.N(); j++) {
//             out_file << m(i,j) << '\t';
//         }
//         out_file << endl;
//     }

    return 0;
}
