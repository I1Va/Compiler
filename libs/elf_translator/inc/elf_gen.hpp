#ifndef ELF_GEN_HPP
#define ELF_GEN_HPP

#include "elf_header_gen.hpp"
#include "segments_gen.hpp"


struct elf_t {
    elf_header_t elf_header = {};
    segment_info_t text_segment = {};
    segment_info_t data_segment = {};
};

void fill_executable_linked_format(elf_t *elf);
bool create_executable_linked_file(elf_t *elf, const char outpath[]);


#endif // ELF_GEN_HPP