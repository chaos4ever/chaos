// Abstract: ELF execution library.
// Author: Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1999 chaos development

#include <log/log.h>
#include <system/system.h>
#include <execute_elf/execute_elf.h>

return_type execute_elf(elf_header_type *elf_header, char *parameter_string,
                        process_id_type *child_process_id)
{
    section_header_type *section_header;
    int index;
    uint32_t code_section_base = 0, data_section_base = 0, bss_section_base = 0;
    uint32_t code_section_size = 0, data_section_size = 0, bss_section_size = 0;
    void *code_section_address = NULL;
    void *data_section_address = NULL;
    process_create_type elf_process_create;
    return_type return_value;

    // First of all, make sure this is an ELF image of a supported type.
    if (!(elf_header->identification[0] == 0x7F &&
            elf_header->identification[1] == 'E' &&
            elf_header->identification[2] == 'L' &&
            elf_header->identification[3] == 'F'))
    {
        return EXECUTE_ELF_RETURN_IMAGE_INVALID;
    }

    if (elf_header->class != ELF_CLASS_32BIT)
    {
        return EXECUTE_ELF_RETURN_ELF_UNSUPPORTED;
    }

    if (elf_header->endian != ELF_ENDIAN_BIG)
    {
        return EXECUTE_ELF_RETURN_ELF_UNSUPPORTED;
    }

    if (elf_header->type != ELF_TYPE_EXECUTABLE)
    {
        return EXECUTE_ELF_RETURN_ELF_UNSUPPORTED;
    }

    if (elf_header->version != ELF_VERSION_CURRENT)
    {
        return EXECUTE_ELF_RETURN_ELF_UNSUPPORTED;
    }

    // Well, that was the identification block. Now for the real part of the ELF header...
    // Make sure it is for Intel 80386.
    if (elf_header->machine != ELF_MACHINE_386)
    {
        return EXECUTE_ELF_RETURN_ELF_UNSUPPORTED;
    }

    // Start by finding the size of the eventual BSS section. The first section is used to mark undefined section
    // references, so we start counting from one.
    for (index = 1; index < elf_header->section_header_entries; index++)
    {
        section_header = (section_header_type *)
                         ((uint32_t) elf_header + elf_header->section_header_offset +
                          index * elf_header->section_header_entry_size);

        if (section_header->flags ==
                (ELF_SECTION_FLAG_WRITE | ELF_SECTION_FLAG_ALLOCATE) &&
                section_header->type == ELF_SECTION_TYPE_NO_SPACE)
        {
            bss_section_size = section_header->size;
            bss_section_base = section_header->address;
        }
    }

    // Go through the section header table to find out where to put stuff.
    for (index = 1; index < elf_header->section_header_entries; index++)
    {
        section_header = (section_header_type *)
                         ((uint32_t) elf_header + elf_header->section_header_offset +
                          index * elf_header->section_header_entry_size);

        // Check which type of section this is.
        switch (section_header->flags)
        {
            // .data or .text
            case ELF_SECTION_FLAG_WRITE | ELF_SECTION_FLAG_ALLOCATE:
            case ELF_SECTION_FLAG_ALLOCATE | ELF_SECTION_FLAG_EXECUTE:
            {
                if (section_header->type == ELF_SECTION_TYPE_NO_SPACE)
                {
                    break;
                }

                switch (section_header->flags)
                {
                    // .data
                    case ELF_SECTION_FLAG_WRITE | ELF_SECTION_FLAG_ALLOCATE:
                    {
                        data_section_size = section_header->size;
                        data_section_base = section_header->address;
                        data_section_address = (void *)((uint32_t) elf_header + section_header->offset);

                        break;
                    }

                    // .text
                    case ELF_SECTION_FLAG_ALLOCATE | ELF_SECTION_FLAG_EXECUTE:
                    {
                        code_section_size = section_header->size;
                        code_section_base = section_header->address;
                        code_section_address = (void *)((uint32_t) elf_header + section_header->offset);

                        break;
                    }
                }
                break;
            }
        }
    }

    // Let the kernel create us a process.
    elf_process_create.initial_eip = elf_header->entry_point;
    elf_process_create.process_type = PROCESS_TYPE_SERVER;
    elf_process_create.parameter_string = parameter_string;
    elf_process_create.process_id = child_process_id;
    elf_process_create.code_section_address = code_section_address;
    elf_process_create.code_section_size = code_section_size;
    elf_process_create.code_section_base = code_section_base;
    elf_process_create.data_section_address = data_section_address;
    elf_process_create.data_section_size = data_section_size;
    elf_process_create.data_section_base = data_section_base;
    elf_process_create.bss_section_size = bss_section_size;
    elf_process_create.bss_section_base = bss_section_base;

    return_value = system_call_process_create(&elf_process_create);

    switch (return_value)
    {
        case STORM_RETURN_SUCCESS:
        {
            return EXECUTE_ELF_RETURN_SUCCESS;
        }

        default:
        {
            return EXECUTE_ELF_RETURN_FAILED;
        }
    }
}
