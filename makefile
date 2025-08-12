main: libstr
	gcc -o AutoHead main.c -L. -l:libs/libstr.a
libstr:
	@$(MAKE) -C libs

clean:
	rm -rf *.o
