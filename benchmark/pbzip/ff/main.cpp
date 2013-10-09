#include <bzlib.h>
#include <chrono>
#include <string>
#include <fstream>

#include <ff.h>

using namespace std;
using namespace ff;

int BWTblockSize = 9; //from bzip2, 9 is best, 1 is fast.


int do_seq_compress(ifstream & infile, ofstream & outfile, long infilesize, int blockSize)
{
  long leftbytes = infilesize;
    while(leftbytes > 0)
    {
      //1. read file
        int toreadsize= leftbytes > blockSize ? blockSize : leftbytes;
        char *readbuf = new (std::nothrow) char[toreadsize];
        if(readbuf == nullptr) {
            cout<<" allocate error!"<<endl;
            return 1;
        }
        infile.read(readbuf, toreadsize);
	streamsize readsize = infile.gcount();
	if(readsize != toreadsize)
	{
	  cout<<"read error! aborting..."<<endl;
	  return 1;
	}
        unsigned int compresssize = toreadsize + 6*1024;
        char *cmpbuf = new (std::nothrow) char[compresssize];
        if(cmpbuf == nullptr) {
            cout <<" allocate error!"<<endl;
            return 1;
        }

        //2. compress
        int ret = BZ2_bzBuffToBuffCompress(cmpbuf, &compresssize, readbuf, readsize, BWTblockSize, 0, 30);
        if (ret != BZ_OK)
        {
            cout<<"error during compressing, aborting..."<<endl;
            return 1;
        }
        
        //3. writefile
        outfile.write(cmpbuf, compresssize);
	leftbytes = leftbytes - readsize;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    string infilename = "/home/athrun/projects/pbzip2-1.1.6/pbzip2.cpp";
    string outfilename = "/home/athrun/projects/pbzip2-1.1.6/pbzip2.cpp.bz2";
    int blockSize = 1024*1024; //bytes
    
    ifstream infile;
    scope_guard sg_infile([&infile, &infilename]() {infile.open(infilename, ios::binary | ios::in);},
			   [&infile]() {infile.close();});
    ofstream outfile;
    scope_guard sg_outfile([&outfile, &outfilename]() {outfile.open(outfilename, ios::binary | ios::out);},
			    [&outfile]() {outfile.close();});

    if(!infile.is_open()) {
        cout<<"cannot open file: "<< infilename<<endl;
        return 1;
    }
    if(!outfile.is_open()) {
        cout<<"cannot open file: "<<outfilename<<endl;
        return 1;
    }

    infile.seekg(0, ios::end);
    long insize = infile.tellg();
    infile.seekg(0, ios::beg);

    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();

    do_seq_compress(infile, outfile, insize, blockSize);
    
    end = chrono::system_clock::now();

    return 0;
}