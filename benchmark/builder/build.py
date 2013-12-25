#!/usr/bin/python
# author: Athrun Arthur (athrunarthur@gmail.com)

import os
import benchmark_configs
import common_config
import inspect
import subprocess

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
  
  
def run(common_config, bms_configs, times):
  for item in bms_configs:
    run_one_bm(common_config, item, times)
    
def run_one_bm(common_config, bms_config, times):
  exe_base = build_dir + getattr(bms_config,'name')
  exe_files = {}
  for item in getattr(common_config, 'targets_to_build'):
    exe_file = exe_base + '_' + item
    if os.path.exists(exe_file):
      exe_files[item] = exe_file

  print exe_files
  
  for (k, v) in exe_files.items():
    args = ''
    for arg in getattr(bms_config, 'params'):
      args += arg + ' '
    cmd = v + ' ' + args
    print cmd
    execute_cmd(cmd)

if __name__=='__main__':
  print 'This is for test!!'
  bms = [benchmark_configs.LU]
  build(common_config.CommonConfig, bms)
  run(common_config.CommonConfig, bms, 1)