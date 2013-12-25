import subprocess
import os
import sys

def bubblesort(numbers):
    for j in range(len(numbers)-1,-1,-1):
        for i in range(j):
            if numbers[i]>numbers[i+1]:
                numbers[i],numbers[i+1] = numbers[i+1],numbers[i]

def execute_cmd(cmd):
	p = subprocess.Popen(cmd, shell=True, stdout = subprocess.PIPE)
	return p.stdout.read()

if __name__ == '__main__':
	path = execute_cmd('pwd').strip('\n')
	if not os.path.exists(r'%s/build' %path):
		execute_cmd('cd %s; mkdir build;' %path)
	path = '%s/build' %path
	cmd = 'cd %s; rm -rf *;cmake -DAll=1 ../; make;' % path
	execute_cmd(cmd)

	file_all = execute_cmd('ls %s' % path)
	file_list = [line for line in file_all.split('\n')]
	if len(sys.argv) > 1:
		for item in file_list:
			if item not in sys.argv:
				#Shouldn't remove the item or the pointer may change, thus not deleting all the unmatched.
				file_list[file_list.index(item)]=""
	
	for item in file_list:
		ifile = r'%s/%s' % (path, item)
		if os.path.isfile('%s' %ifile) and os.access('%s' % ifile, os.X_OK):
			if not cmp(item,'lu'):
				para_n = 1 
			elif not cmp(item,'quicksort'):
				para_n = 256
			elif not cmp(item,'canny'):
				para_n = 1
			elif not cmp(item,'kmeans'):
				para_n = 1
			elif not cmp(item,'nqueen'):
				para_n = 1
			elif not cmp(item,'phash'):
				para_n = 1
			elif not cmp(item,'mutex'):
				para_n = 1
			elif not cmp(item,'fib'):
				para_n = 1
			print 'file:%s' % item
			print 'Parallel time:'
			time_path = '%s/para_time.txt' % path
			if os.path.exists(r'%s' %time_path):
				execute_cmd('rm %s' % time_path)
			cycle = 3;
			for i in range(cycle):
				print execute_cmd('cd %s; ./%s %s' % (path, item, para_n)).strip('\n')
			time_file = open(time_path)
			ptime = 0
			etime = 1
			time_arr = []
			for lines in time_file:
				cur_time = int(lines.strip('\n'))
				ptime += cur_time
				etime *= cur_time
				time_arr.append(cur_time)
			ptime /= cycle
			etime = int(etime**(1.0/cycle))
			bubblesort(time_arr)
			print 'Median time:' + str(time_arr[cycle/2]) + 'us'
			print 'Arithmetic mean time: ' + str(ptime) + 'us'
			print 'Geometric mean time: ' + str(etime) + 'us'
			time_file.close()
			#test std::mutex
			if not cmp(item,'mutex'):
				print 'Use std::mutex'	
				print execute_cmd('cd %s; ./%s %s 1' % (path, item, para_n)).strip('\n')
			execute_cmd('rm %s' % time_path)			
			if not cmp(item,'phash'):
				continue;
			execute_cmd('cd %s; ./%s' % (path, item))			
			time_path2 = '%s/time.txt' % path
			time_file = open(time_path2)
			print 'Serial time:' + time_file.read().strip('\n') + 'us'
			time_file.close()
			execute_cmd('rm %s' % time_path2)
