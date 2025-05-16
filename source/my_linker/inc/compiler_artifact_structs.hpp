#include <stdint.h>

#define MY_SHN_UNDEF 0

/* ------------------------------------------------------------- */
/* 1. ELF-заголовок (минимум для ET_REL)                         */
/* ------------------------------------------------------------- */
#define EI_NIDENT 16

typedef struct {
    unsigned char e_ident[EI_NIDENT]; /* 0x7F 'E' 'L' 'F', класс, endian... */
    uint16_t      e_type;             /* ET_REL=1 */
    uint16_t      e_machine;          /* EM_X86_64=62 */
    uint32_t      e_version;          /* EV_CURRENT=1 */
    uint64_t      e_entry;            /* 0 для ET_REL */
    uint64_t      e_phoff;            /* 0 (нет Program Header) */
    uint64_t      e_shoff;            /* смещение Section Header Table */
    uint32_t      e_flags;            /* 0 */
    uint16_t      e_ehsize;           /* sizeof(Elf64_Ehdr) */
    uint16_t      e_phentsize;        /* 0 */
    uint16_t      e_phnum;            /* 0 */
    uint16_t      e_shentsize;        /* sizeof(Elf64_Shdr) */
    uint16_t      e_shnum;            /* число секций */
    uint16_t      e_shstrndx;         /* индекс .shstrtab */
} Elf64_Ehdr;

/* ------------------------------------------------------------- */
/* 2. Section Header для всех секций                             */
/* ------------------------------------------------------------- */
typedef struct {
    uint32_t sh_name;      /* индекс в .shstrtab */
    uint32_t sh_type;      /* SHT_SYMTAB, SHT_STRTAB, SHT_RELA, SHT_PROGBITS */
    uint64_t sh_flags;     /* SHF_ALLOC/WRITE/EXEC */
    uint64_t sh_addr;      /* 0 для ET_REL */
    uint64_t sh_offset;    /* смещение секции в файле */
    uint64_t sh_size;      /* размер секции */
    uint32_t sh_link;      /* для RELA: индекс .symtab; для SYMTAB: .strtab */
    uint32_t sh_info;      /* для SYMTAB: индекс первого глобального;
                               для RELA: номер `.text` или `.data` секции */
    uint64_t sh_addralign; /* выравнивание */
    uint64_t sh_entsize;   /* размер записи (для SYMTAB=24, RELA=24) */
} Elf64_Shdr;

/* ------------------------------------------------------------- */
/* 3. Запись символа в .symtab                                    */
/* ------------------------------------------------------------- */
typedef struct {
    uint32_t st_name;  /* смещение имени в .strtab */
    uint8_t  st_info;  /* binding/type */
    uint8_t  st_other; /* 0 */
    uint16_t st_shndx; /* SHN_UNDEF или индекс секции .text/.data */
    uint64_t st_value; /* смещение внутри секции */
    uint64_t st_size;  /* размер */
} Elf64_Sym;

/* ------------------------------------------------------------- */
/* 4. Релокационная запись для .rela.text и .rela.data          */
/* ------------------------------------------------------------- */
typedef struct {
    uint64_t r_offset; /* смещение в .text или .data */
    uint64_t r_info;   /* ELF64_R_INFO(sym_index, type) */
    int64_t  r_addend; /* addend A (обычно 0) */
} Elf64_Rela;

/* Макросы для r_info и st_info */
#define ELF64_R_SYM(i)      ((i) >> 32)
#define ELF64_R_TYPE(i)     ((i) & 0xffffffff)
#define ELF64_R_INFO(s,t)   (((uint64_t)(s) << 32) + (uint64_t)(t))

#define ELF64_ST_BIND(i)    ((i) >> 4)
#define ELF64_ST_TYPE(i)    ((i) & 0xf)
#define ELF64_ST_INFO(b,t)  (((b) << 4) + ((t) & 0xf))
