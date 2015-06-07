#!/usr/bin/python
import sys
import os
import commands
from commands import getstatusoutput
import datetime
import argparse
import string
if __name__ == '__main__':
    os.system('./drawResolutionCFD.exe HV12')
    os.system('./drawResolutionCFD.exe HV1')
    os.system('./drawResolutionCFD.exe HV2')
    os.system('./drawResolutionCFD.exe Extreme')
    os.system('./drawResolutionCFD.exe Ang')
    os.system('./drawResolutionCFD.exe X0_12')
    os.system('./drawResolutionCFD.exe X0_1')
#    os.system('./drawResolutionCFD.exe multiplicity1')
#    os.system('./drawResolutionCFD.exe multiplicity2')
    os.system('./drawResolutionCFD.exe LongScan2X0')
    os.system('./drawResolutionCFD.exe saturated_HV12')
    os.system('./drawResolutionCFD.exe saturated_X0_12')
    
