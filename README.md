iMCP_TB
=======

Code for iMCP analysis (test beam)

instruction for download and compiling:

	git clone git@github.com:lbrianza/iMCP_TB.git
	cd iMCP_TB/TB/
	source thisroot.sh
	sh compile.sh

..and start to use it!

------------------------------------------------------
Main codes:

1- dumper: 	
		read the raw data and produce a file with a reco tree. A cfg file should be given in input, with:
		- a first raw with the names of the two MCP used as trigger (the 1st one sould be the one used for the time measurement)
		- many rows with 5 columns:
		1)run 2)channel 3)HV 4)X0 5)NameMCP
		there must be exactly n rows for each run (where n=number of channel in input from the digitizer). Look into cfg/ folder for example.
		In the reco tree, branches are sorted following the MCP name list saved in interface/MCPMap.h.

--> usage:      python python/dumper.py
--> example:    python python/analyzer.py -m MiB3 -n HVScan4 -a eff -s HV -cfg cfg/HVScan4.cfg 
(for the list of options, look inside the python)	

2- calcTreshold:
		read the reco tree and calculate the charge tresholds (using S/B) for each channel with PC=ON and HV above a given treshold
		(to ensure the MCP has efficiency=1). Tresholds are saved in cfg/treshold.txt

--> usage: 	python python/calcTreshold.py
--> example:    python python/calcTreshold.py -HV 2700 -i ntuples/reco_HVScan4.root -SB 1000

3- analyzer:
		read the reco tree and calculate efficiencies/time resolution, depending on the options given in input.

--> usage:      python python/analyzer.py
--> example:    python python/analyzer.py -m MiB3 -n HVScan4 -a eff -s HV


4- drawWaveform:
		read raw data and draw 10 waveforms.

--> usage:
		python python/drawWaveform.py
