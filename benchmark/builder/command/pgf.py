#author : Athrun Arthur

def make_standalone(fig):
    '''This is used to wrap a fig with tex headers
    '''
    header = '\\documentclass[border=3mm]{standalone}\n'
    header += '\\usepackage{pgfplots}\n'
    header += '\\pgfplotsset{compat=newest}\n'
    header += '\\pagestyle{empty}\n'
    header += '\\begin{document}\n'
    header += '\\begin{tikzpicture}\n'

    end = '\\end{tikzpicture}\n'
    end += '\\end{document}'

    return header + fig + end

class PlotSet:
    def __init__(self, name):
        self.ps_name = name

    def name(self):
        return self.ps_name

class PlotBase:
    def __init__(self):
        self.hs = '\\begin{axis}'
        self.plotset = None
        self.es = '\\end{axis}\n'
        self.legends = []
        self.plots = []
        self.styles = ''
        self.addplot_sufix = ''
        self.annotations = ''

    def config(self, ps):
        self.plotset = ps

    def append_style(self, style):
        self.styles += style.strip()

    def add_annotation(self, ann):
        self.annotations += ann + '\n'

    def plot_sufix(self, sufix):
        self.addplot_sufix = sufix

    def dump(self):
        rs = ''
        rs += self.hs

        has_ps_style = False
        if (not self.plotset is None) or len(self.styles) != 0:
            has_ps_style = True
        if has_ps_style:
            rs += '['

        if not self.plotset is None:
            rs += self.plotset.name()
            if len(self.styles) != 0:
                rs += ', '
        if len(self.styles) != 0:
            rs += self.styles
        if has_ps_style:
            rs += ']'
        rs += ' \n'

        for p in self.plots:
            rs += p

        if len(self.annotations) != 0:
            rs += self.annotations + '\n'

        if len(self.legends) != 0:
            rs += '\\legend{'
        for l in self.legends :
            rs += l + ','
        if len(self.legends) != 0:
            rs = rs.strip(',') + '}\n'

        rs += self.es
        return rs

class Plot(PlotBase):

    def __init__(self):
        PlotBase.__init__(self)

    def addplot(self, data, xfunc, yfunc, style='', legend=''):
        if len(data) == 0:
            return

        s = '\\addplot '
        if len(style) != 0:
            s += '[ ' + style + ' ]'
        s += ' plot coordinates {\n'
        for d in data:
            s += '(' + str(xfunc(d)) + ', ' + str(yfunc(d)) + ')\n'
        s += '};'

        self.plots.append(s)
        if len(legend) != 0:
            self.legends.append(legend)


class CDFPlot(PlotBase):
    def __init__(self):
        PlotBase.__init__(self)

    def addplot(self, data, style='', legend=''):
        '''@data - should be a data set'''

        if len(data) == 0:
            return

        nd = []
        delta = 100.0 / len(data)
        md = {}
        for d in data:
            if not md.has_key(d):
                md[d] = 0
            md[d] += 1

        nd.append((0, 0))
        last = 0.0
        for k, v in sorted(md.items(), key=lambda d:d[0]):
            last += v * delta
            nd.append((k, last))

        s = '\\addplot '
        if len(style) != 0:
            s += '[ ' + style + ' ]'
        s += ' plot coordinates {\n'
        for d in nd:
            s += '(' + str(d[0]) + ', ' + str(d[1]) + ')\n'
        s += '}'
        if len(self.addplot_sufix) == 0:
            s += self.addplot_sufix
        s+= ';'

        self.plots.append(s)
        if len(legend) != 0:
            self.legends.append(legend)

if __name__ == '__main__':
    p = Plot()

    import math
    p.addplot(range(0, 1000), lambda x: x, lambda x : round(math.sin(x*math.pi/180), 2))
    s = p.dump()
    s = make_standalone(s)
    import os
    if not os.path.exists('tt'):
        os.mkdir('tt')
    open('tt/xx.tex', 'w').write(s)

