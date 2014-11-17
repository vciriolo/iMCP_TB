#! /usr/bin/env python
import os
import glob
import math
from array import array
import sys
import time
import subprocess

currentDir = os.getcwd();
CMSSWDir = "/gwpool/users/brianza/PHD/CMSSW_6_1_1/src/";

name = ["HVScan1","HVScan2","HVScan3","HVScan4","HVScan5","HVScan6","HVScan7","HVScan8"];

for i in range(len(name)):
    fn = "Job_"+name[i];
    outScript = open(fn+".sh","w");
    command = "python python/dumper.py -cfg cfg/treshold_5s.txt -n "+name[i];

    outScript.write('#!/bin/bash');
    outScript.write("\n"+'cd '+CMSSWDir);
    outScript.write("\n"+'eval `scram runtime -sh`');
    outScript.write("\n"+'cd '+currentDir);
    outScript.write("\n"+"unbuffer "+command+" > /gwteray/users/brianza/output_iMCP/"+fn+".txt");
    outScript.close();

    os.system("chmod 777 "+currentDir+"/"+fn+".sh");
    os.system("qsub -V -d "+currentDir+" -q longcms "+currentDir+"/"+fn+".sh");
