main: libs
	gcc -o AutoHead main.c -L. -l:libs/libstr.a
libs:
	@$(MAKE) -C libs
