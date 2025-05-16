#include <assert.h>
#include <cstddef>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "elf_gen.hpp"
#include "general.hpp"
#include "segments_gen.hpp"
#include "elf_header_gen.hpp"


void fill_executable_linked_format(elf_t *elf) {
    assert(elf);

    fill_elf_header(&elf->elf_header);
    fill_text_segment_info__TEMP(&elf->text_segment);
    fill_data_segment_info__TEMP(&elf->data_segment);
}

unsigned char *copy_elf_header_to_mem(elf_header_t *elf_header, unsigned char *mem) {
    assert(elf_header);
    assert(mem);

    #define COPY_FROM_FIELD_TO_MEM(field_name, field_sz) {memcpy(mem, elf_header->field_name, field_sz); mem += field_sz;};

    COPY_FROM_FIELD_TO_MEM(e_ident, E_IDENT_NMEMB)
    COPY_FROM_FIELD_TO_MEM(e_type, E_TYPE_NMEMB);
    COPY_FROM_FIELD_TO_MEM(e_machine, E_MACHINE_NMEMB);
    COPY_FROM_FIELD_TO_MEM(e_version, E_VERSION_NMEMB);
    COPY_FROM_FIELD_TO_MEM(e_entry, E_ENTRY_NMEMB);
    COPY_FROM_FIELD_TO_MEM(e_phoff, E_PHOFF_NMEMB);
    COPY_FROM_FIELD_TO_MEM(e_shoff, E_SHOFF_NMEMB);
    COPY_FROM_FIELD_TO_MEM(e_flags, E_FLAGS_NMEMB);
    COPY_FROM_FIELD_TO_MEM(e_ehsize, E_EHSIZE_NMEMB);
    COPY_FROM_FIELD_TO_MEM(e_phentsize, E_PHENTSIZE_NMEMB);
    COPY_FROM_FIELD_TO_MEM(e_phnum, E_PHNUM_NMEMB);
    COPY_FROM_FIELD_TO_MEM(e_shentsize, E_SHENTSIZE_NMEMB);
    COPY_FROM_FIELD_TO_MEM(e_shnum, E_SHNUM_NMEMB);
    COPY_FROM_FIELD_TO_MEM(e_shstrndx, E_SHSTRNDX_NMEMB);

    #undef COPY_FROM_FIELD_TO_MEM

    return mem;
}

unsigned char *copy_elf_segment_info_to_mem(segment_info_t *segment_info, unsigned char *mem) {
    assert(segment_info);
    assert(mem);

    #define COPY_FROM_FIELD_TO_MEM(field_name, field_sz) {memcpy(mem, segment_info->field_name, field_sz); mem += field_sz;};

    COPY_FROM_FIELD_TO_MEM(p_type, P_TYPE_NMEMB);
    COPY_FROM_FIELD_TO_MEM(p_offset, P_OFFSET_NMEMB);
    COPY_FROM_FIELD_TO_MEM(p_vaddr, P_VADDR_NMEMB);
    COPY_FROM_FIELD_TO_MEM(p_paddr, P_PADDR_NMEMB);
    COPY_FROM_FIELD_TO_MEM(p_filesz, P_FILESZ_NMEBM);
    COPY_FROM_FIELD_TO_MEM(p_memsz, P_MEMSZ_NMEMB);
    COPY_FROM_FIELD_TO_MEM(p_flags, P_FLAGS_NMEMB);
    COPY_FROM_FIELD_TO_MEM(p_align, P_ALIGN_NMEMB);

    #undef COPY_FROM_FIELD_TO_MEM

    return mem;
}

bool create_executable_linked_file(elf_t *elf, const char outpath[]) {
    assert(elf);
    assert(outpath);

    const size_t MAX_ELF_FILE_SZ = 1024;
    unsigned char elf_file_mem_space[MAX_ELF_FILE_SZ] = {};
    unsigned char *cur_mem_addr = elf_file_mem_space;

    cur_mem_addr = copy_elf_header_to_mem(&elf->elf_header, cur_mem_addr);
    cur_mem_addr = copy_elf_segment_info_to_mem(&elf->text_segment, cur_mem_addr);
    cur_mem_addr = copy_elf_segment_info_to_mem(&elf->data_segment, cur_mem_addr);

    assert(size_t(cur_mem_addr - elf_file_mem_space) == 0x74); // TEMP ASSERT

    // 0x80 - адресс начала тела сегмента текста (offset)
    const unsigned char TEMP_TEXT_SEGMENT_BODY[] = {0xBB, 0x01, 0x00, 0x00, 0x00, 0xB8, 0x04, 0x00, 0x00, 0x00, 0xB9, 0xA4, 0x80, 0x04, 0x08, 0xBA, 0x0D, 0x00, 0x00, 0x00, 0xCD, 0x80, 0xB8, 0x01, 0x00, 0x00, 0x00, 0xBB, 0x2A, 0x00, 0x00, 0x00, 0xCD, 0x80};
                                                                                                                   //[^^^^^^^^^^^^^^^^^^^^^^] <- виртуальный адресс строчки в сегменте данных
    memcpy(elf_file_mem_space + 0x80, TEMP_TEXT_SEGMENT_BODY, 0x22);

    // 0xA4 - адресс начала тела сегмента данных (offset)
    const unsigned char TEMP_DATA_SEGMENT_BODY[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, 0x21, 0x0A};
    memcpy(elf_file_mem_space + 0xA4, TEMP_DATA_SEGMENT_BODY, 0x20);

    // размер всего elf файла = 0xB0




    FILE *elf_outfile = fopen(outpath, "wb");
    if (elf_outfile == NULL) {
        debug("open '%s' failed\n", outpath);
        return false;
    }

    fwrite((void *) (elf_file_mem_space), sizeof(unsigned char), 0xB1, elf_outfile); // FIXME:!!!!! WARNING: (void *) (elf) - кринж, переписать
    return true;
}