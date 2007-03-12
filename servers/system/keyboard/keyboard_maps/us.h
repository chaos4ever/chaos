/* $Id$ */
/* Abstract: A swedish keyboard translation map for chaos. */
/* Author: Andreas Zetterlund <metal_leif@mailcity.com */

/* Copyright 1998-2000 chaos development. */

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

#ifndef __US_H__
#define __US_H__

const char *us_keyboard_map[] =
{
  NULL, NULL, "1",  "2",  "3",  "4",  "5",  "6",
  "7",  "8",  "9",  "0",  "-",  "=",  NULL, NULL,
  "q",  "w",  "e",  "r",  "t",  "y",  "u",  "i",
  "o",  "p",  "[",  "]",  NULL, NULL, "a",  "s",
  "d",  "f",  "g",  "h",  "j",  "k",  "l",  ";",
  "\\",  NULL, NULL, "'",  "z",  "x",  "c",  "v",
  "b",  "n",  "m",  ",",  ".",  "/",  NULL, "*",
  NULL, " ",  NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, "-",  NULL, NULL, NULL, "+",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, "<",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL
};

const char *us_keyboard_map_shift[] =
{
  NULL, NULL, "!",  "@", "#",  "$",  "%",  "^",
  "&",  "*",  "(",  ")",  "_",  "+",  NULL, NULL,
  "Q",  "W",  "E",  "R",  "T",  "Y",  "U",  "I",
  "O",  "P",  "{",  "}",  NULL, NULL, "A",  "S",
  "D",  "F",  "G",  "H",  "J",  "K",  "L",  ":",
  "\"",  NULL, NULL, "|",  "Z",  "X",  "C",  "V",
  "B",  "N",  "M",  "<",  ">",  "?",  NULL, "*",
  NULL, " ",  NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, "-",  NULL, NULL, NULL, "+",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, ">",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL
};

const char *us_keyboard_map_altgr[] =
{
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL
};

#endif /* !__SWEDISH_H__ */
