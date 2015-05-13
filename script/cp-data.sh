#!/bin/bash

mkdir ${1}
scp -r cmsdaq@192.168.189.82:/data/IMCP/DQM/${1}/*root ${1}
