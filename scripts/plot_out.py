from os import listdir
from os.path import join, basename
from numpy import *
import matplotlib.pyplot as plt

#plt.rc('font', family='serif')
#plt.rc('text', usetex=True)

#-------------------------------------------------------------------------------
#INPUT

#directory with results in it
resdir = join('..', 'out')

#snapped variables to plot
snapvars = ['w', 'q', 'K', 'dpsidw']

#labels of snapped vars
snaplabs = [r'$\theta$', '$q$', '$K$', r'$\partial\psi/\partial\theta$']

#whether to plot grid variables
grid = 0

#-------------------------------------------------------------------------------
#FUNCTIONS

def isint(x):
    try:
        int(x)
    except (TypeError, ValueError):
        return(False)
    else:
        return(True)

readvar = lambda resdir, fn, dtype='float64': fromfile(join(resdir, fn), dtype=dtype)

def readsnaps(resdir, varname):
    #get potential file names
    fns = listdir(resdir)
    #filter for the ones with the variable name
    fns = [fn for fn in fns if (('_' + varname + '_' in fn) and isint(fn[-1]))]
    #sort by the snap number
    fns = sorted(fns, key=lambda fn: int(fn.split('_')[-1]))
    #read the files
    x = [readvar(resdir, fn) for fn in fns]
    #put into a 2D array
    x = stack(x).T

    return(x)

#-------------------------------------------------------------------------------
#MAIN



#plot results
assert len(snapvars) == len(snaplabs), 'different number of snap variables and labels'
n = len(snapvars)
if grid:
    fig, axs = plt.subplots(1,n+1)
else:
    fig, axs = plt.subplots(1,n)
ze = readvar(resdir, 'ze')
zc = readvar(resdir, 'zc')
tsnap = readvar(resdir, 'richards_tsnap', dtype='float32')
S = {v:readsnaps(resdir, v) for v in snapvars}
L = dict(zip(snapvars, snaplabs))
for i,v in enumerate(S):
    s = S[v]
    if s.shape[0] == len(zc):
        z = zc
    elif s.shape[0] == len(ze):
        z = ze
    else:
        raise(ValueError)
    for j in range(s.shape[1]):
        axs[i].plot(s[:,j], -z, label='%g hr' % (tsnap[j]/3600))
    axs[i].invert_yaxis()
    axs[i].set_xlabel(L[v])
axs[0].set_ylabel('Depth (m)')
axs[-1].legend()

#plot the grid spacing if desired
if grid:
    ze = readvar(resdir, 'ze')
    axs[-1].plot(diff(ze), -ze[:-1])
    axs[-1].invert_yaxis()
    axs[-1].set_xlabel('Cell Depth')

fig.tight_layout()
plt.show()
