import subprocess
import os

def execute_cmd(cmd):
	p = subprocess.Popen(cmd, shell=True, stdout = subprocess.PIPE)
	return p.stdout.read()

if __name__ == '__main__':
	path = execute_cmd('pwd').strip('\n')
	cmd = 'cd %s/build; rm -rf *;cmake ../; make;' % path
	execute_cmd(cmd)
	matrix_n = 1000
	rand_max = 5
	print 'file:random'
	execute_cmd('cd %s/build/benchmark; ./random %s %s' % (path,matrix_n,rand_max))

	file_all = execute_cmd('ls %s/build/benchmark' % path)
	file_list = [line for line in file_all.split('\n')]

	for item in file_list:
		ifile = r'%s/build/benchmark/%s' % (path, item)
		if os.path.isfile('%s' %ifile) and os.access('%s' % ifile, os.X_OK):
			if not cmp(item,'random'):
				continue
			elif not cmp(item,'LU'):
				para_n = matrix_n / 2
			elif not cmp(item,'quick_sort'):
				para_n = 4
			print 'file:%s' % item
			print 'Parallel time:'
			for i in range(3):
				print execute_cmd('cd %s/build/benchmark; ./%s %s' % (path, item, para_n)).strip('\n')
			time_path = '%s/build/benchmark/para_time.txt' % path
			time_file = open(time_path)
			ptime = 0
			for lines in time_file:
				ptime += int(lines.strip('\n'))
			ptime /= 3
			print 'Average parallel time: ' + str(ptime) + 'us'
			time_file.close()
			execute_cmd('rm %s' % time_path)
			execute_cmd('cd %s/build/benchmark; ./%s' % (path, item))			
			time_path2 = '%s/build/benchmark/time.txt' % path
			time_file = open(time_path2)
			print 'Serial time:' + time_file.read().strip('\n') + 'us'
			time_file.close()
			execute_cmd('rm %s' % time_path2)
	
