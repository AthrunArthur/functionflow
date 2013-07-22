import subprocess
import os

def execute_cmd(cmd):
	p = subprocess.Popen(cmd, shell=True, stdout = subprocess.PIPE)
	return p.stdout.read()

if __name__ == '__main__':
	path = execute_cmd('pwd').strip('\n')
	cmd = 'cd %s; make clean; make;' % path
	execute_cmd(cmd)
	para_n = 4
	print 'file:qsort_tbb'
	print 'Parallel time:'
	time_path = '%s/para_time.txt' % path
	if os.path.exists(r'%s' %time_path):
		execute_cmd('rm %s' % time_path)
	for i in range(3):
		print execute_cmd('cd %s; ./qsort_tbb %s' % (path, para_n)).strip('\n')
	time_file = open(time_path)
	ptime = 0
	for lines in time_file:
		ptime += int(lines.strip('\n'))
	ptime /= 3
	print 'Average parallel time: ' + str(ptime) + 'us'
	time_file.close()
	execute_cmd('rm %s' % time_path)
	execute_cmd('cd %s; ./qsort_tbb' % path)			
	time_path2 = '%s/time.txt' % path
	time_file = open(time_path2)
	print 'Serial time:' + time_file.read().strip('\n') + 'us'
	time_file.close()
	execute_cmd('rm %s' % time_path2)
	
