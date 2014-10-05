iMCP_TB
=======

Code for iMCP analysis (test beam)

Main codes:

- dumper: 	
		read the raw data and produce a file with a reco tree. A cfg file should be given in input, with 5 columns:
		1)run 2)channel 3)HV 4)X0 5)NameMCP
		In the reco tree, branches are sorted following the MCP name list saved in interface/MCPMap.h.

--> usage:      ./dumper cfg/Scan2.cfg /gwteray/users/marzocchi/iMCP/dataTrees 9 Scan2
	
where Scan2.cfg is the cfg file, /gwteray/users/marzocchi/iMCP/dataTrees is the folder with the raw data, 9 is the number
of channels (be careful tu put the right number here!!!9, Scan2 is the suffix for the output file. 		

- calcTreshold:
		read the reco tree and calculate the charge tresholds (using S/B) for each channel with PC=ON and HV above a given treshold
		(to ensure the MCP has efficiency=1). Tresholds are saved in cfg/treshold.txt

--> usage: 	./calcTreshold ntuples/reco_Scan2.root 1000 9 2000

where ntuples/reco_Scan2.root is the reco tree, 1000 is the treshold for S/B, 9 is the number of channels, 2000 is the HV treshold.

- analyzer:
		read the reco tree and calculate efficiencies/time resolution, depending on the options given in input.

--> usage:      ./analyzer cfg/treshold.txt MiB3 all eff HVScan Scan2

where cfg/treshold.txt is the file with the charge tresholds, MiB3 is the MCP you want to analyze, "all" is the type of hodoscope cut (all/tight),
"eff" is the type of analysis (eff/time), HVScan is the type of scan (HVScan/X0Scan), Scan2 is the suffix of the reco file in input.