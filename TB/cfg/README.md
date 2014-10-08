####### instruction to create the cfg file ###########

The .cfg file given in input to read raw data must have:

- a first row with the name of the two MCPs used as trigger: the 1st one SHOULD BE the MCP used for the time measurement

- then, many rows with the following sintax:
	247 0 2500 0.1 1 MiB1
where 247=run number, 0=channel number, 2500=HV, 0.1=X0, 1=PC ON (0=OFF), MiB1=MCP name (should match the names saved in src/MCPMap.cc)

for each row, there must be exactly n rows, where n=number of channel in input (from the digitizer). Then, n rows for the next run, etc..

for example, look at Scan2.cfg. 