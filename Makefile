CC = gcc
CFLAGS = -Wall -pedantic -Werror -O2
PARFLAGS = -fopenmp
ODIR=obj
LDIR=include
SRCDIR=src
BDIR=build
PATHOBJECTS_SINGLE_THREADED=$(addprefix $(ODIR)/,$(OBJECTS_SINGLE_THREADED))
OBJECTS_SINGLE_THREADED=single_threaded.o func_single_thread.o
PATHOBJECTS_MULTITHREADED=$(addprefix $(ODIR)/,$(OBJECTS_MULTITHREADED))
OBJECTS_MULTITHREADED=multithreaded.o func_multithreaded.o

all: make_dirs build/single_threaded build/multithreaded

make_dirs:
	mkdir -p obj
	mkdir -p build

build/single_threaded: $(PATHOBJECTS_SINGLE_THREADED)
	gcc $(PATHOBJECTS_SINGLE_THREADED) -o $@ -lm

obj/single_threaded.o: $(SRCDIR)/single_threaded.c $(LDIR)/colors.h $(LDIR)/single_threaded.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/func_single_thread.o: $(SRCDIR)/func_single_thread.c $(LDIR)/colors.h
	$(CC) $(CFLAGS) -c $< -o $@

build/multithreaded: $(PATHOBJECTS_MULTITHREADED)
	gcc $(PATHOBJECTS_MULTITHREADED) -o $@ -lm $(PARFLAGS)

obj/multithreaded.o: $(SRCDIR)/multithreaded.c $(LDIR)/colors.h $(LDIR)/multithreaded.h
	$(CC) $(CFLAGS) -c $< -o $@ $(PARFLAGS)

obj/func_multithreaded.o: $(SRCDIR)/func_multithreaded.c $(LDIR)/colors.h
	$(CC) $(CFLAGS) -c $< -o $@ $(PARFLAGS)

cppcheck:
	@echo
	@echo Realizando verificacion CppCheck
	@echo Los resultados de la misma se encuentran en err.txt
	cppcheck --enable=all . 2> err.txt

clean:
	@echo Borrando archivos
	rm -rf *o $(ODIR) $(BDIR) $(LDIR)/*.gch err.txt
