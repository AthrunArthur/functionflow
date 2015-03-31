#include "../header.h"
#include "../Lloyd.h"
#include "../point.h"
#include "tbb/parallel_for.h"
#include "tbb/mutex.h"

using namespace tbb;
typedef tbb::mutex TMutex;
typedef std::shared_ptr<TMutex> TMutex_ptr;
std::vector<TMutex_ptr> p_mutexes,i_mutexes;
typedef std::shared_ptr<Point> Point_ptr;
std::vector<Point_ptr> point_sums;
typedef std::shared_ptr<int> Int_ptr;
std::vector<Int_ptr> num_sums;

struct Calculate {
	Points & c_points;
	Lloyd & c_lloyd; 
	int c_size;
	int c_dimension;
	Calculate(Points & points,Lloyd & lloyd,int size,int dimension):c_points(points),
	c_lloyd(lloyd),c_size(size),c_dimension(dimension){}
	void operator()( const blocked_range<int>& range ) const {
		std::vector<Point_ptr> c_point_sums;
		std::vector<Int_ptr> c_num_sums;
		Point sum0(c_dimension);
		for(int i =0; i < c_size; ++i)
	    {		
			c_point_sums.push_back(std::make_shared<Point>(sum0));
			c_num_sums.push_back(std::make_shared<int>(0));
		}	
        for( int i=range.begin(); i!=range.end(); ++i ){
			int ci = c_lloyd.assignment(c_points.at(i));
			*(c_point_sums[ci])+=c_points.at(i);
			*(c_num_sums[ci])+=1;
		}
		for(int i =0; i < c_size; ++i)
	    {		
			TMutex::scoped_lock lock;
			lock.acquire(*(p_mutexes[i]));
			*(point_sums[i])+=*(c_point_sums[i]);
			lock.release();
			lock.acquire(*(i_mutexes[i]));
			*(num_sums[i])+=*(c_num_sums[i]);
			lock.release();
		}
		
		
    }
};


void Lloyd::update(Points & points, int start, int end)
{
	if(!last_means.empty())
		last_means.clear();
	copy(means.begin(),means.end(),back_inserter(last_means));

	int dimension = means.at(0).dimension;
	Point sum0(dimension);

	for(int i =0; i < means.size(); ++i)
	{
		p_mutexes.push_back(std::make_shared<TMutex>());
		i_mutexes.push_back(std::make_shared<TMutex>());
        point_sums.push_back(std::make_shared<Point>(sum0));
		num_sums.push_back(std::make_shared<int>(0));
	}	
	
	Calculate cal(points,*this,means.size(),dimension);
	int threshold = (1<<16)/sizeof(uint64_t);
	parallel_for(blocked_range<int>(start,end,threshold),cal);
	
	for(int i = 0; i < means.size(); ++i)
	{
		means[i] = (*(point_sums[i]))/(*(num_sums[i]));
	}
}
