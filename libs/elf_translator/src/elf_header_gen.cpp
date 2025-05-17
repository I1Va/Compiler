#include <assert.h>
#include <string.h>

#include "elf_header_gen.hpp"
#include "elf_gen.hpp"
#include "general.h"

const unsigned char GAP_BYTE = 0x0;

void fill_field_seg_gap(unsigned char field[], const size_t left, const size_t right) {
    assert(field);

    for (size_t i = left; i<= right; i++) { field[i] = GAP_BYTE; }
}

void fill_elf_identifier(unsigned char e_ident[E_IDENT_NMEMB]) {
    assert(e_ident);

    e_ident[0] = 0x7F;
    e_ident[1] = 'E';
    e_ident[2] = 'L';
    e_ident[3] = 'F';

    e_ident[4] = ARCHITECTURE_64B_CODE;
    e_ident[5] = LITTLE_ENDIAN_CODE;
    e_ident[6] = ELF_VERSION_CODE;

    fill_field_seg_gap(e_ident, 7, 15); // [0:15] = 0
}

void fill_elf_type(unsigned char e_type[E_TYPE_NMEMB]) {
    assert(e_type);

    e_type[0] = EXEC_FILE_TYPE;
    e_type[1] = GAP_BYTE;
}

void fill_elf_machine(unsigned char e_machine[E_MACHINE_NMEMB]) {
    assert(e_machine);
    e_machine[0] = AMD_X86_64_MACHINE_CODE;
    e_machine[1] = GAP_BYTE;
}

void fill_elf_version(unsigned char e_version[E_VERSION_NMEMB]) {
    assert(e_version);

    e_version[0] = ELF_VERSION;
    fill_field_seg_gap(e_version, 1, 3);
}

void fill_elf_header(elf_header_t *elf_header) {
    assert(elf_header);

    fill_elf_identifier(elf_header->e_ident);
    fill_elf_type(elf_header->e_type);
    fill_elf_machine(elf_header->e_machine);
    fill_elf_version(elf_header->e_version);

    // TEMP:


    const unsigned char TEMP_ENTRY_POINT[E_ENTRY_NMEMB] = {0x80, 0x80, 0x04, 0x08};
    memcpy(elf_header->e_entry, TEMP_ENTRY_POINT, E_ENTRY_NMEMB);


    write_bytes_to_field(elf_header->e_phoff, E_PHOFF_NMEMB, 0x34, 0x00, 0x00, 0x00);
    write_bytes_to_field(elf_header->e_shoff, E_SHOFF_NMEMB, 0x00, 0x00, 0x00, 0x00);

    write_bytes_to_field(elf_header->e_flags, E_FLAGS_NMEMB, 0x00, 0x00, 0x00, 0x00);

    write_bytes_to_field(elf_header->e_ehsize, E_EHSIZE_NMEMB, 0x34, 0x00);
    write_bytes_to_field(elf_header->e_phentsize, E_PHENTSIZE_NMEMB, 0x20, 0x00);
    write_bytes_to_field(elf_header->e_phnum , E_PHNUM_NMEMB, 0x02, 0x00);

    write_bytes_to_field(elf_header->e_shentsize, E_SHENTSIZE_NMEMB, 0x00, 0x00);
    write_bytes_to_field(elf_header->e_shnum, E_SHNUM_NMEMB, 0x00, 0x00);
    write_bytes_to_field(elf_header->e_shstrndx , E_SHSTRNDX_NMEMB, 0x00, 0x00);
}
