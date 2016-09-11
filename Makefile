CC=gcc
ODIR=obj
SRCDIR=src
IDIR=include
HDIR=libhdhomerun
LIBEXT=dylib

CFLAGS=-I$(IDIR) -I$(HDIR)
EXEFLAGS=-L$(HDIR)

_HEADERS = \
log.h \
exit.h\
commandline.h\
network.h\
homerun.h
HEADERS = $(patsubst %,$(IDIR)/%,$(_HEADERS))

_HDHEADERS = \
hdhomerun_pkt.h
HDHEADERS = $(patsubst %,$(HDIR)/%,$(_HDHEADERS))

_OBJ = \
log.o\
exit.o\
commandline.o\
network.o\
homerun.o\
hdpirun.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c $(HEADERS) $(HDHEADERS)
	@mkdir -p $(ODIR)
	@echo $@
	@$(CC) -c -o $@ $< $(CFLAGS)

libhdhomerun:
	@echo "Building libhdhomerun"
	@cd libhdhomerun;make

hdpirun: $(OBJ)
	@echo $@
	@gcc -o $@ $(CFLAGS) $(EXEFLAGS) $^ -lhdhomerun

.PHONY: clean libhdhomerun

clean:
	@rm -rf $(ODIR)
	@rm -f hdpirun