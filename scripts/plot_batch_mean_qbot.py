from os import listdir
from os.path import join, isfile
from numpy import *
from pandas import read_csv, concat
import matplotlib.pyplot as plt
import pickle

plt.rc('font', family='serif')
plt.rc('text', usetex=True)

#-------------------------------------------------------------------------------
#INPUT

#directory with multiple batch csvs in it (with a qbot column)
dirbat = join('..', 'batch')

#pickel filename
fnpickle = join(dirbat, 'batch.pickle')

#seconds in an hour
hrsec = 3600

#seconds in a day
daysec = hrsec*24

#seconds in a year
yrsec = daysec*365.25

#two columns to plot against each other
pcols = ['infdur', 'infper']
pscal = ['linear', 'linear']
plabs = ['Wetting Cycle Duration (hr)', 'Wetting Event Period (days)']

#selection of specific params
filters = dict(
    wilt=0.1,
    tauevap=3600,
    dep=3
)

#-------------------------------------------------------------------------------
#MAIN

if not isfile(fnpickle):
    fns = [fn for fn in listdir(dirbat) if '.csv' in fn]
    df = {fn.replace('.csv',''):read_csv(join(dirbat, fn), index_col=0) for fn in fns}
    for k in df:
        df[k]['dep'] = float(k)
    df = concat([df[k] for k in df], ignore_index=True)
    with open(fnpickle, 'wb') as ofile:
        pickle.dump(df, ofile)
else:
    with open(fnpickle, 'rb') as ifile:
        df = pickle.load(ifile)

#filter parameters
for p in filters:
    v = filters[p]
    df = df[(df[p] > 0.99*v) & (df[p] < 1.01*v)]
df = df[df['infper'] > 20*3600*24]

#convert units
df['infdur'] /= hrsec
df['infper'] /= daysec
df['tauevap'] /= hrsec
df['qbot'] *= yrsec

"""g = df.groupby(['perm', 'wilt', 'tauevap', 'infper']).groups
for k in g:
    sl = df.loc[g[k]]
    print(sl)
    fig, ax = plt.subplots(1,1)
    for infdur in sorted(sl['infdur'].unique()):
        sm = sl[sl['infdur'] == infdur].sort_values('dep')
        #sm = sm[sm['dep'] > 2]
        ax.plot(sm['dep'], sm['qbot'], label='%g hour wetting' % infdur)
    ax.legend()
    plt.show()"""

cols = list(df.columns)
cols.remove('qbot')
for col in pcols:
    cols.remove(col)

g = df.groupby(cols).groups
for k in g:
    sl = df.loc[g[k]].sort_values(pcols)
    nx = len(sl[pcols[0]].unique())
    ny = len(sl[pcols[1]].unique())
    X = sl[pcols[0]].values.reshape(nx,ny).T
    Y = sl[pcols[1]].values.reshape(nx,ny).T
    Z = sl['qbot'].values.reshape(nx,ny).T
    v = abs(Z).max()

    print(dict(zip(cols,k)))
    fig, ax = plt.subplots(1,1)
    r = ax.contourf(X, Y, Z, cmap='coolwarm', vmin=-v, vmax=v)
    cb = plt.colorbar(r, ax=ax)
    cb.set_label('Water Table Flux (m/yr)', rotation=270, va='bottom')
    ax.contour(X, Y, Z, levels=[0], colors='k', linewidths=1)
    ax.set_xlabel(plabs[0])
    ax.set_ylabel(plabs[1])
    ax.set_xscale(pscal[0])
    ax.set_yscale(pscal[1])
    ax.set_title('Permeability = 10$^{%g}$ m$^2$' % log10(sl['perm'].iat[0]))

plt.show()
