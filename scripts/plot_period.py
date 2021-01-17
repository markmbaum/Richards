from os.path import join
from numpy import *
from scipy.integrate import simps, trapz
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection

#plt.style.use('dark_background')
#plt.rc('font', family='serif')
#plt.rc('text', usetex=True)

#-------------------------------------------------------------------------------
#INPUT

#directory with results in it
resdir = join('..', 'out')

#whether to plot qall
qall = True

#whether to plot wall
wall = True

#number of bottom levels to plot cumulative flux
meanq = -1

#-------------------------------------------------------------------------------
#FUNCTIONS

readvar = lambda fn, dtype='float32': fromfile(join(resdir, fn), dtype=dtype)

#-------------------------------------------------------------------------------
#FUNCTIONS

def flux_lines(ax, x, y, negcol='C0', poscol='C3', format=True):

    segs = []
    colors = []
    for i in range(len(x)-1):
        if y[i]*y[i+1] < 0:
            m = (y[i+1] - y[i])/(x[i+1] - x[i])
            b = y[i] - m*x[i]
            xint = -b/m
            segs.append([(x[i],y[i]),(xint,0.0)])
            segs.append([(xint, 0.0),(x[i+1],y[i+1])])
            if y[i] > 0:
                colors += [poscol, negcol]
            else:
                colors += [negcol, poscol]
        else:
            segs.append([(x[i],y[i]),(x[i+1],y[i+1])])
            if y[i] > 0:
                colors.append(poscol)
            else:
                colors.append(negcol)
    lc = LineCollection(segs, colors=colors)
    ax.add_collection(lc)
    if format:
        ax.set_xlim(min(x), max(x))
        yr = (max(y) - min(y))
        ax.set_ylim(min(y) - 0.03*yr, max(y) + 0.03*yr)

#-------------------------------------------------------------------------------
#MAIN

ze = readvar('ze', 'float64')
zc = readvar('zc', 'float64')
t = readvar('richards_t')
th = (t - t.min())/3600
q = {k:readvar('richards_q' + k) for k in ['bot', 'mid', 'top']}

fig, axs = plt.subplots(3,1)
#top
flux_lines(axs[0], th, q['top'])
axs[0].set_ylabel('Top Flux (m/s)')
#mid
flux_lines(axs[1], th, q['mid'])
axs[1].set_ylabel('Middle Flux (m/s)')
#bot
flux_lines(axs[2], th, q['bot'])
axs[2].set_ylabel('Bottom Flux (m/s)')
axs[2].set_ylabel('Bottom Flux (m/s)')
axs[2].set_xlabel('Time (hr)')
#fig.text(0.02, 0.98, 'C', fontsize=18, fontweight='extra bold', ha='left', va='top')
fig.tight_layout()

if wall:
    try:
        w = readvar('richards_wall')
    except FileNotFoundError:
        print("wall file not found, can't plot wall")
    else:
        fig, ax = plt.subplots(1,1)
        w = w.reshape(len(zc), len(t))
        r = ax.contourf(th, -zc, w, cmap='Blues')
        cb = plt.colorbar(r, ax=ax)
        ax.set_xlabel('Time (hr)')
        ax.set_ylabel('Depth (m)')
        cb.set_label(r'Water Fraction $\theta$')
        ax.invert_yaxis()
        #fig.text(0.02, 0.98, 'A', fontsize=18, fontweight='extra bold', ha='left', va='top')
        fig.tight_layout()

if qall:
    try:
        q = readvar('richards_qall')
    except FileNotFoundError:
        print("qall file not found, can't plot qall")
    else:
        fig, ax = plt.subplots(1,1)
        q = q.reshape(len(ze), len(t))
        for i in range(q.shape[0]):
            q[i,:] /= abs(q[i,:]).max()
        r = ax.contourf(th, -ze, q, cmap='coolwarm', vmin=-1, vmax=1)
        cb = plt.colorbar(r, ax=ax)
        ax.set_xlabel('Time (hr)')
        ax.set_ylabel('Depth (m)')
        cb.set_label('Normalized Water Flux')
        ax.invert_yaxis()
        #fig.text(0.02, 0.98, 'B', fontsize=18, fontweight='extra bold', ha='left', va='top')
        fig.tight_layout()

if meanq:
    try:
        q = readvar('richards_qall')
    except FileNotFoundError:
        print("qall file not found, can't plot mean fluxes")
    else:
        fig, ax = plt.subplots(1,1)
        q = q.reshape(len(ze), len(t))
        mqs = array([simps(i,t) for i in q[:meanq]])/(t.max() - t.min())
        mqt = array([trapz(i,t) for i in q[:meanq]])/(t.max() - t.min())
        ax.plot(mqs, -ze[:meanq], label='simps')
        ax.plot(mqt, -ze[:meanq], label='trapz')
        xl = ax.get_xlim()
        ax.legend()
        ax.set_xlabel('Mean Flux (m/s)')
        ax.set_ylabel('Depth (m)')
        ax.invert_yaxis()
        fig.tight_layout()

plt.show()
