#BHEADER***********************************************************************
# (c) 1998   The Regents of the University of California
#
# See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
# notice, contact person, and disclaimer.
#
# $Revision$
#EHEADER***********************************************************************

SHELL = /bin/sh

srcdir = .

HYPRE_DIRS =\
 utilities\
 krylov\
 struct_mv\
 struct_ls\
 test

all:
	@ \
	for i in ${HYPRE_DIRS}; \
	do \
	  if [ -d $$i ]; \
	  then \
	    echo "Making $$i ..."; \
	    (cd $$i; make); \
	    echo ""; \
	  fi; \
	done

clean:
	@ \
	for i in ${HYPRE_DIRS}; \
	do \
	  if [ -d $$i ]; \
	  then \
	    echo "Cleaning $$i ..."; \
	    (cd $$i; make clean); \
	  fi; \
	done

veryclean:
	@ \
	for i in ${HYPRE_DIRS}; \
	do \
	  if [ -d $$i ]; \
	  then \
	    echo "Very-cleaning $$i ..."; \
	    (cd $$i; make veryclean); \
	  fi; \
	done

