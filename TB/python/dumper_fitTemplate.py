#!/usr/bin/python
import sys
import os
import commands
from commands import getstatusoutput
import datetime
import argparse
import string
if __name__ == '__main__':
    parser = argparse.ArgumentParser (description = 'dumper: create reco tree from raw data')
    parser.add_argument ('-cfg', '--cfg' , default = 'cfg/HVScan1.cfg' , help='list of run')
    parser.add_argument ('-d', '--dir' , default = '/data_secure/public_DQM_plots_secure', help='directory containing raw data')
    parser.add_argument ('-c', '--chNumber' , default = '10', help='number of channels (in the raw data!)')
    parser.add_argument ('-n', '--name' , default = 'HVScan1', help='suffix of reco output file')
    parser.add_argument ('-f', '--promedioFile' , default = 'ntuples/promedio.root', help='suffix of reco output file')
    args = parser.parse_args ()
    print 'dumper_fitTemplate '+args.cfg+' '+args.dir+' '+args.chNumber+' '+args.name+' '+args.promedioFile
    os.system('./dumper_fitTemplate '+args.cfg+' '+args.dir+' '+args.chNumber+' '+args.name+' '+args.promedioFile)
    
