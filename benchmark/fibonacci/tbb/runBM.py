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
	cmd = 'cd %s; make clean; make;' % path
	execute_cmd(cmd)
	item = 'fib_tbb'
	para_n = 256
	print 'file:fib_tbb'
	print 'Parallel time:'
	time_path = '%s/para_time.txt' % path
	if os.path.exists(r'%s' %time_path):
		execute_cmd('rm %s' % time_path)
	cycle = 3
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
	#execute_cmd('rm %s' % time_path)
	execute_cmd('cd %s; ./%s' % (path, item))			
	time_path2 = '%s/time.txt' % path
	time_file = open(time_path2)
	print 'Serial time:' + time_file.read().strip('\n') + 'us'
	time_file.close()
	#execute_cmd('rm %s' % time_path2)
	execute_cmd('make clean')	
