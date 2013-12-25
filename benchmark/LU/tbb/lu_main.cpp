#include "matrix.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tbb/parallel_invoke.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for_each.h>
#include <tbb/task_scheduler_init.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace tbb;

typedef matrix_impl<double, block, 32> Matrix;
typedef matrix_impl<double, row_major> GeneralMatrix;

#define MSIZE 2048

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

void parallel(Matrix & m)
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
int main(int argc, char *argv[])
{
    task_scheduler_init init(8);
    bool bIsPara = false;//false;  
    boost::property_tree::ptree pt;
    pt.put("time-unit", "us");
    
    if(argc > 1) {
        stringstream ss_argv;
        int n;// n > 0 means parallel, otherwise serial.
        ss_argv << argv[1];
        ss_argv >> n;
        bIsPara = (n > 0)?true:false;
    }

    Matrix m(MSIZE, MSIZE);
    //init m here!
//     string matrix_file_name = "../ff/matrix.txt";
    string matrix_file_name = "../LU/ff/matrix.txt";
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
	pt.put("para-elapsed-time", elapsed_seconds);
        cout << "tbb elapsed time: " << elapsed_seconds << "us" << endl;
    }
    else {
        //!Start test sequential version
        start = chrono::system_clock::now();
        sequential(m);
        end = chrono::system_clock::now();
        elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                          (end-start).count();
	pt.put("sequential-elapsed-time", elapsed_seconds);
        cout << "sequential elapsed time: " << elapsed_seconds << "us" << endl;
    }
    boost::property_tree::write_json("time.json", pt);

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
    
    return 0;
}
