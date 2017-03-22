include ../make.conf

bin_PROGRAMS    = chelfsym modclean

all: $(bin_PROGRAMS)

inject: inject.c
	@echo "[CC] $^"
ifneq ($(KERNELVER), LINUX26)
	@$(CC) $(CFLAGS) $(MODDEFS) $(MODFLAGS) -c $^ -o $@.o
else
	@$(CC) $(CFLAGS) $(KCFLAGS) $(MODDEFS) $(MODFLAGS) -c $^ -o $@.o
endif

chelfsym: chelfsym.c
	@echo "[CC] $^"
	@$(CC) $^ -o $@

modclean: modclean.c
ifneq ($(KERNELVER), LINUX26)
	$(CC) $(CFLAGS) $(MODDEFS) $(MODFLAGS) -c $^ -o $@.o
else
	$(CC) $(CFLAGS) $(KCFLAGS) $(MODDEFS) $(MODFLAGS) -DKBUILD_BASENAME=$@ -DKBUILD_MODNAME=$@ -c $^ -o $@.o
	$(KERNELDIR)/scripts/modpost $@.o
	$(CC) $(CFLAGS) $(KCFLAGS) $(MODDEFS) $(MODFLAGS) -c -o $@.mod.o $@.mod.c
	ld -m elf_i386 -r -o $@.ko $@.o $@.mod.o
endif

clean:
	@rm -f *.o *.ko *.mod.* $(bin_PROGRAMS)
