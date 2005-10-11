INCLUDE_DIR = 

CXXFLAGS = -g
CFLAGS = -g

CC = cc  
C++ = g++

LIB = 

######################################################

PERMUTATOR_SRCS = \
    gen_perm.cxx

PERMUTATOR = gen_perm

######################################################

all : ${PERMUTATOR}

######################################################

$(PERMUTATOR): ${PERMUTATOR_SRCS} 
	${C++} ${CXXFLAGS} -o $(PERMUTATOR) \
	${PERMUTATOR_SRCS} 

######################################################

clobber:
	rm -f \
	$(PERMUTATOR) \
	core*

