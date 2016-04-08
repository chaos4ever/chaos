// Abstract: A dvorak translation table for chaos. This probably needs some fixup before it is usable.
// Author: Andreas etterlund <metal_leif@lycos.com>
//
// © Copyright 2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

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
  NULL, NULL, NULL, "@",  "�",  "$",  NULL, NULL,
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
