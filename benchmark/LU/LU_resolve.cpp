#include <fstream>
#include <iostream>
#include <chrono>
#include <vector>
#include "ff.h"
#define MAX_SERIAL_SIZE 4// The max column/row size to be calculated serially.
using namespace ff;
using namespace std;


float ** malloc_matrix(int row, int column)
{
    float ** M = (float **)malloc(row * sizeof(float *));
    int i;
    if(!M) {
        cout << "Error when malloc." << endl;
        return NULL;
    }
    for(i = 0; i < row; i++) {
        M[i] = (float *)malloc(column * sizeof(float));
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

void free_matrix(float ** M,int row)
{
    if(M) {
        for(int i=0; i<row; i++) {
            if(M[i])
                free(M[i]);
        }
        free(M);
    }
}
bool copy_matrix(float ** src_M, float **dest_M, int src_start_x, int src_start_y,
                 int dest_start_x, int dest_start_y, int row, int column)
{
    if(!src_M || !dest_M)
        return false;
    for(int i = 0; i < row; i++)
        for(int j = 0; j < column; j++)
            dest_M[dest_start_x+i][dest_start_y+j] = src_M[src_start_x+i][src_start_y+j];
    return true;
}

bool LU_Doolittle(float ** A, int n)
// n is the size of the matrix
// The resolved matrix is stored in the original matrix A.
{
    if(!A)
        return false;
    for(int k=0; k<n; k++) {
        for(int j=k; j<n; j++) {
            for(int p=0; p<k; p++)A[k][j]-=A[k][p]*A[p][j];
        }
        if(A[k][k] == 0.0) {
            return false;
        }
        for(int i=k+1; i<n; i++) {
            float sum=0.;
            for(int p=0; p<k; p++)A[i][k]-=A[i][p]*A[p][k];
            A[i][k]/=A[k][k];
        }
    }
    return true;
}


float ** triangle_matrix_inversion(float ** A,int n, bool isUpperM = true)
// The matrix need to call this function is very small,
// so the multiple don't need to be parallelized.
{
    float ** B = NULL;
    if(!A)
        return NULL;
    if(!(B = malloc_matrix(n,n)))
        return NULL;
    for(int i=0; i<n; i++) // Init matrix B to unit matrix.
        for(int j=0; j<n; j++)
            B[i][j]=(i==j)?1:0;
    if(isUpperM) { //Upper triangular matrix U
        for(int i=0; i<n; i++) { // Set the diagonal value of A to 1.
            if(A[i][i]==0) {
                cout << "Error:Singular matrix!" << endl;
                for(int m=0; m<n; m++) {
                    for(int p=0; p<n; p++)
                        cout << A[m][p] << ' ';
                    cout << endl;
                }
                free_matrix(B,n);
                return NULL;
            }
            B[i][i]/=A[i][i];
            for(int j=i+1; j<n; j++) {
                A[i][j]/=A[i][i];//Row Transformation
            }
            A[i][i]=1;
        }
        for(int l=n-1; l>0; l--) // Use the elementary transformation to inverse upper triangular matrix.
        {
            for(int i=l-1; i>=0; i--)// Translate the A matrix to a unit matrix.
            {
                for(int j=0; j<n; j++)
                    B[i][j]=B[i][j]-A[i][l]*B[l][j];//Row Transformation
                A[i][l]=0;
            }
        }
    }
    else { //Lower triangular matrix L
        for(int l=n-1; l>0; l--) // Use the elementary transformation to inverse lower triangular matrix.
        {
            for(int i=l-1; i>=0; i--)// Translate the A matrix to a unit matrix.
            {
                for(int j=0; j<n; j++)
                    B[j][i]=B[j][i]-A[l][i]*B[j][l];//Column Transformation
                A[l][i]=0;
            }
        }
    }
    return B;
}

float ** matrix_multiple(float ** A, float ** B, int A_row, int AB_col_row, int B_col)
// The matrix need to call this function is very small
{
    float ** C = NULL;
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

float ** para_matrix_multiple(float ** A, float ** B, int A_row, int AB_col_row, int B_col)
{
    if(!A || !B)
        return NULL;
    if(A_row <= MAX_SERIAL_SIZE && B_col <= MAX_SERIAL_SIZE) {
        return matrix_multiple(A,B,A_row,AB_col_row,B_col);
    }
    else {
        float ** A_up = NULL,** A_down = NULL,** B_left = NULL,** B_right = NULL;
        float ** C = NULL;
        int A_row_up,A_row_down,B_col_left,B_col_right;
        bool A_is_divided = false, B_is_divided = false;
        if(!(C = malloc_matrix(A_row,B_col)))
            return NULL;
        if(A_row > MAX_SERIAL_SIZE) {
            A_row_up = A_row / 2;
            A_row_down = A_row - A_row_up;
            A_up = A;
            A_down = A + AB_col_row * A_row_up;
            A_is_divided = true;
        }
        if(B_col > MAX_SERIAL_SIZE) {
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
            ff::para<float **> a,b,c,d;
            a([&A_up,&B_left,&A_row_up,&AB_col_row,&B_col_left]() {
                return para_matrix_multiple(A_up,B_left,A_row_up,AB_col_row,B_col_left);
            });
            b([&A_down,&B_left,&A_row_down,&AB_col_row,&B_col_left]() {
                return para_matrix_multiple(A_down,B_left,A_row_down,AB_col_row,B_col_left);
            });
            c([&A_up,&B_right,&A_row_up,&AB_col_row,&B_col_right]() {
                return para_matrix_multiple(A_up,B_right,A_row_up,AB_col_row,B_col_right);
            });
            d([&A_down,&B_right,&A_row_down,&AB_col_row,&B_col_right]() {
                return para_matrix_multiple(A_down,B_right,A_row_down,AB_col_row,B_col_right);
            });
            (a && b).then([&C,&A_row_up,&A_row_down,&B_col_left,&A_row](float ** C_ul,float ** C_dl) {
                if(!C_ul || !C_dl) {
                    free_matrix(C,A_row);
                    C = NULL;
                }
                copy_matrix(C_ul,C,0,0,0,0,A_row_up,B_col_left);// Need to judge
                copy_matrix(C_dl,C,0,0,A_row_up,0,A_row_down,B_col_left);// Need to judge
                free_matrix(C_ul,A_row_up);
                free_matrix(C_dl,A_row_down);
            });
            (c && d).then([&C,&A_row_up,&A_row_down,&B_col_left,&B_col_right,&A_row](float ** C_ur, float ** C_dr) {
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
            ff::para<float **> a,b;
            a([&A_up,&B,&A_row_up,&AB_col_row,&B_col]() {
                return para_matrix_multiple(A_up,B,A_row_up,AB_col_row,B_col);
            });
            b([&A_down,&B,&A_row_down,&AB_col_row,&B_col]() {
                return para_matrix_multiple(A_down,B,A_row_down,AB_col_row,B_col);
            });
            (a&&b).then([&C,&A_row_up,&A_row_down,&B_col,&A_row](float ** C_up,float ** C_down) {
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
            ff::para<float **> a,b;
            a([&A,&B_left,&A_row,&AB_col_row,&B_col_left]() {
                return para_matrix_multiple(A,B_left,A_row,AB_col_row,B_col_left);
            });
            b([&A,&B_right,&A_row,&AB_col_row,&B_col_right]() {
                return para_matrix_multiple(A,B_right,A_row,AB_col_row,B_col_right);
            });
            (a&&b).then([&C,&A_row,&B_col_left,&B_col_right](float ** C_left, float ** C_right) {
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

float ** get_U12(float ** A11, float ** A12, int n, int r)
{
    if(!A11 || !A12)
        return NULL;
    float ** L11 = NULL, ** L11_inv = NULL, ** U12 = NULL;
    if(L11 = malloc_matrix(r,r)) {
        for(int i = 0; i < r; i++)// Get the L11 from the A11
            for(int j = 0; j < r; j++) {
                if(i < j)
                    L11[i][j] = 0;
                else if(i == j)
                    L11[i][j] = 1;
                else
                    L11[i][j] = A11[i][j];
            }
        if(L11_inv = triangle_matrix_inversion(L11,r,false)) { //Lower triangular matrix
            U12 = para_matrix_multiple(L11_inv,A12,r,r,n-r);
            free_matrix(L11_inv,r);
        }
        free_matrix(L11,r);
    }
    return U12;
}

float ** get_L21(float ** A11, float ** A21, int n, int r)
{
    if(!A11 || !A21)
        return NULL;
    float ** U11 = NULL, ** U11_inv = NULL, ** L21 = NULL;
    if(U11 = malloc_matrix(r,r)) {
        for(int i = 0; i < r; i++)// Get the U11 from the A11
            for(int j = 0; j < r; j++)
                U11[i][j] = (i <= j)?A11[i][j]:0;

        if(U11_inv = triangle_matrix_inversion(U11,r)) { //Upper triangular matrix
            L21 = para_matrix_multiple(A21,U11_inv,n-r,r,r);
            free_matrix(U11_inv,r);
        }
        free_matrix(U11,r);
    }
    return L21;
}

bool para_LU(float ** A, int n, int r)
// r is the max length of a matrix to call serial LU_Doolittle() function.
// The resolved matrix is stored in the original matrix A.
{
    float ** A11 = NULL, ** A12 = NULL, ** A21 = NULL, ** A22 = NULL;
    float ** U12 = NULL, ** L21 = NULL,** L21_U12 = NULL;
    bool ret_val = true;
    if(!A)
        return false;
    if(n <= r) {
        return LU_Doolittle(A,n);
    }
    A11 = malloc_matrix(r,r);
    A12 = malloc_matrix(r,n-r);
    A21 = malloc_matrix(n-r,r);
    A22 = malloc_matrix(n-r,n-r);
    if(!copy_matrix(A,A11,0,0,0,0,r,r) ||
            !copy_matrix(A,A12,0,r,0,0,r,n-r) ||
            !copy_matrix(A,A21,r,0,0,0,n-r,r) ||
            !copy_matrix(A,A22,r,r,0,0,n-r,n-r))
        ret_val = false;

    if(!LU_Doolittle(A11,r) || !copy_matrix(A11,A,0,0,0,0,r,r))
        ret_val = false;

    ff::para<float **> a,b;// No intersection between L21 and U12
    a([&A11,&A12,&n,&r]() {
        return get_U12(A11,A12,n,r);
    });
    b([&A11,&A21,&n,&r]() {
        return get_L21(A11,A21,n,r);
    });
    (a && b).then([&U12,&L21](float ** A, float ** B) {
        U12 = A;
        L21 = B;
    });

    if(!copy_matrix(L21,A,0,0,r,0,n-r,r) || !copy_matrix(U12,A,0,0,0,r,r,n-r))
        ret_val = false;

    L21_U12 = para_matrix_multiple(L21,U12,n-r,r,n-r);
    if(A22 && L21_U12) {// Matrix minus -- Need to be parallelized
        for(int i = 0; i<n-r; i++)
            for(int j=0; j<n-r; j++)
                A22[i][j] -= L21_U12[i][j];
    }
    if(!para_LU(A22,n-r,r) || !copy_matrix(A22,A,0,0,r,r,n-r,n-r))
        ret_val = false;
    free_matrix(L21_U12,n-r);
    free_matrix(U12,r);
    free_matrix(L21,n-r);
    free_matrix(A11,r);
    free_matrix(A12,r);
    free_matrix(A21,n-r);
    free_matrix(A22,n-r);
    return ret_val;
}

int main(int argc, char *argv[])
{
    string in_file_name = "../../benchmark/LU/matrix.txt";
    string out_file_name = "../../benchmark/LU/LU_matrix.txt";
    //int data[BUFFER_LEN],len,i;
    ifstream in_file;
    ofstream out_file;
    float ** matrix = NULL;
    int n,r;
    char tmp;

    if(in_file_name.empty() || out_file_name.empty()) {
        cout << "File name is empty!" << endl;
        return -1;
    }
    else {
        in_file.open(in_file_name.c_str());
        out_file.open(out_file_name.c_str());
    }
    if(!in_file.is_open()) {
        cout << "Can't open the file " << in_file_name << endl;
        return -1;
    }
    if(!out_file.is_open()) {
        cout << "Can't open the file " << out_file_name << endl;
        return -1;
    }
    in_file >> n;
    in_file >> tmp;
    in_file >> r;// The matrix length to call a serial Doolittle function.
    matrix = malloc_matrix(n,n);
    for(int i=0; !in_file.eof() && i < n; i++) {
        for(int j=0; !in_file.eof() && j < n; j++) {
            in_file >> matrix[i][j];
//             cout << matrix[i][j] << ' ';
        }
//         cout << endl;
    }
    in_file.close();


    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();

    //para_quick_sort(data,0,len-1);
    if(!para_LU(matrix,n,r))
        cout << "Fail to resolve the matrix." << endl;


    end = chrono::system_clock::now();

    int elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                          (end-start).count();

    cout << "Elapsed time: " << elapsed_seconds << "us\n";

//     for(int i=0; i<n; i++) {
//         for(int j=0; j<n; j++) {
//             out_file << matrix[i][j] << ' ';
//             cout << matrix[i][j] << ' ';
//         }
//         cout << endl;
//         out_file << endl;
//     }
    out_file << "Elapsed time: " << elapsed_seconds << "us" << endl;
    out_file.close();
    return elapsed_seconds;//return the parallel time to the system
}
