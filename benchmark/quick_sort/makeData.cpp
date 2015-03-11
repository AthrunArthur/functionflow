#include<iostream>
#include<fstream>
#include<cstdlib>
using namespace std;
int main()
{
	ofstream out("numbers.txt");
	for(int i = 0; i < 18000000; ++i)
	{
		out << rand() % 10000 - 5000 << ",";
	}
	out << 1;
	return 0;
}
