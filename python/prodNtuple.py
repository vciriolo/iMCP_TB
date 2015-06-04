#! /usr/bin/env python
import os
import glob
import math
from array import array
import sys
import time
import subprocess

currentDir = os.getcwd();
CMSSWDir = currentDir+"/../../";

name = ["AngScan_Double9040","HVScan12_Double9040_part1","HVScan12_Double9040_part2","HVScan12_Double9090","HVScan12_GaAsEm_part1","HVScan12_GaAsEm_part2",
        "HVScan12_MultiAlkEm_part1","HVScan12_MultiAlkEm_part2","HVScan12bis_GaAsEm","HVScan1_Double9040","HVScan1_Double9090","HVScan1_GaAsEm",
        "HVScan1_MultiAlkEm","HVScan2_Double9040","HVScan2_Double9090","HVScanExt_Double9040","HVScanExt_Double9090","LongScan0X0_Double9040",
        "LongScan0X0_Double9090","LongScan0X0_GaAsEm","LongScan0X0_MultiAlkEm","LongScan2X0_Double9040","LongScan2X0_Double9090","LongScan2X0_GaAsEm",
        "LongScan2X0_MultiAlkEm","X0Scan12_Double9040","X0Scan12_Double9090","X0Scan12_GaAsEm","X0Scan12_MultiAlkEm","X0Scan1_Double9040",
        "X0Scan1_Double9090","X0Scan1_MultiAlkEm","X0Scan1_GaAsEm"];

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
    os.system("qsub -V -d "+currentDir+" -q production "+currentDir+"/"+fn+".sh");
