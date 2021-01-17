#compiler
CXX=g++
#compiler CFLAGS
CFLAGS=-Wall -Wextra -pedantic -O3
#openmp flag
omp=-fopenmp
#path to top libode directory
odepath=../../../code/libode

#-------------------------------------------------------------------------------
#local directories

#source code directory
dirs=src
#built object directory
diro=obj
#built executable directory
dirb=bin
#CFLAGS to include libode
odesrc=-I$(odepath)/src
odelib=-L$(odepath)/bin -lode

#-------------------------------------------------------------------------------
#stuff to compile

#independent objects to compile
obj=$(diro)/io.o $(diro)/util.o $(diro)/settings.o

#model object
mod=$(diro)/grid.o $(diro)/richards.o

#default targets
all: libodemake \
	$(dirb)/richards.exe \
	$(dirb)/richards_periodic.exe \
	$(dirb)/richards_periodic_batch.exe

#-------------------------------------------------------------------------------
#compilation rules

libodemake:
	$(MAKE) -C $(odepath)

$(obj): $(diro)/%.o: $(dirs)/%.cc $(dirs)/%.h
	$(CXX) $(CFLAGS) -o $@ -c $< -I$(dirs)

$(diro)/grid.o: $(dirs)/grid.cc $(dirs)/grid.h $(diro)/io.o
	$(CXX) $(CFLAGS) -o $@ -c $< -I$(dirs)

$(diro)/richards.o: $(dirs)/richards.cc $(dirs)/richards.h $(dirs)/grid.h $(obj) $(diro)/grid.o $(libodemake)
	$(CXX) $(CFLAGS) -o $@ -c $< -I$(dirs) $(odesrc) $(odelib)

$(dirb)/richards.exe: $(dirs)/main.cc $(obj) $(mod)
	$(CXX) $(CFLAGS) -o $@ $< $(obj) $(mod) -I$(dirs) $(odesrc) $(odelib)

$(dirb)/richards_periodic.exe: $(dirs)/main_periodic.cc $(obj) $(mod)
	$(CXX) $(CFLAGS) -o $@ $< $(obj) $(mod) -I$(dirs) $(odesrc) $(odelib)

$(dirb)/richards_periodic_batch.exe: $(dirs)/main_periodic_batch.cc $(obj) $(mod)
	$(CXX) $(CFLAGS) $(omp) -o $@ $< $(obj) $(mod) -I$(dirs) $(odesrc) $(odelib)

.PHONY : clean
clean:
	rm obj/* bin/*
