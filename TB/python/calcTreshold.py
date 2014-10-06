#!/usr/bin/python
import sys
import os
import commands
from commands import getstatusoutput
import datetime
import argparse
import string
if __name__ == '__main__':
    parser = argparse.ArgumentParser (description = 'calcTreshold: calculate charge tresholds for each channel')
    parser.add_argument ('-i', '--inputFile' , default = 'ntuples/reco_Scan2.root' , help='input file')
    parser.add_argument ('-tVal', '--tresholdValue' , default = '1000', help='value to use in calculating S/B treshold')
    parser.add_argument ('-c', '--chNumber' , default = '9', help='number of channels (in the raw data!)')
    parser.add_argument ('-HV', '--HV' , default = '2000', help='analyze only channels with HV>treshold')
    parser.add_argument ('-p', '--doPlots' , default = '0', help='do plot of charge for S and B')
    args = parser.parse_args ()
    print 'calcTreshold '+args.inputFile+' '+args.tresholdValue+' '+args.chNumber+' '+args.HV+' ' +args.doPlots
    os.system('./calcTreshold '+args.inputFile+' '+args.tresholdValue+' '+args.chNumber+' '+args.HV+' '+args.doPlots)
    
