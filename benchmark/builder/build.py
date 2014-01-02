#!/usr/bin/python
# author: Athrun Arthur (athrunarthur@gmail.com)

import os
import benchmark_configs
import common_config
import inspect
import subprocess
import json

current_file = os.path.abspath(__file__)
current_dir = os.path.dirname(current_file)
benchmark_base_dir = os.path.dirname(current_dir)
build_dir = benchmark_base_dir + '/build/'
ff_base_dir = os.path.dirname(benchmark_base_dir)


def execute_cmd(cmd):
  p = subprocess.Popen(cmd, shell=True, stdout = subprocess.PIPE)
  return p.stdout.read()


def get_all_cpp_files_in_dir(abs_dir):
  print abs_dir
  files = []
  for item in os.listdir(abs_dir):
    if item == 'test.cpp':
      continue
    if item == 'makeData.cpp':
      continue    
    if item.endswith('.cpp'):
      files.append(abs_dir+'/' + item)
      
  return files

#build_flags can be 'ff', 'tbb' or 'openmp'
def generate_building_cmd(common_config, bms_config, cpp_files, build_flags):
  cmd = getattr(common_config, 'CXX') + ' '
  for item in getattr(common_config, 'CXX_FLAGS'):
    cmd += str(item) + ' '
    
  tbf = ''
  if build_flags == 'tbb':
    tbf = 'TBB'
  elif build_flags == 'ff':
    tbf = 'FF'
  elif build_flags == 'openmp':
    tbf = 'OPENMP'
    
  for item in getattr(common_config, tbf + '_FLAGS'):
    cmd += str(item) + ' '
  
  for item in getattr(common_config, tbf + '_INCLUDE_DIRS'):
    if inspect.isfunction(item):
      cmd += '-I' + item(ff_base_dir) + ' '
    else:
      cmd += '-I' + str(item) + ' '
    
  for item in getattr(common_config, tbf + '_LINK_DIRS'):
    if inspect.isfunction(item):
      cmd += '-L' + item(ff_base_dir) + ' '
      cmd += '-Wl,-rpath=' + item(ff_base_dir) + ' '
    else:
      cmd += '-L' + str(item) + ' '
      cmd += '-Wl,-rpath=' + str(item) + ' '
      
    
  for item in getattr(common_config, tbf + '_LINK_LIBS'):
    cmd += '-l' + str(item) + ' '
    
  for item in getattr(common_config, 'CXX_OPT_FLAGS'):
    cmd += str(item) + ' '
    
  for item in cpp_files:
    cmd += item + ' '
  
  #import wxWidgets for canny
  if not cmp('canny',getattr(bms_config, 'name')):
    cmd += '`wx-config --cflags --libs`' + ' '
  
  output_file= build_dir + getattr(bms_config, 'name') + '_' +build_flags
  
  cmd += ' -o ' + output_file
  
  return cmd
  
def build_one_bm(common_config, bms_config, abs_sub_dir, flag):
  files = get_all_cpp_files_in_dir(abs_sub_dir)
  build_cmd = generate_building_cmd(common_config, bms_config, files, flag)
  
  print 'building with cmd...'
  print '    ' + build_cmd
  res = execute_cmd(build_cmd)


def build(common_config, bms_configs):
  for item in bms_configs:
    bm_dir = benchmark_base_dir + '/' + getattr(item, 'path')
    for sub_dir in os.listdir(bm_dir):
      if not sub_dir in getattr(common_config, 'targets_to_build'):
	continue
      abs_sub_dir = os.path.abspath(bm_dir + '/' + sub_dir)
      build_one_bm(common_config, item, abs_sub_dir, sub_dir)
  pass

def cmake_move():
  if os.path.exists(r'%s/CMakeLists.txt' %ff_base_dir):
    execute_cmd('mv %s/CMakeLists.txt %s/CMakeLists.txt.org;' %(ff_base_dir,ff_base_dir))
  
def cmake_recover():
  if os.path.exists(r'%s/CMakeLists.txt' %ff_base_dir):
    execute_cmd('rm %s/CMakeLists.txt;' %ff_base_dir)
  if os.path.exists(r'%s/CMakeLists.txt.org' %ff_base_dir):
    execute_cmd('mv %s/CMakeLists.txt.org %s/CMakeLists.txt;' %(ff_base_dir,ff_base_dir))

def build_ff_framework(opt):
  mdfile = ff_base_dir + '/CMakeLists.md'
  cmakefile = ff_base_dir + '/CMakeLists.txt'
  mdfp = file(mdfile, 'r')
  cmfp = file(cmakefile, 'w')
  for s in mdfp.readlines():
      if (s.find('${OPTIONS-NEED-TO-ADD}') != -1):
	for item in opt:
	  cmfp.write(s.replace('${OPTIONS-NEED-TO-ADD}','add_definitions(%s)' %item))
      else:
	cmfp.write(s)
  mdfp.close()
  cmfp.close()
  if not os.path.exists(r'%s/build' %ff_base_dir):
    execute_cmd('cd %s; mkdir build;' %ff_base_dir)
  path = '%s/build' %ff_base_dir
  cmd = 'cd %s; rm -rf *;cmake -DRelease=1 ../; make;' % path
  execute_cmd(cmd)  
  
def run(common_config, bms_configs, times):
  res = {}
  for item in bms_configs:
    res[getattr(item, 'name')] = run_one_bm(common_config, item, times)
    
  return res
    
def run_one_bm(common_config, bms_config, times):
  exe_base = build_dir + getattr(bms_config,'name')
  exe_files = {}
  for item in getattr(common_config, 'targets_to_build'):
    exe_file = exe_base + '_' + item
    if os.path.exists(exe_file):
      exe_files[item] = exe_file

  print exe_files
  res = {}
  for (k, v) in exe_files.items():
    args = ''
    res[k] = []
    for arg in getattr(bms_config, 'params'):
      args += arg + ' '
    cmd = v + ' ' + args
    print cmd
    for i in range(times):
      execute_cmd(cmd)
      json_file = current_dir + '/' + getattr(common_config, 'exe_log_file')
      fp = open(json_file)
      rs = fp.read()
      jrs = json.loads(rs)
      fp.close()
      res[k].append(jrs)
  return res

def build_and_run_all(common_config):
  if not os.path.exists(build_dir):
    execute_cmd('cd %s; mkdir build;' %benchmark_base_dir)
  #bms = [benchmark_configs.CANNY]
  bms = [benchmark_configs.LU,benchmark_configs.CANNY,benchmark_configs.QSORT,benchmark_configs.NQUEEN,benchmark_configs.FIB,benchmark_configs.KMEANS]
  build(common_config, bms)
  #res= run(common_config, bms, 5)
  res= run(common_config, bms, 3)
#  return reduce_res(res,'para-elapsed-time')
  return reduce_res(res,'sequential-elapsed-time')


def reduce_res(input_res,time_flag):
  res = []
  for (k, v) in input_res.items():
    t = {}
    t['name'] = k
    for (ik, iv) in v.items():
      avg_time = 0
      counts = 0
      for item in iv:
#	avg_time += int(item['para-elapsed-time'])
	avg_time += int(item[time_flag])
	counts += 1
      avg_time = avg_time/counts
      t[ik] = avg_time
    
    res.append(t)
    
  return res


if __name__=='__main__':
  if not os.path.exists(build_dir):
    execute_cmd('cd %s; mkdir build;' %benchmark_base_dir)
  print 'This is for test!!'  
#  bms = [benchmark_configs.CANNY]
  bms = [benchmark_configs.LU,benchmark_configs.CANNY,benchmark_configs.QSORT,benchmark_configs.MUTEX,benchmark_configs.NQUEEN,benchmark_configs.FIB,benchmark_configs.KMEANS]
  build(common_config.CommonConfig, bms)
  print run(common_config.CommonConfig, bms, 1)
