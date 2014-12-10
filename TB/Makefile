CXXFLAGS=-Wall # -pedantic -ansi #-std=c++0x
ROOT_LIB:=`root-config --libs --glibs`
ROOT_FLAGS:=`root-config --cflags --ldflags`
ROOT_INCLUDE:=`root-config --incdir`

DEPS= interface/MCPMap.h interface/init_Reco_Tree.h interface/init_tree_H4.h interface/analysis_tools.h interface/histoFuncT.h interface/ScaleEstimators.h
DEPS_OBJ= lib/MCPMap.o lib/init_Reco_Tree.o lib/init_tree_H4.o lib/analysis_tools.o lib/histoFuncT.o lib/ScaleEstimators.o

CC = g++
CFLAGS = -Wall

lib/%.o: src/%.cc $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< $(ROOT_LIB) $(ROOT_FLAGS)

all: dumper.exe analyzer.exe calcTreshold.exe preshower.exe drawWaveform.exe dumper_fitTemplate.exe calcTreshold_fitTemplate.exe drawEfficiency.exe drawResolutionCFD.exe drawResolutionLED.exe

dumper.exe: bin/dumper.cpp $(DEPS_OBJ)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LIB) $(ROOT_FLAGS) 

analyzer.exe: bin/analyzer.cpp $(DEPS_OBJ)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LIB) $(ROOT_FLAGS) 

calcTreshold.exe: bin/calcTreshold.cpp $(DEPS_OBJ)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LIB) $(ROOT_FLAGS) 

calcTreshold_fitTemplate.exe: bin/calcTreshold_fitTemplate.cpp $(DEPS_OBJ)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LIB) $(ROOT_FLAGS) 

drawWaveform.exe: bin/drawWaveform.cpp $(DEPS_OBJ)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LIB) $(ROOT_FLAGS) 

dumper_fitTemplate.exe: bin/dumper_fitTemplate.cpp $(DEPS_OBJ)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LIB) $(ROOT_FLAGS) 

preshower.exe: bin/preshower.cpp $(DEPS_OBJ)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LIB) $(ROOT_FLAGS) 

drawEfficiency.exe: bin/drawEfficiency.cpp $(DEPS_OBJ)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LIB) $(ROOT_FLAGS) 

drawResolutionCFD.exe: bin/drawResolutionCFD.cpp $(DEPS_OBJ)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LIB) $(ROOT_FLAGS) 

drawResolutionLED.exe: bin/drawResolutionLED.cpp $(DEPS_OBJ)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LIB) $(ROOT_FLAGS) 

clean:
	rm -f lib/*.o
	rm -f lib/*.d
	rm -f *.exe
