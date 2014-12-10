#!/usr/bin/python
import sys
import os
import commands
from commands import getstatusoutput
import datetime
import argparse
import string
if __name__ == '__main__':
    os.system('./drawResolutionCFD.exe EffAll')
    os.system('./drawResolutionCFD.exe scanX0_HVHigh_50')
    os.system('./drawResolutionCFD.exe scanX0_HVHigh_20')
    os.system('./drawResolutionCFD.exe scanX0_HVLow_50')
    os.system('./drawResolutionCFD.exe scanX0_HVLow_20')
    os.system('./drawResolutionCFD.exe AngScanLow')
    os.system('./drawResolutionCFD.exe AngScanHigh')

    
