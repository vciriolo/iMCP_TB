#!/usr/bin/python
import sys
import os
import commands
from commands import getstatusoutput
import datetime
import argparse
import string
if __name__ == '__main__':
    os.system('./drawResolutionLED.exe EffAll')
    os.system('./drawResolutionLED.exe scanX0_HVHigh_50')
    os.system('./drawResolutionLED.exe scanX0_HVHigh_20')
    os.system('./drawResolutionLED.exe scanX0_HVLow_50')
    os.system('./drawResolutionLED.exe scanX0_HVLow_20')
    os.system('./drawResolutionLED.exe AngScanLow')
    os.system('./drawResolutionLED.exe AngScanHigh')

    
