CC = gcc
CFLAGS = -Wall -pedantic -Werror -O2
ODIR=obj
LDIR=include
SRCDIR=src
BDIR=build
PATHOBJECTS=$(addprefix $(ODIR)/,$(OBJECTS))
OBJECTS=single_threaded.o func_single_thread.o

all: make_dirs single_threaded

make_dirs:
	mkdir -p obj
	mkdir -p build

single_threaded: $(OBJECTS)
	gcc $(PATHOBJECTS) -o $(BDIR)/$@ -lprofiler -lm

single_threaded.o: $(SRCDIR)/single_threaded.c $(LDIR)/single_threaded.h
	$(CC) $(CFLAGS) -c $< -o $(ODIR)/$@

func_single_thread.o: $(SRCDIR)/func_single_thread.c
	$(CC) $(CFLAGS) -c $< -o $(ODIR)/$@	


cppcheck:
	@echo
	@echo Realizando verificacion CppCheck
	@echo Los resultados de la misma se encuentran en err.txt
	cppcheck --enable=all . 2> err.txt

clean:
	@echo Borrando archivos
	rm -rf *o $(ODIR) $(BDIR) $(LDIR)/*.gch err.txt
