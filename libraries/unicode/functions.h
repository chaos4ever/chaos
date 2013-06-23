// Abstract: Function prototypes for the Unicode library.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000, 2007, 2013 chaos development.

#pragma once

extern return_type unicode_ucs2_to_utf8_string(u8 *utf8_string, ucs2_type *ucs2_string, unsigned int max_utf8_bytes);
extern return_type unicode_ucs4_to_utf8_string(u8 *utf8_string, ucs4_type *ucs4_string, unsigned int max_utf8_bytes);
extern return_type unicode_utf8_to_ucs2_string(ucs2_type *ucs2_string, u8 *utf8_string, unsigned int max_ucs2_characters);
extern return_type unicode_utf8_to_ucs4_string(ucs4_type *ucs4_string, u8 *utf8_string, unsigned int max_ucs4_characters);

// FIXME: Consider making these be internal to the unicode library. Do we really need to expose such low-level functionality to our servers & apps? Perhaps we do.
extern return_type unicode_ucs2_to_utf8(u8 *utf8, ucs2_type ucs2);
extern return_type unicode_ucs4_to_utf8(u8 *utf8, ucs4_type ucs4);
extern return_type unicode_utf8_to_ucs2(ucs2_type *ucs2, u8 *utf8, unsigned int *length);
extern return_type unicode_utf8_to_ucs4(ucs4_type *ucs4, u8 *utf8, unsigned int *length);
extern unsigned int unicode_utf8_previous_character_length(char *utf8_string, unsigned int string_position);
extern unsigned int unicode_utf8_next_character_length(u8 *utf8_string, unsigned int string_position);
extern unsigned int unicode_utf8_string_characters(u8 *utf8_string);
