################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

LCDEFS64 = -I$ROOT/include/dpcl -pg -g -gdwarf-2 -D__64BIT__
LLDLIBS64 = -L $ROOT/lib -ldpcl -lelf 
LCDEFS32 = -g -gdwarf-2
LLDLIBS32 =  -ldl

TARGETS= fred_calls_ted 

default all: fred_calls_ted
	@$(MAKE) $(TARGETS)

fred_calls_ted: fred_calls_ted.c foo.c fred.c ted.c
	echo ROOT=$(ROOT) ; \
    ls -ld $(ROOT)/ia64-unknown-linux-gnu ; \
	if test -d $(ROOT)/ia64-unknown-linux-gnu ; then \
		echo IA64; \
	    cc $(LCDEFS64) -o fred_calls_ted fred_calls_ted.c foo.c fred.c ted.c $(LLDLIBS64) ; \
	else \
		echo IA32; \
	    cc $(LCDEFS32) -o fred_calls_ted fred_calls_ted.c foo.c fred.c ted.c $(LLDLIBS32) ; \
	fi; \
	rm -fr $(OPENSPEEDSHOP_PLUGIN_PATH)/../../../usability;
	mkdir $(OPENSPEEDSHOP_PLUGIN_PATH)/../../../usability;
	mkdir $(OPENSPEEDSHOP_PLUGIN_PATH)/../../../usability/phaseI;
	cp *.c $(OPENSPEEDSHOP_PLUGIN_PATH)/../../../usability/phaseI;

clobber: clean
	-rm -f $(TARGETS)
	-rm -f fred_calls_ted

clean:
	@$(MAKE) $(TARGETS)
	-rm -f *.o
	-rm -f core*
	-rm -f *.a
	-rm -f *.o
	-rm -f core*
	-rm -f so_loc*
	-rm -fr test00*
	-rm -fr ss*
	-rm -fr *.openss
	-rm -fr nohup.out
	-rm -fr *html*
