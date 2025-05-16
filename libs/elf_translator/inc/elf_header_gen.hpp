#ifndef ELF_HEADER_GEN_HPP
#define ELF_HEADER_GEN_HPP

#include <string.h>


// elf header fields NMEMB
const unsigned char E_IDENT_NMEMB     =  16;
const unsigned char E_TYPE_NMEMB      =  2;
const unsigned char E_MACHINE_NMEMB   =  2;
const unsigned char E_VERSION_NMEMB   =  4;
const unsigned char E_ENTRY_NMEMB     =  4;
const unsigned char E_PHOFF_NMEMB     =  4;
const unsigned char E_SHOFF_NMEMB     =  4;
const unsigned char E_FLAGS_NMEMB     =  4;
const unsigned char E_EHSIZE_NMEMB    =  2;
const unsigned char E_PHENTSIZE_NMEMB =  2;
const unsigned char E_PHNUM_NMEMB     =  2;
const unsigned char E_SHENTSIZE_NMEMB =  2;
const unsigned char E_SHNUM_NMEMB     =  2;
const unsigned char E_SHSTRNDX_NMEMB  =  2;

// elf_identifier constants
const unsigned char ARCHITECTURE_64B_CODE   = 0x02;
const unsigned char LITTLE_ENDIAN_CODE      = 0x01;
const unsigned char ELF_VERSION_CODE        = 0x01;

// elf_type constants
const unsigned char SHARED_OBJ_FILE_TYPE    = 0x03;
const unsigned char EXEC_FILE_TYPE          = 0x02;

// elf machine constants
const unsigned char AMD_X86_64_MACHINE_CODE = 0x3E;


// elf version constants
const unsigned char ELF_VERSION             = 0x01;


struct elf_header_t {
    unsigned char e_ident[E_IDENT_NMEMB]         = {};

    unsigned char e_type[E_TYPE_NMEMB]           = {};
    unsigned char e_machine[E_MACHINE_NMEMB]     = {};
    unsigned char e_version[E_VERSION_NMEMB]     = {};
    unsigned char e_entry[E_ENTRY_NMEMB]         = {};
    unsigned char e_phoff[E_PHOFF_NMEMB]         = {};
    unsigned char e_shoff[E_SHOFF_NMEMB]         = {};
    unsigned char e_flags[E_FLAGS_NMEMB]         = {};
    unsigned char e_ehsize[E_EHSIZE_NMEMB]       = {};
    unsigned char e_phentsize[E_PHENTSIZE_NMEMB] = {};
    unsigned char e_phnum[E_PHNUM_NMEMB]         = {};
    unsigned char e_shentsize[E_SHENTSIZE_NMEMB] = {};
    unsigned char e_shnum[E_SHNUM_NMEMB]         = {};
    unsigned char e_shstrndx[E_SHSTRNDX_NMEMB]   = {};
};

void fill_field_seg_gap(unsigned char field[], const size_t left, const size_t right);
void fill_elf_identifier(unsigned char e_ident[E_IDENT_NMEMB]);
void fill_elf_type(unsigned char e_type[E_TYPE_NMEMB]);
void fill_elf_machine(unsigned char e_machine[E_MACHINE_NMEMB]);
void fill_elf_version(unsigned char e_version[E_VERSION_NMEMB]);
void fill_elf_header(elf_header_t *elf_header);


#endif // ELF_HEADER_GEN_HPP