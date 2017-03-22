/*
 * Replace a symbol in symtab with a new one
 * by rd <rd@thc.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

// change below line for 64bit
#define ELF_CLASS_32

#ifdef ELF_CLASS_32

#define Elf_Ehdr    Elf32_Ehdr
#define Elf_Shdr    Elf32_Shdr
#define Elf_Sym     Elf32_Sym
#define ELF_ST_BIND ELF32_ST_BIND
#define ELF_ST_TYPE ELF32_ST_TYPE

#else

#define Elf_Ehdr    Elf64_Ehdr
#define Elf_Shdr    Elf64_Shdr
#define Elf_Sym     Elf64_Sym
#define ELF_ST_BIND ELF64_ST_BIND
#define ELF_ST_TYPE ELF64_ST_TYPE

#endif

struct elf_info
{
	unsigned long size;
	Elf_Ehdr *hdr;
	Elf_Shdr *sechdrs;
	Elf_Sym *symtab_start;
	Elf_Sym *symtab_stop;
	char *strtab;
};

void *
grab_file(const char *filename, unsigned long *size)
{
	struct stat st;
	void *map;
	int fd;

	fd = open(filename, O_RDWR);
	if (fd < 0) {
		perror(filename);
		abort();
	}
	if (fstat(fd, &st) != 0) {
		perror(filename);
		abort();
	}

	*size = st.st_size;
	map = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED) {
		perror(filename);
		abort();
	}
	close(fd);
	return map;
}

void
parse_elf(struct elf_info *info, const char *filename)
{
	unsigned int i;
	Elf_Ehdr *hdr = info->hdr;
	Elf_Shdr *sechdrs;
	Elf_Sym *sym;

	hdr = grab_file(filename, &info->size);
	info->hdr = hdr;
	if (info->size < sizeof(*hdr))
		goto truncated;

	/* Check ELF header "\177ELF" */
	if (memcmp(info->hdr, ELFMAG, SELFMAG) != 0) {
		fprintf(stderr, "%s is not an elf object\n", filename);
		abort();
	}

	sechdrs = (void *) hdr + hdr->e_shoff;
	info->sechdrs = sechdrs;

	/* Find symbol table. */
	for (i = 1; i < hdr->e_shnum; i++) {
		if (sechdrs[i].sh_offset > info->size)
			goto truncated;
		if (sechdrs[i].sh_type != SHT_SYMTAB)
			continue;

		info->symtab_start = (void *) hdr + sechdrs[i].sh_offset;
		info->symtab_stop = (void *) hdr + sechdrs[i].sh_offset
		    + sechdrs[i].sh_size;
		info->strtab = (void *) hdr +
		    sechdrs[sechdrs[i].sh_link].sh_offset;
	}
	if (!info->symtab_start) {
		fprintf(stderr, "%s has no symtab\n", filename);
		abort();
	}
	return;

      truncated:
	fprintf(stderr, "%s is truncated.\n", filename);
	abort();
}

void
parse_elf_finish(struct elf_info *info)
{
	msync(info->hdr, info->size, MS_SYNC);
	munmap(info->hdr, info->size);
}

int
main(int argc, char **argv)
{
	int fl = 0;
	char *symname;
	Elf_Sym *sym;
	struct elf_info info = { };

	if (argc != 4) {
		fprintf(stderr, "usage: %s module orig_sym new_sym\n",
		    argv[0]);
		return -1;
	}

	if (strlen(argv[3]) > strlen(argv[2])) {
		fprintf(stderr, "Cannot replace %s with a longer symbol\n",
		    argv[2]);
		return -1;
	}

	parse_elf(&info, argv[1]);
	for (sym = info.symtab_start; sym < info.symtab_stop; sym++) {
		symname = info.strtab + sym->st_name;
		if (!strcmp(symname, argv[2])) {
			printf("Replacing %s to %s at offset 0x%x\n", symname,
			    argv[3], (void *) symname - (void *) info.hdr);
			strcpy(symname, argv[3]);
			fl = 1;
		}
	}
	parse_elf_finish(&info);

	if (!fl) {
		fprintf(stderr, "%s not found\n", argv[2]);
		return -1;
	}

	return 0;
}
