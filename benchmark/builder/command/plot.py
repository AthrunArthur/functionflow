import os
import sys
import json
import pgf

cur_dir = os.path.dirname(os.path.realpath(__file__))

def collect_data_for_key(data, key):
    res = []
    for oc in data:
        if not oc.has_key(key):
            continue
        res.append(float(oc[key]))

    return res

def plot_avg(key_and_data):
    d = [(x[0], sum(x[1])/len(x[1])) for x in key_and_data.items()]
    ps = pgf.PlotSet('avgbar')
    p = pgf.Plot()
    p.config(ps)
    for k, v in d:
        p.addplot([v], lambda x: 1, lambda x: x, '', k)
    return p.dump()


def plot_seq(key_and_data):
    ps = pgf.PlotSet('seqline')
    p = pgf.Plot()
    p.config(ps)
    for k, v in key_and_data.items():
        d = zip(v, range(1, len(v) + 1))
        p.addplot(d, lambda x: x[1], lambda x: x[0], '', k)
    return p.dump()

def plot_sort(key_and_data):
    ps = pgf.PlotSet('sortline')
    p = pgf.Plot()
    p.config(ps)
    for k, v in key_and_data.items():
        d = zip(sorted(v), range(1, len(v) + 1))
        p.addplot(d, lambda x: x[1], lambda x: x[0], '', k)
    return p.dump()
    pass

def plot_data(input_data, avg_fp, seq_fp, sort_fp):
    keys = []
    key_and_data = {}
    for i in range(2, len(sys.argv)):
        keys.append(sys.argv[i])
    for k in keys:
        key_and_data[k] = collect_data_for_key(input_data, k)
    print key_and_data

    open(avg_fp, 'w').write(plot_avg(key_and_data))

    open(seq_fp, 'w').write(plot_seq(key_and_data))

    open(sort_fp, 'w').write(plot_sort(key_and_data))
    return [avg_fp, seq_fp, sort_fp]

def plot_original_data(input_data):
    res = '\n\nThe following figures shows original input data.\n\n'

    avg_fp = os.path.join(cur_dir, 'fig_avg_original.tex')
    seq_fp = os.path.join(cur_dir, 'fig_seq_original.tex')
    sort_fp = os.path.join(cur_dir, 'fig_sort_original.tex')
    fps = plot_data(input_data, avg_fp, seq_fp, sort_fp)
    for fp in fps:
        res += '\\begin{tikzpicture}\n'
        res += '\\input{' + fp + '}\n'
        res += '\\end{tikzpicture}\n\n'
    return res

def plot_selected_data(input_data):
    input_data = input_data[10: ]
    res = '\n\nThe following figures shows the selected input data.\n\n'

    avg_fp = os.path.join(cur_dir, 'fig_avg_s.tex')
    seq_fp = os.path.join(cur_dir, 'fig_seq_s.tex')
    sort_fp = os.path.join(cur_dir, 'fig_sort_s.tex')
    fps = plot_data(input_data, avg_fp, seq_fp, sort_fp)
    for fp in fps:
        res += '\\begin{tikzpicture}\n'
        res += '\\input{' + fp + '}\n'
        res += '\\end{tikzpicture}\n\n'
    return res

def main():
    keys = []
    input_fp = os.path.join(cur_dir, sys.argv[1])
    input_data = json.loads(open(input_fp, 'r').read())

    tex_fp = os.path.join(cur_dir, 'main.tex.template')
    tgt_fp = os.path.join(cur_dir, 'main.tex')

    content = plot_original_data(input_data)
    #content += plot_selected_data(input_data)
    raw = open(tex_fp, 'r').read()
    raw = raw.replace('{{input_figures}}', content)
    open(tgt_fp, 'w').write(raw)






if __name__ == '__main__':
    '''
    The command should be 'python plot.py input_file key1 key2...'
    @input_file should be like [{key1: v1}, {key1:v2} ...]
    '''
    main()
