#
#  vlogger 2.2
#  (C) 2002 by RD <rd@thc.org>
#

include make.conf

bin_PROGRAMS    = vlogger vlogsrv

all: $(bin_PROGRAMS)
	@echo "*** Excute 'vlogconfig' to configure vlogger module."

vlogger: vlogger.c
ifneq ($(KERNELVER), LINUX26)
	$(CC) $(CFLAGS) $(MODDEFS) $(MODFLAGS) -c $^ -o $@.o
else
	$(CC) $(CFLAGS) $(KCFLAGS) $(MODDEFS) $(MODFLAGS) -DKBUILD_BASENAME=$@ -DKBUILD_MODNAME=$@ -c $^ -o $@.o
	$(KERNELDIR)/scripts/modpost $@.o
	$(CC) $(CFLAGS) $(KCFLAGS) $(MODDEFS) $(MODFLAGS) -c -o $@.mod.o $@.mod.c
	ld -m elf_i386 -r -o $@.ko $@.o $@.mod.o
endif

vlogsrv: vlogsrv.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(bin_PROGRAMS) *.o *.mod.* *.ko
