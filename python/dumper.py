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
    parser.add_argument ('-cfg', '--cfg' , default = 'cfg/prova.cfg' , help='list of run')
    parser.add_argument ('-d', '--dir' , default = '/gwteray/users/brianza/iMCP_H4', help='directory containing raw data')
    parser.add_argument ('-c', '--chNumber' , default = '18', help='number of channels (in the raw data!)')
    parser.add_argument ('-n', '--name' , default = 'prova', help='suffix of reco output file')
    parser.add_argument ('-tP', '--trigPos' , default = '4', help='position (channel) of the trigger in the raw files')
    args = parser.parse_args ()
    print 'dumper '+args.cfg+' '+args.dir+' '+args.chNumber+' '+args.name+' '+args.trigPos
    os.system('./dumper.exe '+args.cfg+' '+args.dir+' '+args.chNumber+' '+args.name+' '+args.trigPos)
    
