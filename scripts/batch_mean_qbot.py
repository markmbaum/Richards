import sys
from os import listdir
from os.path import join
from numpy import *
import pandas as pd
from scipy.integrate import trapz, simps
from multiprocessing import Pool

#-------------------------------------------------------------------------------
#INPUT

#batch directory with output files
batdir = sys.argv[1]

#output filename
fnout = sys.argv[2]

#number of processes/cpus
cpus = int(sys.argv[3])

#name of trials table
fntrials = 'trials.csv'

#-------------------------------------------------------------------------------
#FUNCTIONS

def isint(x):
    try:
        x = float(x)
    except(ValueError):
        return(False)
    else:
        if int(x) == x:
            return(True)
        return(False)

def mean_qbot(batdir, num):
    qbot = fromfile(join(batdir, str(num) + '_qbot'), dtype='float32')
    t = fromfile(join(batdir, str(num) + '_t'), dtype='float32')
    return( trapz(qbot, t)/(t.max() - t.min()) )

#-------------------------------------------------------------------------------
#MAIN

#get filenames with integer prefixes
fns = [fn for fn in listdir(batdir) if isint(fn.split('_')[0])]
#take only qbot files
fns = [fn for fn in fns if ('qbot' in fn)]
#sort by prefix
fns = sorted(fns, key=lambda fn: int(fn.split('_')[0]))
#read the trials table
df = pd.read_csv(join(batdir, fntrials), index_col=0).sort_index()

#start a pool
pool = Pool(cpus)
#make an argument list
args = zip([batdir]*len(fns), range(len(fns)))
#compute mean qbot values in parallel
mqbots = pool.starmap(mean_qbot, args)
#close the pool
pool.close()

#put values into the dataframe
df['qbot'] = mqbots
#save the dataframe
df.to_csv(fnout)
