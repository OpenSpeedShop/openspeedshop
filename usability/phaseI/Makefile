LCDEFS= -g
LLDLIBS=

TARGETS= fred_calls_ted 

default all: fred_calls_ted
	@$(MAKE) $(TARGETS)

fred_calls_ted: fred_calls_ted.c foo.c fred.c ted.c
	cc $(LCDEFS) -o fred_calls_ted fred_calls_ted.c foo.c fred.c ted.c $(LLDLIBS)

clobber:
	-rm -f $(TARGETS)
	-rm -f *.a
	-rm -f *.o
	-rm -f core*
	-rm -f so_loc*
	-rm -f fred_calls_ted
	-rm -fr test00*
	-rm -fr *html*

clean:
	@$(MAKE) $(TARGETS)
	-rm -f *.o
	-rm -f core*
