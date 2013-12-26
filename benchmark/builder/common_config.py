#/usr/bin/python
#author: Athrun Arthur (athrunarthur@gmail.com)

class CommonConfig:
  CXX='g++'
  FF_FLAGS = ['-DUSING_LOCK_FREE_QUEUE']
  FF_INCLUDE_DIRS = [lambda ff_base_dir: ff_base_dir + '/ff/include']
  FF_LINK_DIRS = [lambda ff_base_dir: ff_base_dir + '/bin']
  FF_LINK_LIBS = ['ff', 'pthread']
  
  TBB_FLAGS = []
  TBB_INCLUDE_DIRS = []
  TBB_LINK_DIRS = []
  TBB_LINK_LIBS = ['tbb', 'rt']
  
  OPENMP_FLAGS = ['-fopenmp']
  OPENMP_INCLUDE_DIRS = []
  OPENMP_LINK_DIRS = []
  OPENMP_LINK_LIBS = ['rt']
  
  CXX_FLAGS = ['-std=c++11', '-DNDEBUG']
  CXX_OPT_FLAGS = ['-O2']

  targets_to_build = ['ff','tbb','openmp']
  exe_log_file = 'time.json'
  