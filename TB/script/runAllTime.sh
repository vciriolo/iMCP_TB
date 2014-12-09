python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack1 -a time -n HVScan1 -s HV
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack2 -a time -n HVScan2 -s HV
python python/analyzer.py -cfg cfg/treshold_5s.txt -m MiB3 -a time -n HVScan2 -s HV
python python/analyzer.py -cfg cfg/treshold_5s.txt -m enSEE -a time -n HVScan5 -s HV
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack2 -a time -n HVScan6 -s HV
python python/analyzer.py -cfg cfg/treshold_5s.txt -m MiB3 -a time -n HVScan7 -s HV
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack1 -a time -n HVScan8 -s HV

python python/analyzer.py -cfg cfg/treshold_5s.txt -m MiB2 -a time -n scanX0_HVHigh50GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m enSEE -a time -n scanX0_HVHigh50GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack1 -a time -n scanX0_HVHigh50GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack2 -a time -n scanX0_HVHigh50GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m MiB3 -a time -n scanX0_HVHigh50GeV -s X0

python python/analyzer.py -cfg cfg/treshold_5s.txt -m MiB2 -a time -n scanX0_HVHigh20GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m enSEE -a time -n scanX0_HVHigh20GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack1 -a time -n scanX0_HVHigh20GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack2 -a time -n scanX0_HVHigh20GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m MiB3 -a time -n scanX0_HVHigh20GeV -s X0

python python/analyzer.py -cfg cfg/treshold_5s.txt -m MiB2 -a time -n scanX0_HVLow20GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m enSEE -a time -n scanX0_HVLow20GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack1 -a time -n scanX0_HVLow20GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack2 -a time -n scanX0_HVLow20GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m MiB3 -a time -n scanX0_HVLow20GeV -s X0

python python/analyzer.py -cfg cfg/treshold_5s.txt -m MiB2 -a time -n scanX0_HVLow50GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m enSEE -a time -n scanX0_HVLow50GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack1 -a time -n scanX0_HVLow50GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack2 -a time -n scanX0_HVLow50GeV -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m MiB3 -a time -n scanX0_HVLow50GeV -s X0

python python/analyzer.py -cfg cfg/treshold_5s.txt -m enSEE -a time -n AngScan_HVLow -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack1 -a time -n AngScan_HVLow -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack2 -a time -n AngScan_HVLow -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m MiB3 -a time -n AngScan_HVLow -s X0

python python/analyzer.py -cfg cfg/treshold_5s.txt -m enSEE -a time -n AngScan_HVHigh -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack1 -a time -n AngScan_HVHigh -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m ZStack2 -a time -n AngScan_HVHigh -s X0
python python/analyzer.py -cfg cfg/treshold_5s.txt -m MiB3 -a time -n AngScan_HVHigh -s X0
