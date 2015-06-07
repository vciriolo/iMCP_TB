#!/usr/bin/python
import sys
import os
import commands
from commands import getstatusoutput
import datetime
import argparse
import string
if __name__ == '__main__':
    os.system('./drawEfficiency.exe HV12')
    os.system('./drawEfficiency.exe HV1')
    os.system('./drawEfficiency.exe HV2')
    os.system('./drawEfficiency.exe Extreme')
    os.system('./drawEfficiency.exe Ang')
    os.system('./drawEfficiency.exe X0_12')
    os.system('./drawEfficiency.exe X0_1')
    os.system('./drawEfficiency.exe multiplicity1')
    os.system('./drawEfficiency.exe multiplicity2')
    os.system('./drawEfficiency.exe LongScan2X0')
    
