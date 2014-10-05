iMCP_TB
=======

Code for iMCP analysis (test beam)

instruction for download and compiling:

	cmsrel CMSSW_6_1_1 (but you can use different releases)
	cd CMSSW_6_1_1/src
	git clone https://github.com/lbrianza/iMCP_TB
	scramv1 b -j16
	cmsenv
	cd iMCP_TB/TB/

..and start to use it!

------------------------------------------------------
Main codes:

1- dumper: 	
		read the raw data and produce a file with a reco tree. A cfg file should be given in input, with 5 columns:
		1)run 2)channel 3)HV 4)X0 5)NameMCP
		In the reco tree, branches are sorted following the MCP name list saved in interface/MCPMap.h.

--> usage:      python python/dumper.py

(for the list of options, look inside the python)	

2- calcTreshold:
		read the reco tree and calculate the charge tresholds (using S/B) for each channel with PC=ON and HV above a given treshold
		(to ensure the MCP has efficiency=1). Tresholds are saved in cfg/treshold.txt

--> usage: 	python python/calcTreshold.py


3- analyzer:
		read the reco tree and calculate efficiencies/time resolution, depending on the options given in input.

--> usage:      python python/analyzer.py

