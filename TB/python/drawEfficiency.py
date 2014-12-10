#!/usr/bin/python
import sys
import os
import commands
from commands import getstatusoutput
import datetime
import argparse
import string
if __name__ == '__main__':
    os.system('./drawEfficiency.exe EffAll')
    os.system('./drawEfficiency.exe scanX0_HVHigh_50')
    os.system('./drawEfficiency.exe scanX0_HVHigh_20')
    os.system('./drawEfficiency.exe scanX0_HVLow_50')
    os.system('./drawEfficiency.exe scanX0_HVLow_20')
    os.system('./drawEfficiency.exe AngScanLow')
    os.system('./drawEfficiency.exe AngScanHigh')

    
