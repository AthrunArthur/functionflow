#include<iostream>
#include<fstream>
#include<cstdlib>
#define MAXN 10000000
using namespace std;
int main()
{
	ofstream out("numbers.txt");
	for(int i = 0 ; i < MAXN; ++i)
	{
		if (i % 100 == 0)
			out << endl;
		out << abs(rand()) <<" ";
	}
	out.close();
}
