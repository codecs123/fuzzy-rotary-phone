SYSTEM     = x86-64_sles10_4.1
LIBFORMAT  = static_pic

# ---------------------------------------------------------------------         
# Project Group
# Ben Larson, Nick Weber, Shivam Nanda                                                            
# ---------------------------------------------------------------------         


# ---------------------------------------------------------------------         
# Compiler selection                                                            
# ---------------------------------------------------------------------         

CCC = g++

# ---------------------------------------------------------------------         
# Compiler options                                                              
# ---------------------------------------------------------------------         

CCOPT = -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -g -Wall

# ---------------------------------------------------------------------         
# Link options and libraries                                                    
# ---------------------------------------------------------------------         

CCFLAGS = $(CCOPT) 
CCLNFLAGS = -lm -pthread 

#------------------------------------------------------------                   
#  make all      : to compile.                                     
#  make execute  : to compile and execute.                         
#------------------------------------------------------------    

ROUTE.exe: main.o ece556.o calculateWeights.o orderNets.o netDec.o aStar.o
	/bin/rm -f ROUTE.exe
	$(CCC) $(LINKFLAGS) $(CCFLAGS) main.o ece556.o calculateWeights.o orderNets.o netDec.o aStar.o  $(CCLNFLAGS) -o ROUTE.exe

main.o: main.cpp ece556.h
	/bin/rm -f main.o
	$(CCC) $(CCFLAGS) main.cpp -c

ece556.o: ece556.cpp ece556.h
	/bin/rm -f ece556.o
	$(CCC) $(CCFLAGS) ece556.cpp -c

calculateWeights.o: calculateWeights.cpp calculateWeights.h ece556.h
	/bin/rm -f calculateWeights.o
	$(CCC) $(CCFLAGS) calculateWeights.cpp -c

orderNets.o: orderNets.cpp orderNets.h ece556.h
	/bin/rm -f orderNets.o
	$(CCC) $(CCFLAGS) orderNets.cpp -c

netDec.o: netDec.cpp netDec.h ece556.h
	/bin/rm -f netDec.o
	$(CCC) $(CCFLAGS) netDec.cpp -c

aStar.o: aStar.cpp aStar.h ece556.h
	/bin/rm -f aStar.o
	$(CCC) $(CCFLAGS) aStar.cpp -c

clean:
	/bin/rm -f *~ *.o ROUTE.exe 
