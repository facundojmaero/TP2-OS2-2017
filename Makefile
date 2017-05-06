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

all: make_dirs single_threaded multithreaded

make_dirs:
	mkdir -p obj
	mkdir -p build

single_threaded: $(OBJECTS_SINGLE_THREADED)
	gcc $(PATHOBJECTS_SINGLE_THREADED) -o $(BDIR)/$@ -lm

single_threaded.o: $(SRCDIR)/single_threaded.c $(LDIR)/colors.h $(LDIR)/single_threaded.h
	$(CC) $(CFLAGS) -c $< -o $(ODIR)/$@

func_single_thread.o: $(SRCDIR)/func_single_thread.c $(LDIR)/colors.h
	$(CC) $(CFLAGS) -c $< -o $(ODIR)/$@

multithreaded: $(OBJECTS_MULTITHREADED)
	gcc $(PATHOBJECTS_MULTITHREADED) -o $(BDIR)/$@ -lm $(PARFLAGS)

multithreaded.o: $(SRCDIR)/multithreaded.c $(LDIR)/colors.h $(LDIR)/multithreaded.h
	$(CC) $(CFLAGS) -c $< -o $(ODIR)/$@ $(PARFLAGS)

func_multithreaded.o: $(SRCDIR)/func_multithreaded.c $(LDIR)/colors.h
	$(CC) $(CFLAGS) -c $< -o $(ODIR)/$@ $(PARFLAGS)

cppcheck:
	@echo
	@echo Realizando verificacion CppCheck
	@echo Los resultados de la misma se encuentran en err.txt
	cppcheck --enable=all . 2> err.txt

clean:
	@echo Borrando archivos
	rm -rf *o $(ODIR) $(BDIR) $(LDIR)/*.gch err.txt
