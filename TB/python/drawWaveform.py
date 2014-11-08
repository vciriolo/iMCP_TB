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
    parser.add_argument ('-d', '--dir' , default = '/data_secure/public_DQM_plots_secure', help='directory containing raw data')
    parser.add_argument ('-r', '--run' , default = '744', help='run to scan')
    parser.add_argument ('-c', '--channel' , default = '4', help='channel to analyze')
    parser.add_argument ('-f', '--firstEntry' , default = '0', help='first entry')
    parser.add_argument ('-n', '--nEvents' , default = '100', help='number of events to analyze')
    args = parser.parse_args ()
    print './drawWaveform '+args.dir+' '+args.run+' '+args.channel+' '+args.firstEntry+' '+args.nEvents
    os.system('./drawWaveform '+args.dir+' '+args.run+' '+args.channel+' '+args.firstEntry+' '+args.nEvents)
    
