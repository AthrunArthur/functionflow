#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#define RANDOM_MAX 5//Set this value not too big in case of overflow (the right-down corner would get the wrong result)
#define RANDOM_MIN 2
#define DEFAULT_LEN 1000
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

int main(int argc, char *argv[])
{
    string out_file_name = "../../benchmark/LU/matrix.txt";
    string outLU_file_name = "../../benchmark/LU/matrixLU.txt";
    int ** L_matrix,**U_matrix,**LU_matrix,n = DEFAULT_LEN,max = RANDOM_MAX;// max no more than 1000 recommended
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
    LU_matrix = matrix_multiple(L_matrix,U_matrix,n,n,n);
//     cout << "LU matrix:" << endl;
    out_file << n << endl;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            out_file << LU_matrix[i][j] << '\t';
        }
        out_file << endl;
    }
    out_file.close();
    outLU_file.close();
    free_matrix(LU_matrix,n);
    free_matrix(L_matrix,n);
    free_matrix(U_matrix,n);
    return 0;

}
