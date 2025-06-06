# Структура ELF

## 0. Cборка
cmake -S ./BackEnd -B ./BackEnd/build && cmake --build ./BackEnd/build
cmake -S ./FrontEnd -B ./FrontEnd/build && cmake --build ./FrontEnd/build
./run.sh --AST --graphics ./testing_space/code.mcc
nasm -f elf64 ./code.asm -o code.o
gcc -no-pie code.o -o code.out
./code.out


```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build build
./build/Compiler
```

## 1. ELF header
* e_ident(16) информация для идентификации elf файла
    * [0:3] - `0x7f 0x45 0x4c 0x46` - ELF
    * [4:4] - битность архитектуры (`0x01` - 32 бита, `0x02` - 64 бита)
    * [5:5] - LSB = `0x1`, MSB = `0x2`
    * [6:6] - версия elf. Пока есть только `0x01`
    * [7:15] - заполнители

* e_type(2) - тип elf файла :
    * CORE (значение 4)
    * DYN (Shared object file), библиотека (значение 3)
    * EXEC (Executable file), исполняемый файл (значение 2)
    * REL (Relocatable file), файл до линковки (значение 1)

* e_machine(2)     - код машины, на которой исполняется файл. У меня `0x3e` - AMD_X86_64_MACHINE_CODE (хотя у меня intel. Странно... FIXME:)
* e_version(4) - версия elf. `0x01 0x00 0x00 0x00`
* e_entry(4)       - Точка входа по адресу в `виртуальной памяти`. Начальный адрес виртуальной памяти по модулю размера страницы должен быть равен смещению в файле (p_offset)
* e_phoff(4)       - смещение от файла к program header table. (т.е. размер elf header в байтах)
* e_shoff(4)       - смещение от начала файла к таблице заголовков раздела
* e_flags(4)       - ?????
* e_ehsize(2)      - размер заголовка ELF
* e_phentsize(2)   - размер заголовка программы
* e_phnum(2)       - количество заголовков программы, что напрямую соответствует количеству сегментов в файле

* e_shentsize(2)    -
* e_shnum(2)        - информация для заголовков секций
* e_shstrndx(2)     -

FIXME: чем сегмент отличается от секции?

## 2. Program header table

### 2.1 Segment structure

Данная структура - список данных о каждом сегменте (сами тела сегментов идут после всех header`ов).

* p_type(4)     - тип сегмента
* p_offset(4)   - адресс начала тела сегмента (его смещение относительно начала файла elf в байтах)
* p_vaddr(4)    - `виртуальный` адрес начала тела сегмента. Должен быть равен
* p_paddr(4)    - физическая адресация
* p_filesz(4)   - размер тела
* p_memsz(4)    - количество байтов в памяти образа сегмента
* p_flags(4)    - флаги доступа. `READ - 0x04`, флаг `WRITE - 0x02`, а флаг `EXEC - 0x01`.
* p_align(4)    - выравнивание. Размер страницы обычно составляет 4 КиБ, поэтому значение должно быть 0x1000

Название                        | Размер (в байтах) |
--------------------------------|-------------------|
elf header                      |       52          |
ячейка program header table     |       32          |