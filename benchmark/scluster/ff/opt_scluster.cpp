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
	std::vector<accumulator<Point> * > accs;
	std::vector<accumulator<int> * > accnums;
	Point sum0(dimension);
	for(int i = 0; i < means.size(); ++i)
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
	auto mean_size = means.size();

	ff::paracontainer pc;
	int last = start;
	size_t threshold = (1<<16)/sizeof(uint64_t);
	for(int i = start; i < end; i++)
	{
		if(i-last >= threshold || i == end - 1)
		{
			ff::para<> a;

			a([last, i, &accs, &accnums, this, &points, mean_size, &sum0](){
					std::vector<int> pSums;
					std::vector<Point> ps;
					for(int k = 0; k < mean_size; ++k)
					{
					pSums.push_back(0);
					ps.push_back(sum0);
					}

					for(int k = last; k < i; ++k)
					{
					int ci = assignment(points.at(k));
					pSums[ci] += 1;
					ps[ci] += points.at(k);
					}

					for(int k=0; k < mean_size; ++k)
					{
					accnums[k]->increase(pSums[k]);
					accs[k]->increase(ps[k]);
					}
			});
			pc.add(a);
			last = i;
		}
	}
	ff_wait(all(pc));

	for(int i = 0; i < means.size(); ++i)
	{
		means[i] = accs[i]->get()/accnums[i]->get();
		delete accs[i];
		delete accnums[i];
	}
}
