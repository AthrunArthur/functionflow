#!/usr/bin/python
#author: Athrun Arthur (athrunarthur@gmail.com)

class BMConfig:
  pass
  
class LU(BMConfig):
  name = 'lu'
  path = 'LU'
#  params = ['1']
  params = ['0']
  
class FIB(BMConfig):
  name = 'fib'
  path = 'fibonacci'
#  params = ['1']
  params = ['0']

class CANNY(BMConfig):
  name = 'canny'
  path = 'canny'
#  params = ['1']
  params = ['0']
  
class KMEANS(BMConfig):
  name = 'kmeans'
  path = 'kmeans'
#  params = ['1']
  params = ['0']
  
class MUTEX(BMConfig):
  name = 'mutex'
  path = 'mutex'
#  params = ['1']
  params = ['0']
  
class NQUEEN(BMConfig):
  name = 'nqueen'
  path = 'n_queen'
#  params = ['1']
  params = ['0']
  
class PHASH(BMConfig):
  name = 'phash'
  path = 'parallel_hash_insert'
  params = ['1']
  
class QSORT(BMConfig):
  name = 'qsort'
  path = 'quick_sort'
#  params = ['256']
  params = ['1']
  
