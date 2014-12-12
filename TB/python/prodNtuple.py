#! /usr/bin/env python
import os
import glob
import math
from array import array
import sys
import time
import subprocess

currentDir = os.getcwd();
CMSSWDir = currentDir+"../../";

name = ["HVScan1","HVScan2","HVScan3","HVScan4","HVScan5","HVScan6","HVScan7","HVScan8","AngScan_HVHigh","AngScan_HVLow","pedestal","preshower_600GeV_HighV","scanX0_HVHigh20GeV","scanX0_HVHigh50GeV","scanX0_HVLow20GeV","scanX0_HVLow50GeV","H4test_X0scan_HVH_20GeV", "H4test_X0scan_HVH_50GeV", "H4test_X0scan_HVL_20GeV", "H4test_X0scan_HVL_50GeV"];

for i in range(len(name)):
    fn = "Job/Job_"+name[i];
    outScript = open(fn+".sh","w");
    command = "python python/dumper.py -cfg cfg/"+name[i]+".cfg -n "+name[i];

    outScript.write('#!/bin/bash');
    outScript.write("\n"+'cd '+CMSSWDir);
    outScript.write("\n"+'eval `scram runtime -sh`');
    outScript.write("\n"+'cd '+currentDir);
    outScript.write("\n"+"unbuffer "+command+" > "+fn+"_output.txt");
    outScript.close();

    os.system("chmod 777 "+currentDir+"/"+fn+".sh");
    os.system("qsub -V -d "+currentDir+" -q longcms "+currentDir+"/"+fn+".sh");
