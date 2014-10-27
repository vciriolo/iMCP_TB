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
    parser.add_argument ('-cfg', '--cfg' , default = 'cfg/treshold_SB1000.txt' , help='cfg file containing treshold values')
    parser.add_argument ('-m', '--MCP' , default = 'MiB3', help='MCP to analyze')
    parser.add_argument ('-o', '--hodoCut' , default = 'no', help='hodoscope selection: no->no hodo cut, all->hit everywhere, tight->hit in the central 4 fibres')
    parser.add_argument ('-a', '--analysis' , default = "eff", help='select analysis: eff->efficiency, Q->charge, time=time resolution')
    parser.add_argument ('-s', '--scan' , default = "HV", help='type of scan: HVScan=scan in HV, X0Scan = scan in X0')
    parser.add_argument ('-n', '--name' , default = "HVScan1", help='suffix of the reco file in input')
    args = parser.parse_args ()
    print 'analyzer '+args.cfg+' '+args.MCP+' '+args.hodoCut+' '+args.analysis+' '+args.scan+' '+args.name
    os.system('./analyzer '+args.cfg+' '+args.MCP+' '+args.hodoCut+' '+args.analysis+' '+args.scan+' '+args.name)
    
