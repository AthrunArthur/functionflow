import os
import numpy
import matplotlib.pyplot as plt
import sys

cur_dir = os.path.dirname(os.path.realpath(__file__))
cur_dir = os.path.join(cur_dir, './build/')

def read_mutex_time(path):
    fp = open(path, 'r')
    res = []
    for line in fp:
        t = int(line.strip())
        res.append(t)

    return res

paths = ['mutex_0', 'mutex_1', 'mutex_2', 'mutex_3']
cdf_field=[100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000, 3000, 4000, 5000, 10000, 100000, 1000000, 10000000, sys.maxint]

def get_cdf(ts):
    acdfs = cdf_field
    res = numpy.arange(len(acdfs))

    for d in ts:
        i = 0
        while i < len(acdfs)  and acdfs[i] < d:
            i += 1
        res[i] += 1
    rr = []
    for d in res:
        t = 1.0 * d/ len(ts)
        if len(rr) == 0:
            rr.append(t)
        else:
            rr.append(t + rr[len(rr) -1])

    return rr




def draw_all_mutex_time(paths):
    fig = plt.figure()
    ax = fig.add_subplot(111)
    for p in paths:
        print 'drawing ' + str(p)
        fp = os.path.join(cur_dir, p)
        ts = read_mutex_time(fp)
        cdf = get_cdf(ts)
        print 'data done!'

        x = numpy.arange(len(cdf))
        print x
        ax.plot(x, cdf)
        ax.set_xticks(x)
        print '...'
        ax.set_xticklabels([str(x) for x in cdf_field], rotation=45)
        #plt.savefig(fp+'.png')
        #plt.close()
    plt.grid(b='on')    
    plt.show()    


if __name__ == "__main__":
    draw_all_mutex_time(paths)

