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
    parser.add_argument ('-d', '--dir' , default = '/gwteray/users/pigazzini/IMCP/BTF2015', help='directory containing raw data')
    parser.add_argument ('-r', '--run' , default = '1834', help='run to scan')
    parser.add_argument ('-c', '--channel' , default = '1', help='channel to analyze')
    parser.add_argument ('-f', '--firstEntry' , default = '11', help='first entry')
    parser.add_argument ('-n', '--nEvents' , default = '1', help='number of events to analyze')
    parser.add_argument ('-tP', '--trigPos' , default = '4', help='position (channel) of the trigger in the raw files')
    args = parser.parse_args ()
    print './drawWaveform '+args.dir+' '+args.run+' '+args.channel+' '+args.firstEntry+' '+args.nEvents+' '+args.trigPos
    os.system('./drawWaveform.exe '+args.dir+' '+args.run+' '+args.channel+' '+args.firstEntry+' '+args.nEvents+' '+args.trigPos)
    
