#!/usr/bin/python
#author: Athrun Arthur (athrunarthur@gmail.com)

import common_config
import build
import json
import csv

gcc_o3_opts = ['-falign-functions',
	       '-falign-jumps',
	       '-falign-loops',
	       '-falign-labels',
	       '-freorder-blocks',
	       '-freorder-blocks-and-partition']
		 # , 
	     #  '-fprefetch-loop-arrays']


def generate_single_possible_opts(opts):
  res = {}
  for i in range(len(opts)):
    res[str(i)] = [opts[i]]
  return res

def generate_all_possible_opts(opts):
  res = {}
  for i in range(len(opts)):
    t= select_n_opts(opts, i+1)
    for (k, v) in t.items():
      res[k] = v
    
  return res

    
def select_n_opts(opts, i):
  res = []
  stack = []
  ii = 0
  while len(stack) != i:
    stack.append(ii)
    ii += 1
  res.append(list(stack))
  
  
  while len(stack) != 0:
    t = stack.pop()
    if len(opts) - t <= i-len(stack):
      continue
    
    while len(stack) != i:
      stack.append(t+1)
      t += 1
    res.append(list(stack))
    
  rres = res
  res = {}  
  for item in rres:
    s = ''
    t = []
    for it in item:
      t.append(opts[it])
      s += str(it) + '-'
      
    s = s[0:len(s)-1]
    res[s] = t
    
  return res


if __name__ == '__main__':
  build.cmake_move()
  opts =  generate_all_possible_opts(gcc_o3_opts)
  #opts = generate_single_possible_opts(gcc_o3_opts)
  cfg = common_config.CommonConfig
  res = {}
  for (k,v) in opts.items():
    build.build_ff_framework(v)
    setattr(cfg, 'CXX_OPT_FLAGS', ['-O2'] + v)
    time = build.build_and_run_all(cfg)
    res[k] = time
  
  print 'options: '
  print '    ' + str(opts)
  jstr = json.dumps(opts)
  opt_json_file = 'options.json'
  fp_opt = open(opt_json_file,'w')
  fp_opt.write(jstr)
  fp_opt.close()
  
  print '\nresults: ' 
  jstr = json.dumps(res)
  print '    ' + jstr
  
  #Write jsonfile
  json_file = 'results.json'
  fp = open(json_file,'w')
  fp.write(jstr)
  fp.close()
  
  #Write csvfile
  firstline = ['options', 'framework']
  for item in res[opts.keys()[0]]:
    firstline.append(item['name'])  
  csvfile = file('results.csv', 'wb')
  writer = csv.writer(csvfile)
  writer.writerow(firstline)
  for (k,v) in res.items():
    for framework in getattr(cfg, 'targets_to_build'):
      line = []
      #line.append(k)#use number to identify options
      line.append(opts[k])
      line.append(framework)
      for item in v:
	line.append(item[framework])
      writer.writerow(line)
  csvfile.close()
  
  build.cmake_recover()
