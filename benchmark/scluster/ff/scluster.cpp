#include "../header.h"
#include "../Lloyd.h"
#include "../point.h"
#include "ff.h"

using namespace ff;

void Lloyd::update(Points & points, int start, int end)
{
	if(!last_means.empty())
		last_means.clear();
	copy(means.begin(),means.end(),back_inserter(last_means));

	int dimension = means.at(0).dimension;
	Point sum0(dimension);
	std::vector<accumulator<Point> * > accs;
	std::vector<accumulator<int> * > accnums;

	for(int i =0; i < means.size(); ++i)
	{
		accs.push_back(new accumulator<Point>(sum0, [](const Point& x, const Point& y)
					{
					return x + y;
					}));
		accnums.push_back(new accumulator<int>(0, [](const int& x, const int& y)
					{
					return x + y;
					}));
	}

	ff::paracontainer pc;
	for(int i = start; i < end; i++)
	{
		ff::para<> a;
		a([this,&accs, &accnums, &points, i](){
				int ci = assignment(points.at(i));
				accnums[ci]->increase(1);
				accs[ci]->increase(points.at(i));
				});
		pc.add(a);
	}

	ff_wait(all(pc));

	for(int i = 0; i < means.size(); ++i)
	{
		means[i] = accs[i]->get()/accnums[i]->get();
		delete accs[i];
		delete accnums[i];
	}
}
