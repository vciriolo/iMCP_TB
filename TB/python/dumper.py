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
    parser.add_argument ('-cfg', '--cfg' , default = 'cfg/H4test3.cfg' , help='list of run')
    parser.add_argument ('-d', '--dir' , default = '/data/public_DQM_plots', help='directory containing raw data')
    parser.add_argument ('-c', '--chNumber' , default = '10', help='number of channels (in the raw data!)')
    parser.add_argument ('-n', '--name' , default = 'H4test3', help='suffix of reco output file')
    args = parser.parse_args ()
    print 'dumper '+args.cfg+' '+args.dir+' '+args.chNumber+' '+args.name
    os.system('./dumper '+args.cfg+' '+args.dir+' '+args.chNumber+' '+args.name)
    
