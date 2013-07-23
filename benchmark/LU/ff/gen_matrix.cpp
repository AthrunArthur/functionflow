#include <fstream>
#include <iostream>
#include <random>
#include <chrono>
#include <sstream>
#include "ff.h"
#define RANDOM_MAX 5//Set this value not too big in case of overflow (the right-down corner would get the wrong result)
#define RANDOM_MIN 2
#define DEFAULT_LEN 1000
#define PARA_DEGREE 10
using namespace std;

int ** malloc_matrix(int row, int column)
{
    int ** M = (int **)malloc(row * sizeof(int *));
    int i;
    if(!M) {
        cout << "Error when malloc." << endl;
        return NULL;
    }
    for(i = 0; i < row; i++) {
        M[i] = (int *)malloc(column * sizeof(int));
        if(!M[i]) {
            cout << "Error when malloc." << endl;
            break;
        }
    }
    if(i!=row) {
        for(int j=0; j<i; j++)
            free(M[j]);
        free(M);
        return NULL;
    }
    return M;
}

void free_matrix(int ** M,int row)
{
    if(M) {
        for(int i=0; i<row; i++) {
            if(M[i])
                free(M[i]);
        }
        free(M);
    }
}

bool copy_matrix(int ** src_M, int **dest_M, int src_start_x, int src_start_y,
                 int dest_start_x, int dest_start_y, int row, int column)
{
    if(!src_M || !dest_M)
        return false;
    for(int i = 0; i < row; i++)
        for(int j = 0; j < column; j++)
            dest_M[dest_start_x+i][dest_start_y+j] = src_M[src_start_x+i][src_start_y+j];
    return true;
}


int ** get_L_matrix(int n,int max)
{
    int ** L_matrix = malloc_matrix(n,n);
    random_device rd;
    mt19937 gen(rd());
    // min() is 1, and max() is 6
    uniform_int_distribution<> dis(RANDOM_MIN, max);

    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++) {
            if(i==j)
                L_matrix[i][j] = 1;
            else if(i > j)
                L_matrix[i][j] = dis(gen);
            else
                L_matrix[i][j] = 0;
        }
    return L_matrix;
}

int ** get_U_matrix(int n,int max)
{
    int ** U_matrix = malloc_matrix(n,n);
    random_device rd;
    mt19937 gen(rd());
    // min() is 1, and max() is 6
    uniform_int_distribution<> dis(RANDOM_MIN, max);

    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++) {
            if(i <= j)
                U_matrix[i][j] = dis(gen);
            else
                U_matrix[i][j] = 0;
        }
    return U_matrix;
}

int ** matrix_multiple(int ** A, int ** B, int A_row, int AB_col_row, int B_col)
// The matrix need to call this function is very small
{
    int ** C = NULL;
    if(!A || !B)
        return NULL;
    if(!(C = malloc_matrix(A_row,B_col)))
        return NULL;
    for(int i=0; i<A_row; i++)
        for(int j=0; j<B_col; j++) {
            C[i][j] = 0;
            for(int k=0; k<AB_col_row; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
    return C;
}

int ** para_matrix_multiple(int ** A, int ** B, int A_row, int AB_col_row, int B_col, int para_len)
{
    if(!A || !B)
        return NULL;
    if(A_row <= para_len && B_col <= para_len) {
        return matrix_multiple(A,B,A_row,AB_col_row,B_col);
    }
    else {
        int ** A_up = NULL,** A_down = NULL,** B_left = NULL,** B_right = NULL;
        int ** C = NULL;
        int A_row_up,A_row_down,B_col_left,B_col_right;
        bool A_is_divided = false, B_is_divided = false;
        if(!(C = malloc_matrix(A_row,B_col)))
            return NULL;
        if(A_row > para_len) {
            A_row_up = A_row / 2;
            A_row_down = A_row - A_row_up;
            A_up = A;
            A_down = A + A_row_up;// The pointer would move an entire column(not A + A_row_up * AB_col_row)
            A_is_divided = true;
        }
        if(B_col > para_len) {
            B_col_left = B_col / 2;
            B_col_right = B_col - B_col_left;
            if(!(B_left = malloc_matrix(AB_col_row,B_col_left)) ||
                    !(B_right = malloc_matrix(AB_col_row,B_col_right)) ||
                    !(copy_matrix(B,B_left,0,0,0,0,AB_col_row,B_col_left)) ||
                    !(copy_matrix(B,B_right,0,B_col_left,0,0,AB_col_row,B_col_right))) {
                free_matrix(C,A_row);
                free_matrix(B_left,AB_col_row);
                free_matrix(B_right,AB_col_row);
                return NULL;
            }
            B_is_divided = true;
        }
        if(A_is_divided && B_is_divided) {
            ff::para<int **> a,b,c,d;
            a([&A_up,&B_left,&A_row_up,&AB_col_row,&B_col_left,&para_len]() {
                return para_matrix_multiple(A_up,B_left,A_row_up,AB_col_row,B_col_left,para_len);
            });
            b([&A_down,&B_left,&A_row_down,&AB_col_row,&B_col_left,&para_len]() {
                return para_matrix_multiple(A_down,B_left,A_row_down,AB_col_row,B_col_left,para_len);
            });
            c([&A_up,&B_right,&A_row_up,&AB_col_row,&B_col_right,&para_len]() {
                return para_matrix_multiple(A_up,B_right,A_row_up,AB_col_row,B_col_right,para_len);
            });
            d([&A_down,&B_right,&A_row_down,&AB_col_row,&B_col_right,&para_len]() {
                return para_matrix_multiple(A_down,B_right,A_row_down,AB_col_row,B_col_right,para_len);
            });
            (a && b).then([&C,&A_row_up,&A_row_down,&B_col_left,&A_row](int ** C_ul,int ** C_dl) {
                if(!C_ul || !C_dl) {
                    free_matrix(C,A_row);
                    C = NULL;
                }
                copy_matrix(C_ul,C,0,0,0,0,A_row_up,B_col_left);// Need to judge
                copy_matrix(C_dl,C,0,0,A_row_up,0,A_row_down,B_col_left);// Need to judge
                free_matrix(C_ul,A_row_up);
                free_matrix(C_dl,A_row_down);
            });
            (c && d).then([&C,&A_row_up,&A_row_down,&B_col_left,&B_col_right,&A_row](int ** C_ur, int ** C_dr) {
                if(!C_ur || !C_dr) {
                    free_matrix(C,A_row);
                    C = NULL;
                }
                copy_matrix(C_ur,C,0,0,0,B_col_left,A_row_up,B_col_right);// Need to judge
                copy_matrix(C_dr,C,0,0,A_row_up,B_col_left,A_row_down,B_col_right);// Need to judge
                free_matrix(C_ur,A_row_up);
                free_matrix(C_dr,A_row_down);
            });
            free_matrix(B_left,AB_col_row);
            free_matrix(B_right,AB_col_row);
        }
        else if(A_is_divided) {
            ff::para<int **> a,b;
            a([&A_up,&B,&A_row_up,&AB_col_row,&B_col,&para_len]() {
                return para_matrix_multiple(A_up,B,A_row_up,AB_col_row,B_col,para_len);
            });
            b([&A_down,&B,&A_row_down,&AB_col_row,&B_col,&para_len]() {
                return para_matrix_multiple(A_down,B,A_row_down,AB_col_row,B_col,para_len);
            });
            (a&&b).then([&C,&A_row_up,&A_row_down,&B_col,&A_row](int ** C_up,int ** C_down) {
                if(!C_up || !C_down) {
                    free_matrix(C,A_row);
                    C = NULL;
                }
                copy_matrix(C_up,C,0,0,0,0,A_row_up,B_col);// Need to judge
                copy_matrix(C_down,C,0,0,A_row_up,0,A_row_down,B_col);// Need to judge
                free_matrix(C_up,A_row_up);
                free_matrix(C_down,A_row_down);
            });

        }
        else { // B_is_divided
            ff::para<int **> a,b;
            a([&A,&B_left,&A_row,&AB_col_row,&B_col_left,&para_len]() {
                return para_matrix_multiple(A,B_left,A_row,AB_col_row,B_col_left,para_len);
            });
            b([&A,&B_right,&A_row,&AB_col_row,&B_col_right,&para_len]() {
                return para_matrix_multiple(A,B_right,A_row,AB_col_row,B_col_right,para_len);
            });
            (a&&b).then([&C,&A_row,&B_col_left,&B_col_right](int ** C_left, int ** C_right) {
                if(!C_left || !C_right) {
                    free_matrix(C,A_row);
                    C = NULL;
                }
                copy_matrix(C_left,C,0,0,0,0,A_row,B_col_left);// Need to judge
                copy_matrix(C_right,C,0,0,0,B_col_left,A_row,B_col_right);// Need to judge
                free_matrix(C_left,A_row);
                free_matrix(C_right,A_row);
            });
            free_matrix(B_left,AB_col_row);
            free_matrix(B_right,AB_col_row);
        }
        return C;
    }

}

int main(int argc, char *argv[])
{
    string out_file_name = "../../benchmark/LU/ff/matrix.txt";
    string outLU_file_name = "../../benchmark/LU/ff/matrixLU.txt";
    int ** L_matrix=NULL,**U_matrix=NULL,**LU_matrix=NULL,n = DEFAULT_LEN,max = RANDOM_MAX;// max no more than 1000 recommended

    ofstream out_file,outLU_file;
    if(argc > 1) {
        stringstream ss_argv;
        ss_argv << argv[1];
        ss_argv >> n;
    }
    if(argc == 3){
	stringstream ss_argv;
        ss_argv << argv[2];
        ss_argv >> max;
    }
    if(out_file_name.empty() || outLU_file_name.empty()) {
        cout << "File name is empty!" << endl;
        return -1;
    }
    else {
        out_file.open(out_file_name.c_str());
	outLU_file.open(outLU_file_name.c_str());
    }
    if(!out_file.is_open()) {
        cout << "Can't open the file " << out_file_name << endl;
        return -1;
    }
    if(!outLU_file.is_open()) {
        cout << "Can't open the file " << outLU_file_name << endl;
        return -1;
    }
    
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    L_matrix = get_L_matrix(n,max);
    U_matrix = get_U_matrix(n,max);
    outLU_file << "L+U matrix:" << n << endl;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++){
	  if(i > j)
	    outLU_file << L_matrix[i][j] << '\t';
	  else
            outLU_file << U_matrix[i][j] << '\t';
	}
        outLU_file << endl;
    }
    LU_matrix = para_matrix_multiple(L_matrix,U_matrix,n,n,n,n/PARA_DEGREE);
//     cout << "LU matrix:" << endl;
    out_file << n << endl;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            out_file << LU_matrix[i][j] << '\t';
        }
        out_file << endl;
    }
    
    end = chrono::system_clock::now();
    int elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                          (end-start).count();

    cout << "Elapsed time: " << elapsed_seconds << "us" << endl;
    out_file.close();
    outLU_file.close();
    free_matrix(LU_matrix,n);
    free_matrix(L_matrix,n);
    free_matrix(U_matrix,n);
    return 0;

}
