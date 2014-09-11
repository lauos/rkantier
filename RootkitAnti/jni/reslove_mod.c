#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>

#include "debug.h"

struct modversion_info
{
	unsigned long crc;
	char name[60];
};

/* Parse tag=value strings from .modinfo section */
static char *next_string(char *string, unsigned long *secsize)
{
	/* Skip non-zero chars */
	while (string[0]) {
		string++;
		if ((*secsize)-- <= 1)
			return NULL;
	}

	/* Skip any zero padding. */
	while (!string[0]) {
		string++;
		if ((*secsize)-- <= 1)
			return NULL;
	}
	return string;
}

static char *get_modinfo(char *info, unsigned long size, const char *tag)
{
	char *p;
	unsigned int taglen = strlen(tag);
	char* result = NULL;

	for (p = (char *)info; p; p = next_string(p, &size))
	{
		//EGIS_DEBUG("Modinfo: %s\n", p);
		if (strncmp(p, tag, taglen) == 0 && p[taglen] == '=')
		{
			result =  p + taglen + 1;
		}
	}
	return result;
}

int load_and_reslove (const char* path, char* true_magic, unsigned int* true_checksum)
{
	FILE* fd;
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int k = 0;

	unsigned char* oriso;
	unsigned int soSize;

    oriso = path;

	fd = fopen(oriso,"rb");
    fseek(fd, 0, SEEK_END);
    soSize = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    EGIS_DEBUG("File size is %u\n", soSize);

    void* soPtr = (void *)calloc(1, soSize);

	if(fread(soPtr, 1, soSize, fd) != soSize)
    {
        EGIS_DEBUG("Read file error\n");
        fclose(fd);
        exit(1);
    }
    fclose(fd);

	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)soPtr;

/*	EGIS_DEBUG("Ident : %s\n", ehdr->e_ident);
	EGIS_DEBUG("Type  : %u\n", ehdr->e_type);
	EGIS_DEBUG("Machine : %u\n", ehdr->e_machine);
	EGIS_DEBUG("Version : %u\n", ehdr->e_version);
	EGIS_DEBUG("Entry point address: %p\n", ehdr->e_entry);
	EGIS_DEBUG("Start of program headers: %u\n", ehdr->e_phoff);
	EGIS_DEBUG("Start of section headers: %u\n", ehdr->e_shoff);
	EGIS_DEBUG("Flags : 0x%08x\n", ehdr->e_flags);
	EGIS_DEBUG("Size of this header: : %u\n", ehdr->e_ehsize);
	EGIS_DEBUG("Size of program headers: %u\n", ehdr->e_phentsize);
	EGIS_DEBUG("Number of program headers: : %u\n", ehdr->e_phnum);
	EGIS_DEBUG("Size of section headers: : %u\n", ehdr->e_shentsize);
	EGIS_DEBUG("Number of section headers: %u\n", ehdr->e_shnum);
	EGIS_DEBUG("Section header string table index: %u\n\n", ehdr->e_shstrndx);*/


	Elf32_Sym* sym = NULL;

	char* strtab = NULL;

	Elf32_Shdr* shdr = (Elf32_Shdr *)(ehdr->e_shoff + (Elf32_Off)ehdr) ;
	Elf32_Shdr* strshdr = (Elf32_Shdr *)&shdr[ehdr->e_shstrndx];

	const char* sh_strtab = (char *)(strshdr->sh_offset + (Elf32_Off)ehdr);
	for(i = 0; i < ehdr->e_shnum; i++, shdr++)
	{
/*		EGIS_DEBUG("Section[%u] : 0x%08x\n", i, shdr->sh_type);
		EGIS_DEBUG("---->sh_name :%s\n", (char *)&(sh_strtab[shdr->sh_name]));
		EGIS_DEBUG("---->sh_flags :0x%08x\n", shdr->sh_flags);
		EGIS_DEBUG("---->sh_offset :%u\n", shdr->sh_offset);
		EGIS_DEBUG("---->sh_size :%u\n", shdr->sh_size);
		EGIS_DEBUG("---->sh_link :%u\n", shdr->sh_link);
		EGIS_DEBUG("---->sh_info :%u\n", shdr->sh_info);
		EGIS_DEBUG("---->sh_addralign :%u\n", shdr->sh_addralign);
		EGIS_DEBUG("---->sh_entsize :%u\n\n", shdr->sh_entsize);*/

		if(!strcmp(&(sh_strtab[shdr->sh_name]), ".strtab"))
		{
			strtab = (char *)(shdr->sh_offset + (Elf32_Off)ehdr);
		}
		if(!strcmp(&(sh_strtab[shdr->sh_name]), ".symtab"))
		{
			sym = (char *)(shdr->sh_offset + (Elf32_Off)ehdr);
		}
	}

	shdr = (Elf32_Shdr *)(ehdr->e_shoff + (Elf32_Off)ehdr);

	for(i = 0; i < ehdr->e_shnum; i++, shdr++)
	{
		if(!strcmp(&(sh_strtab[shdr->sh_name]), ".gnu.linkonce.this_module"))
		{
			EGIS_DEBUG("\n");
		}
		if(!strcmp(&(sh_strtab[shdr->sh_name]), "__versions"))
		{
			unsigned int j, num_versions;
			struct modversion_info *versions;
			versions = (void *) (shdr->sh_offset + (Elf32_Off)ehdr);
			num_versions = shdr->sh_size / sizeof(struct modversion_info);

			EGIS_DEBUG("number versions : %u\n", num_versions);

			for (j = 0; j < num_versions; j++)
			{

				//EGIS_DEBUG("Versions[%d] : %s\n", j, versions[j].name);

				if (!strcmp(versions[j].name, "module_layout"))
				{
					EGIS_DEBUG("CRC of module_layout is : %08lx\n", versions[j].crc);
					*true_checksum = versions[j].crc;
				}
			}

		}
		if(!strcmp(&(sh_strtab[shdr->sh_name]), ".modinfo"))
		{
			const char *modmagic = get_modinfo((char* )(shdr->sh_offset + (Elf32_Off)ehdr), shdr->sh_size, "vermagic");
			EGIS_DEBUG("Vermagic : %s\n", modmagic);
			memcpy(true_magic, modmagic, strlen(modmagic) + 1);
		}
	}
}
