import os
import sys
import json

cur_dir = os.path.dirname(os.path.realpath(__file__))

def isnum(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

#implement this method to return data for each time's command line!
def parse_tmp_file(fp):
    res = {}
    '''
    keys = ['SerialSumTree', 'SimpleParallelSumTree',
            'OptimizedParallelSumTree',
            'FFSumTree',
            'OptFFSumTree']

    for line in open(fp, 'r'):
        line = line.strip()
        ns = [float(s) for s in line.split() if isnum(s)]
        if len(ns) == 0:
            continue
        n = ns[0]
        lk = line.split()
        if len(lk) == 0:
            continue
        tk = lk[0].strip(':')
        if tk in keys:
            res[tk] = n
    '''
    keys = ['qsort', 'kmeans', 'fib', 'nqueen', 'lu', 'mutex']
    data = json.loads(open(fp, 'r').read())
    for k in keys:
        td = data[k]['ff'][0]['para-elapsed-time']
        res[k] = td
    print res
    return res

def main():
    fp = os.path.join(cur_dir, sys.argv[1])
    res_fp = os.path.join(cur_dir, sys.argv[2])
    prefix = ''
    if len(sys.argv) == 4:
        prefix = str(sys.argv[3])
    old_data = []
    if os.path.exists(res_fp):
        old_data = json.loads(open(res_fp, 'r').read())

    d = parse_tmp_file(fp)
    r = {}
    for k, v in d.items():
        r[prefix + k] = v

    old_data.append(r)
    open(res_fp, 'w').write(json.dumps(old_data))


if __name__ == '__main__':
    main()

