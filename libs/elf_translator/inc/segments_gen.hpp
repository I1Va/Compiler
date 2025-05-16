#ifndef SEGMENTS_BUILDER_HPP
#define SEGMENTS_BUILDER_HPP

// program segment fields nmemb
#define P_TYPE_NMEMB    4
#define P_OFFSET_NMEMB  4
#define P_VADDR_NMEMB   4
#define P_PADDR_NMEMB   4
#define P_FILESZ_NMEBM  4
#define P_MEMSZ_NMEMB   4
#define P_FLAGS_NMEMB   4
#define P_ALIGN_NMEMB   4

struct segment_info_t {
    unsigned char p_type[P_TYPE_NMEMB]       = {};
    unsigned char p_offset[P_OFFSET_NMEMB]   = {};
    unsigned char p_vaddr[P_VADDR_NMEMB]     = {};
    unsigned char p_paddr[P_PADDR_NMEMB]     = {};
    unsigned char p_filesz[P_FILESZ_NMEBM]   = {};
    unsigned char p_memsz[P_MEMSZ_NMEMB]     = {};
    unsigned char p_flags[P_FLAGS_NMEMB]     = {};
    unsigned char p_align[P_ALIGN_NMEMB]     = {};
};

const unsigned char PT_LOAD_CODE[] = {0x01, 0x00, 0x00, 0x00};
const unsigned char P_DEFAULT_ALIGN[] = {0x00, 0x10, 0x00, 0x00}; // т.к. обычно размер страницы = 0x1000
const unsigned char READ_P_FLAG = 0x04;
const unsigned char WRITE_P_FLAG = 0x02;
const unsigned char EXEC_P_FLAG  = 0x01;

void fill_text_segment_info__TEMP(segment_info_t *segment);
void fill_data_segment_info__TEMP(segment_info_t *segment);
#endif // SEGMENTS_BUILDER_HPP