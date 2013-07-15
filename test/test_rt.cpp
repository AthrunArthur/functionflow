
//#include "common/log.h"
#include "runtime/runtime.h"


int main(int argc, char *argv[])
{	
	//ff::log<>::init(ff::INFO, "log.txt");
	//LOG_INFO(main)<<"main start";

	ff::rt::threadpool tp;
	tp.run(std::thread::hardware_concurrency(), [](){
			std::cout<<"enter thread"<<std::endl;
			});
	tp.join();
	std::cout<<"init runtime..."<<std::endl;
	ff::rt::runtime::instance();
	
	return 0;
}