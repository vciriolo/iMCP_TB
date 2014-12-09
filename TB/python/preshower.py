#!/usr/bin/python
import sys
import os
import commands
from commands import getstatusoutput
import datetime
import argparse
import string
if __name__ == '__main__':
    parser = argparse.ArgumentParser (description = 'do iMCP TB preshower')
    parser.add_argument ('-cfg', '--cfg' , default = 'cfg/treshold_SB1000.txt' , help='cfg file containing treshold values')
    parser.add_argument ('-m1', '--MCP1' , default = 'enSEE', help='MCP after 1X0')
    parser.add_argument ('-m3', '--MCP3' , default = 'ZStack2', help='MCP after 3X0')
    parser.add_argument ('-n1', '--name1' , default = "preshower", help='suffix of the 1st reco file in input- with preshower')
    parser.add_argument ('-n2', '--name2' , default = "CeF3", help='suffix of the 2nd reco file in input- CeF3 only')
    args = parser.parse_args ()
    print 'preshower '+args.cfg+' '+args.MCP1+' '+args.MCP3+' '+args.name1+' '+args.name2
    os.system('./preshower.exe '+args.cfg+' '+args.MCP1+' '+args.MCP3+' '+args.name1+' '+args.name2)
    
