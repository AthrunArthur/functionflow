import sys
import os
import struct

basic_fp = 'wsr.dat'
fp_num = 8

def read_all_records():
    res = []
    for line in open('queue_acc.dat', 'r'):
        ls = line.split('\t')
        record = {}
        for l in ls:
            if len(l.strip()) == 0:
                continue
            ts = l.split(':')
            record[ts[0]] = int(ts[1])
        res.append(record)

    return res

if __name__ == "__main__":
    #read_all_records()
    i = 0
    rs = read_all_records()
    zq = [x for x in rs if x['queue_id'] == 0]
    szq = sorted(zq, key=lambda d: d['rid'])
    szq = [x for x in szq if x['op_res'] == 1]
    i = 0
    max_index = 0
    for item in szq:
        if item['op_res'] == 1:
            max_index = i
        i += 1

    rszq = szq[max_index - 500 : ]

    fh = open('final.dat', 'w')
    for item in rszq:
        if item['op_res'] == 1:
            print item['rid']
        fh.write( str(item) + '\n')
