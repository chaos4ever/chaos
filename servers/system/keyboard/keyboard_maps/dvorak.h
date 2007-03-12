/* $Id$ */
/* Abstract: A dvorak translation table for chaos. This probably
   needs some fixup before it is usable. */
/* Author: Andreas etterlund <metal_leif@lycos.com> */

/* Copyright 2000 chaos development. */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __DVORAK_H__
#define __DVORAK_H__

const char *dvorak_keyboard_map[] =
{
  NULL, NULL, "1",  "2",  "3",  "4",  "5",  "6",
  "7",  "8",  "9",  "0",  "[",  "]",  NULL, NULL,
  "'",  ",",  ".",  "p",  "y",  "f",  "g",  "c",
  "r",  "l",  "/",  "=",  NULL, NULL, "a",  "o",
  "e",  "u",  "i",  "b",  "h",  "t",  "n",  "s",
  "-",  "\\", NULL, "<",  ";",  "q",  "j",  "k",
  "x",  "b",  "m",  "w",  "v",  "z",  NULL, "*",
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, "-",  NULL, NULL, NULL, "+",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, "<",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL
};

const char *dvorak_keyboard_map_shift[] =
{
  NULL, NULL, "!",  "@",  "#",  "$",  "%",  "^",
  "&",  "*",  "(",  ")",  "{",  "}",  NULL, NULL,
  "\"", "<",  ">",  "P",  "Y",  "F",  "G",  "C",
  "R",  "L",  "?",  "+",  NULL, NULL, "A",  "O",
  "E",  "U",  "I",  "D",  "H", "T",   "N",  "S",
  "_",  "|",  NULL, ">",  ":", "Q",   "J",  "K",
  "X",  "B",  "M",  "W",  "V",  "Z",  NULL, "*",
  NULL, " ",  NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, "-",  NULL, NULL, NULL, "+",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, ">",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL
};

const char *dvorak_keyboard_map_altgr[] =
{
  NULL, NULL, NULL, "@",  "£",  "$",  NULL, NULL,
  "{",  "[",  "]",  "}",  "\\", NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, "~",  NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, " ",  NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, "|",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL
};

#endif /* !__DVORAK_H__ */
