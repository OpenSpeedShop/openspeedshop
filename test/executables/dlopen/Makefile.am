noinst_PROGRAMS = mutatee libwork

mutatee_CFLAGS = \
	-g -gdwarf-2 -O0

mutatee_LDADD = \
	-ldl

mutatee_SOURCES = \
	mutatee.c 


libwork_CFLAGS = \
	-g -gdwarf-2 -shared -O0

libwork_LDFLAGS = \
        -shared -o libwork.so

libwork_SOURCES = \
	libwork.c 
