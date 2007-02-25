/* $Id$ */
/* Abstract: Function prototypes for the Unicode library. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __LIBRARY_UNICODE_FUNCTIONS_H__
#define __LIBRARY_UNICODE_FUNCTIONS_H__

extern return_type unicode_ucs2_to_utf8_string
  (u8 *utf8_string, ucs2_type *ucs2_string, unsigned int max_utf8_bytes);

extern return_type unicode_ucs4_to_utf8_string
  (u8 *utf8_string, ucs4_type *ucs4_string, unsigned int max_utf8_bytes);

extern return_type unicode_utf8_to_ucs2_string
  (ucs2_type *ucs2_string, u8 *utf8_string, unsigned int max_ucs2_characters);

extern return_type unicode_utf8_to_ucs4_string
  (ucs4_type *ucs4_string, u8 *utf8_string, unsigned int max_ucs4_characters);

extern return_type unicode_ucs2_to_utf8 (u8 *utf8, ucs2_type ucs2);

extern return_type unicode_ucs4_to_utf8 (u8 *utf8, ucs4_type ucs4);

extern return_type unicode_utf8_to_ucs2 (ucs2_type *ucs2, u8 *utf8, 
                                         unsigned int *length);

extern return_type unicode_utf8_to_ucs4 (ucs4_type *ucs4, u8 *utf8,
                                         unsigned int *length);

extern unsigned int unicode_utf8_previous_character_length 
  (char *utf8_string, unsigned int string_position);

extern unsigned int unicode_utf8_next_character_length
  (char *utf8_string, unsigned int string_position);

extern unsigned int unicode_utf8_string_characters (u8 *utf8_string);

#endif /* !__LIBRARY_UNICODE_FUNCTIONS_H__ */
