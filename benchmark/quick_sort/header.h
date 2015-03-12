#ifndef BENCHMARK_QUICK_SORT_HEADER_H_
#define BENCHMARK_QUICK_SORT_HEADER_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include "utils.h"

#define BUFFER_LEN 100000
#define INCREMENT 1000
#define SEPARATOR ','

using namespace std;

int partition(int *data,int start,int end);
void quick_sort(int *data,int start,int end);
void para_quick_sort(int * data,int i,int j,int para_len);
#endif
