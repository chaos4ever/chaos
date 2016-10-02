// Abstract: Function prototypes and structure definitions for ELF support.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/generic/bit.h>
#include <storm/generic/types.h>

// More information about the ELF format can be found in the ELF specification, as published by the Tool Interface Standards (TIS)

// File classes.
enum
{
    // No class. Bad ELF.
    ELF_CLASS_NONE,

    // 32-bit ELF.
    ELF_CLASS_32BIT,

    // 64-bit ELF.
    ELF_CLASS_64BIT,
};

// File types.
enum
{
    // No file type.
    ELF_TYPE_NONE,

    // Relocatable file (i.e. PIC, position independant code)
    ELF_TYPE_RELOCATABLE,

    // Executable file.
    ELF_TYPE_EXECUTABLE,

    // Shared object file.
    ELF_TYPE_DYNAMIC,

    // Core dump.
    ELF_TYPE_CORE,
};

// Machine types.
enum
{
    // No machne.
    ELF_MACHINE_NONE,

    // AT&T WE 32100
    ELF_MACHINE_WE32100,

    // Sparc.
    ELF_MACHINE_SPARC,

    // Intel 80386.
    ELF_MACHINE_386,

    // Motorola 68000.
    ELF_MACHINE_68000,

    // Motorola 88000.
    ELF_MACHINE_88000,

    // Intel 80860.

    ELF_MACHINE_860,

    // MIPS RS3000.
    ELF_MACHINE_RS3000,
};

// ELF versions.
enum
{
    // Invalid version.
    ELF_VERSION_NONE,

    // Current version.
    ELF_VERSION_CURRENT,
};

// Endianness.
enum
{
    // No endian (invalid).
    ELF_ENDIAN_NONE,

    // Big endian.
    ELF_ENDIAN_BIG,

    // Little endian.
    ELF_ENDIAN_LITTLE,
};

// Section types.
enum
{
    // This section is inactive.
    ELF_SECTION_TYPE_NULL,

    // This section is used for something defined by the program itself.
    ELF_SECTION_TYPE_UNDEFINED,

    // This section holds the symbol table. May only exist once in the file.
    ELF_SECTION_TYPE_SYMBOL_TABLE,

    // This section holds the string table. May only exist once in the file.
    ELF_SECTION_TYPE_STRING_TABLE,

    // This section holds relocation entries with explicit addends (?).
    ELF_SECTION_TYPE_RELOCATION_ADDENDS,

    // This section holds a symbol hash table.
    ELF_SECTION_TYPE_SYMBOL_HASH_TABLE,

    // This section holds information for dynamic linking.
    ELF_SECTION_TYPE_DYNAMIC,

    // This section holds information that marks the file in some way.
    ELF_SECTION_TYPE_NOTE,

    // This section does not take up any space in the ELF file. Most often used for BSS sections.
    ELF_SECTION_TYPE_NO_SPACE,

    // This section holds relocation entries without explicit addends.
    ELF_SECTION_TYPE_RELOCATION,

    // This section type is reserved by has unspecified semantics. Programs that contain a section of this type does not conform
    // to the ABI.
    ELF_SECTION_TYPE_SHARED_LIBRARY,
};

// Section flags.
enum
{
    // This section contains data that should be writable during process execution.
    ELF_SECTION_FLAG_WRITE = BIT_VALUE(0),

    // This section occupies memory during process execution.
    ELF_SECTION_FLAG_ALLOCATE = BIT_VALUE(1),

    // This section contains code that should be executable.
    ELF_SECTION_FLAG_EXECUTE = BIT_VALUE(2),
};

// Type definitions.
// An ELF (Executable and linkable format) header.
typedef struct
{
    // Must be 0x7F, 'E, 'L', 'F' in this order.
    uint8_t identification[4];

    // The class of this executable. ELF_CLASS_WHATEVER.
    uint8_t class;

    // The endianess of the data in this file. ELF_ENDIAN_*
    uint8_t endian;

    // Current version is ELF_VERSION_CURRENT.
    uint8_t version;

    // Should be zero. FreeBSD uses those to put 'FreeBSD' in the ELF header. We could do the same...
    uint8_t pad[9];

    // The type of ELF. ELF_TYPE_WHATEVER.
    uint16_t type;

    // The machine type this ELF is designed to run on. ELF_MACHINE_WHATEVER.
    uint16_t machine;

    // Current version is still ELF_VERSION_CURRENT. (Don't ask me why they put the version ID in two places...)
    uint32_t version2;

    // The location of the program entry point.
    uint32_t entry_point;

    // Offset of program header table.
    uint32_t program_header_offset;

    // Offset of section header table.
    uint32_t section_header_offset;
    uint32_t flags;

    // The size of the ELF header.
    uint16_t elf_header_size;

    // The size of a program header table entry.
    uint16_t program_header_entry_size;

    // The number of program header entries.
    uint16_t program_header_entries;

    // The size of a section header table entry.
    uint16_t section_header_entry_size;

    // The number of section header entries.
    uint16_t section_header_entries;

    // The section header table index of the section name string table.

    uint16_t section_string_index;
} PACKED elf_header_type;

// A section header entry.
typedef struct
{
    // The name of the section (index of the string table).
    uint32_t name;
    uint32_t type;
    uint32_t flags;

    // The start of the section in memory.
    uint32_t address;

    // The start of the section in the file.
    uint32_t offset;

    // The size of the section.
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t address_align;

    // The size of each section entry.
    uint32_t entry_size;
} PACKED section_header_type;

return_type elf_execute(uint8_t *image, char *parameter_string, process_id_type *process_id) INIT_CODE;
