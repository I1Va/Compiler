#include "general.hpp"
#include "segments_gen.hpp"

#include <assert.h>
#include <cstring>


void fill_text_segment_info__TEMP(segment_info_t *segment) {
    assert(segment);

    // TEMP
    memcpy(segment->p_type, PT_LOAD_CODE, P_TYPE_NMEMB);

    const unsigned char TEMP_TEXT_OFFSET[P_OFFSET_NMEMB] = {0x80, 0x00, 0x00, 0x00};
    memcpy(segment->p_offset, TEMP_TEXT_OFFSET, P_OFFSET_NMEMB);

    const unsigned char TEMP_TEXT_VADDR[P_VADDR_NMEMB] = {0x80, 0x80, 0x04, 0x08};
    memcpy(segment->p_vaddr, TEMP_TEXT_VADDR, P_VADDR_NMEMB);

    write_bytes_to_field(segment->p_paddr, P_PADDR_NMEMB, 0x00, 0x00, 0x00, 0x00);


    memcpy(segment->p_vaddr, TEMP_TEXT_VADDR, P_VADDR_NMEMB);

    const unsigned char TEMP_TEXT_FILESZ[P_FILESZ_NMEBM] = {0x24, 0x00, 0x00, 0x00};
    memcpy(segment->p_filesz,TEMP_TEXT_FILESZ, P_FILESZ_NMEBM);

    const unsigned char TEMP_TEXT_MEMSZ[P_FILESZ_NMEBM] = {0x24, 0x00, 0x00, 0x00};
    memcpy(segment->p_memsz, TEMP_TEXT_MEMSZ, P_FILESZ_NMEBM);

    write_bytes_to_field(segment->p_flags, P_FLAGS_NMEMB, READ_P_FLAG | EXEC_P_FLAG, 0x00, 0x00, 0x00);
    memcpy(segment->p_align, P_DEFAULT_ALIGN, P_ALIGN_NMEMB);
}

void fill_data_segment_info__TEMP(segment_info_t *segment) {
    assert(segment);

    // TEMP
    memcpy(segment->p_type, PT_LOAD_CODE, P_TYPE_NMEMB);

    const unsigned char TEMP_DATA_OFFSET[P_OFFSET_NMEMB] = {0xA4, 0x00, 0x00, 0x00};
    memcpy(segment->p_offset, TEMP_DATA_OFFSET, P_OFFSET_NMEMB);

    const unsigned char TEMP_DATA_VADDR[P_VADDR_NMEMB] = {0xA4, 0x80, 0x04, 0x08};
    memcpy(segment->p_vaddr, TEMP_DATA_VADDR, P_VADDR_NMEMB);

    write_bytes_to_field(segment->p_paddr, P_PADDR_NMEMB, 0x00, 0x00, 0x00, 0x00);

    const unsigned char TEMP_DATA_FILESZ[P_FILESZ_NMEBM] = {0x20, 0x00, 0x00, 0x00};
    memcpy(segment->p_filesz, TEMP_DATA_FILESZ, P_FILESZ_NMEBM);

    const unsigned char TEMP_DATA_MEMSZ[P_FILESZ_NMEBM] = {0x20, 0x00, 0x00, 0x00};
    memcpy(segment->p_memsz, TEMP_DATA_MEMSZ, P_FILESZ_NMEBM);

    write_bytes_to_field(segment->p_flags, P_FLAGS_NMEMB, READ_P_FLAG | WRITE_P_FLAG | EXEC_P_FLAG, 0x00, 0x00, 0x00);
    memcpy(segment->p_align, P_DEFAULT_ALIGN, P_ALIGN_NMEMB);
}
