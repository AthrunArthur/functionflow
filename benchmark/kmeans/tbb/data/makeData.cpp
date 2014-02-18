//running command:g++ -lgsl -lgslcblas -o makeData -g makeData.cpp

#include <iostream>
#include <fstream>
#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h> // probability distributions

#define GAUSSIANLEN 50000
#define DIMENSION 3

using namespace std;


void gaussian_generator(string fileName)
{
    gsl_rng *r = gsl_rng_alloc(gsl_rng_ranlxs0); //generator r, gsl_rng_ranlxs2 = ranlux!

    fstream gaussianFile;
    gaussianFile.open(fileName.c_str(), fstream::out | fstream::trunc);
    double rnum[DIMENSION];
    if(gaussianFile.is_open()) {
        for (int i=0; i < GAUSSIANLEN; i++) {
            srand(time(NULL)+i);
            for(int j=0; j < DIMENSION; j++) {
                rnum[j] = gsl_ran_gaussian(r,1.0) + rand() % (DIMENSION*2);
                gaussianFile << rnum[j] << " ";
            }
            gaussianFile << endl;
        }
    }
    else
        cout << "Can't open file: " << fileName << endl;
    gaussianFile.close();
    gsl_rng_free(r);
} 


int main()
{
    string fileName = "gaussian.txt";
    fstream gaussianFile;
    
    if(fileName.empty()) {
        cout << "File name is empty!" << endl;
        return -1;
    }
    else {
        gaussianFile.open(fileName.c_str(),ios::in);
//         cout << "trying open file: " << fileName << endl;
    }
    if(!gaussianFile.is_open()) {
        cout << "Generate now..." << endl;
        gaussian_generator(fileName);
    }
    return 1;
}
