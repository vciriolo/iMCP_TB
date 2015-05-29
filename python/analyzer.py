#!/usr/bin/python
import sys
import os
import commands
from commands import getstatusoutput
import datetime
import argparse
import string
if __name__ == '__main__':
    parser = argparse.ArgumentParser (description = 'do iMCP TB analysis')
    parser.add_argument ('-cfg', '--cfg' , default = 'cfg/treshold_5s.txt' , help='cfg file containing treshold values')
    parser.add_argument ('-m', '--MCP' , default = 'MiB3', help='MCP to analyze')
    parser.add_argument ('-a', '--analysis' , default = "eff", help='select analysis: eff->efficiency, Q->charge, time->CDF resolution, timeLED')
    parser.add_argument ('-s', '--scan' , default = "HV", help='type of scan: HV = scan in HV, X0 = scan in X0')
    parser.add_argument ('-n', '--name' , default = "prova", help='suffix of the reco file in input')
    parser.add_argument ('-i', '--inputDir', default='/gwteray/users/pigazzini/IMCP/BTF2015/ntuples/', help='input directory')
    args = parser.parse_args ()
    print 'analyzer '+args.cfg+' '+args.MCP+' '+args.analysis+' '+args.scan+' '+args.name+' '+args.inputDir
    os.system('./analyzer.exe '+args.cfg+' '+args.MCP+' '+args.analysis+' '+args.scan+' '+args.name+' '+args.inputDir)
    
