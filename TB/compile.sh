c++ -o calcTreshold bin/calcTreshold.cpp `root-config --cflags --glibs`;
c++ -o analyzer bin/analyzer.cpp `root-config --cflags --glibs`;
c++ -o dumper bin/dumper.cpp `root-config --cflags --glibs`;
c++ -o dumper_fitTemplate bin/dumper_fitTemplate.cpp `root-config --cflags --glibs`;
c++ -o drawWaveform bin/drawWaveform.cpp `root-config --cflags --glibs`;
c++ -o preshower bin/preshower.cpp `root-config --cflags --glibs`

