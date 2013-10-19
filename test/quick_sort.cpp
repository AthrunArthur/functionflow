/***********************************************
The MIT License (MIT)

Copyright (c) 2012 Athrun Arthur <athrunarthur@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*************************************************/
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
// #include <ctime>
#include "ff.h"
#define MIN_PARA_LEN 5
#define BUFFER_LEN 1000
#define SEPARATOR ','
using namespace ff;
using namespace std;

/* partition -- Partition the data from start to end to two parts
 * with the left part is less than the pivot, and the right part is larger.
 * the return value is the index of the partition pivot */
int partition(int *data,int start,int end)
{
    int pivot;
    int i, j;
    int tmp;
    pivot = data[end];
    i = start-1;
    for(j=start; j<end; j++)
        if(data[j]<=pivot)
        {
            i++;/* i is the number of data less than the pivot before the index j. */
            tmp=data[i];
            data[i]=data[j];
            data[j]=tmp;
        }
    tmp=data[i+1];
    data[i+1]=data[end];
    data[end]=tmp;
    /* data[i+1]=pivot */
    return i+1;
}

/* quick_sort -- Use the serial quick sort algorithm to sort data. */

void quick_sort(int *data,int start,int end)
{
    int r;
    int i;
    if(start < end)
    {
        r = partition(data,start,end);
        quick_sort(data,start,r-1);
        quick_sort(data,r+1,end);
    }
}

/* parallel quick sort using ff::para. */
void para_quick_sort(int * data,int i,int j)
{
    int r;    
    if(j-i <= MIN_PARA_LEN-1)/*The length is too small.*/
        quick_sort(data,i,j);
    else
    {
        r = partition(data,i,j);
        ff::para<> a,b;
        a([&data,&i,&r]() {
            para_quick_sort(data,i,r-1);
        });
        b([&data,&r,&j]() {
            para_quick_sort(data,r+1,j);
        });
        (a&&b).then([]() {});
    }
}

int main(int argc, char *argv[])
{
    string in_file_name = "../../test/numbers.txt";
    string out_file_name = "../../test/numbers_sort.txt";
    int data[BUFFER_LEN],len,i;
    ifstream in_file;
    ofstream out_file;    

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
    for(i=0; !in_file.eof() && i < BUFFER_LEN; i++) {
        char tmp;
        in_file >> data[i];
        in_file.get(tmp);
        cout << data[i] << tmp;
        if(tmp != SEPARATOR) {
            i++;
            break;
        }
    }
    if(!in_file.eof() && i > BUFFER_LEN) {
        cout << "Max data length is " << BUFFER_LEN << endl;
        cout << "Only the first " << BUFFER_LEN << " numbers will be sorted" << endl;
    }
    len = i;
    cout << "len = " << len << endl;
    in_file.close();

    
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();

    para_quick_sort(data,0,len-1);   
    
    end = chrono::system_clock::now();
 
    int elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                             (end-start).count();
    
//     time_t end_time = chrono::system_clock::to_time_t(end);
//  
//     cout << "finished computation at " << ctime(&end_time);
    cout << "elapsed time: " << elapsed_seconds << "us\n";

    for(i=0; i<len; i++) {
        out_file << data[i];
        cout << data[i];
        if(i < len - 1) {
            out_file << SEPARATOR;
            cout << SEPARATOR;
        }
    }
    cout << endl;
    out_file.close();
    return 0;
}
