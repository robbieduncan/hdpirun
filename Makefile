CC=gcc
ODIR=obj
SRCDIR=src
IDIR=include

CFLAGS=-I$(IDIR)

_HEADERS = \
log.h \
abort.h
HEADERS = $(patsubst %,$(IDIR)/%,$(_HEADERS))

_OBJ = \
log.o\
hdpirun.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	@mkdir -p $(ODIR)
	@$(CC) -c -o $@ $< $(CFLAGS)

hdpirun: $(OBJ)
	@gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	@rm -rf $(ODIR)
	@rm -f hdpirun