GENERATES = prog prog-a prog-so README liboutput_static.a liboutput.so
TRASH = *.o *~ o.* test_output
CFLAGS = -Wall

all:    README prog prog-a prog-so

fun.o: fun.c outlib.h

const.o: const.c

prog.o: prog.c outlib.h

liboutput_static.a: fun.o const.o
	ar rcs $@ $^

funpic.o: fun.c outlib.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

constpic.o: const.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

liboutput.so: funpic.o constpic.o
	$(CC) -shared -o $@ $^

prog: prog.o fun.o const.o

prog-a: prog.o liboutput_static.a
	$(CC) $(CFLAGS) -o $@ $^

prog-so: prog.o liboutput.so
	$(CC) $(CFLAGS) -o $@ $< -L. -loutput -Wl,-rpath,.

README: prog
	./$< 2> $@

test: prog prog-a prog-so
	@echo "Running tests..."
	@mkdir -p test_output
	@for bin in prog prog-a prog-so; do \
	    echo "Testing $$bin with no arguments..."; \
		ln -sf $$bin prog; \
	    ./prog > test_output/$$bin.noargs.out 2>&1; \
	    echo "Testing $$bin with one argument..."; \
	    ./prog arg1 > test_output/$$bin.onearg.out 2>&1; \
	    echo "Testing $$bin with three arguments..."; \
	    ./prog arg1 arg2 arg3 > test_output/$$bin.threeargs.out 2>&1; \
	done
	@echo "Comparing outputs..."
	@cmp test_output/prog.noargs.out test_output/prog-a.noargs.out
	@cmp test_output/prog.noargs.out test_output/prog-so.noargs.out
	@cmp test_output/prog.onearg.out test_output/prog-a.onearg.out
	@cmp test_output/prog.onearg.out test_output/prog-so.onearg.out
	@cmp test_output/prog.threeargs.out test_output/prog-a.threeargs.out
	@cmp test_output/prog.threeargs.out test_output/prog-so.threeargs.out
	@echo "Tests passed."

clean:
	rm -rf $(TRASH)
	rm -f $(GENERATES)

distclean: clean
