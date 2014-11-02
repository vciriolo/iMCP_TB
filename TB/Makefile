CXXFLAGS=-Wall # -pedantic -ansi #-std=c++0x
ROOT_LIB:=`root-config --libs --glibs`
ROOT_FLAGS:=`root-config --cflags --ldflags`
ROOT_INCLUDE:=`root-config --incdir`

# Boost
BOOST = /afs/cern.ch/cms/slc5_amd64_gcc434/external/boost/1.47.0
#BOOST=/afs/cern.ch/cms/slc5_amd64_gcc472/external/boost/1.50.0

ROOSTAT_LIB="-lRooStats"
INCLUDEDIR=./interface
SRCDIR=./src
BUILDDIR=./bin
OBJ_DIR=./lib

#EoPNtupleDir:=../NtuplePackage

#################
INCLUDE=-I$(INCLUDEDIR) -isystem$(ROOT_INCLUDE) -I$(ROOFIT_INCLUDE) -I$(BOOST)/include
LIB=-L$(BOOST)/lib -L/usr/lib64 # -L/usr/lib

#### Make the list of modules from the list of .cc files in the SRC directory
MODULES=$(shell ls $(SRCDIR)/*.cc | sed "s|.cc|.o|;s|$(SRCDIR)|$(OBJ_DIR)|g")

#### Make the list of dependencies for a particular module
default: $(MODULES) dumper.exe

#------------------------------ MODULES (static libraries)
MAKEDEPEND = -MMD -MT '$@ lib/$*.d'
# $<: first prerequisite -> put always the .cc as first
#### General rule for classes (libraries) compiled statically
### Generate also a .d file with prerequisites
lib/%.o: $(SRCDIR)/%.cc
	@echo "--> Making $@"
	@$(COMPILE.cc) $(CXXFLAGS) $(INCLUDE) $(MAKEDEPEND) -o $@ $<

-include $(MODULES:.o=.d)

###### Main program
dumper.exe: $(BUILDDIR)/dumper.exe
$(BUILDDIR)/dumper.exe: $(BUILDDIR)/dumper.cpp
	@echo "---> Making iMCP_TB $(COMPILE.exe)"
	@g++ $(CXXFLAGS) $(INCLUDE) $(MAKEDEPEND) -o $@ $< $(MODULES) $(LIB) $(ROOT_LIB) $(ROOSTAT_LIB) $(ROOT_FLAGS) \
-lboost_program_options -lTreePlayer

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.d
	rm -f $(BUILDDIR)/*.exe