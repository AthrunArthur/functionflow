import subprocess
import os

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
	cmd = 'cd %s/build; rm -rf *;cmake ../; make;' % path
	execute_cmd(cmd)
	matrix_n = 10
	rand_max = 5
	#print 'file:random'
	#execute_cmd('cd %s/build/benchmark; ./random %s %s' % (path,matrix_n,rand_max))

	file_all = execute_cmd('ls %s/build/benchmark' % path)
	file_list = [line for line in file_all.split('\n')]

	for item in file_list:
		ifile = r'%s/build/benchmark/%s' % (path, item)
		if os.path.isfile('%s' %ifile) and os.access('%s' % ifile, os.X_OK):
			if not cmp(item,'random'):
				continue
			elif not cmp(item,'lu'):
				continue
				#para_n = matrix_n / 2
			elif not cmp(item,'quicksort'):
				para_n = 4
			print 'file:%s' % item
			print 'Parallel time:'
			time_path = '%s/build/benchmark/para_time.txt' % path
			if os.path.exists(r'%s' %time_path):
				execute_cmd('rm %s' % time_path)
			cycle = 3;
			for i in range(cycle):
				print execute_cmd('cd %s/build/benchmark; ./%s %s' % (path, item, para_n)).strip('\n')
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
			execute_cmd('rm %s' % time_path)
			execute_cmd('cd %s/build/benchmark; ./%s' % (path, item))			
			time_path2 = '%s/build/benchmark/time.txt' % path
			time_file = open(time_path2)
			print 'Serial time:' + time_file.read().strip('\n') + 'us'
			time_file.close()
			execute_cmd('rm %s' % time_path2)
	
