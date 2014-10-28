#!/usr/bin/python
import sys
import os
import commands
from commands import getstatusoutput
import datetime
import argparse
import string
if __name__ == '__main__':
    parser = argparse.ArgumentParser (description = 'draw waveform: read raw data and draw some waveforms')
    parser.add_argument ('-d', '--dir' , default = 'cfg/HVScan1.cfg', help='directory containing raw data')
    parser.add_argument ('-n', '--chNumber' , default = '9', help='number of channels (in the raw data!)')
    parser.add_argument ('-r', '--run' , default = '251', help='run to scan')
    parser.add_argument ('-c', '--channel' , default = '0', help='channel to analyze')
    parser.add_argument ('-f', '--firstEntry' , default = '0', help='first entry')
    args = parser.parse_args ()
    print './drawWaveform '+args.dir+' '+args.chNumber+' '+args.run+' '+args.channel+' '+args.firstEntry
    os.system('./drawWaveform '+args.dir+' '+args.chNumber+' '+args.run+' '+args.channel+' '+args.firstEntry)
    
