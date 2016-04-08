// Abstract: A british translation table for chaos. This probably needs some fixup before it is usable.
// Author: John Hennessy <john@somewhere>
//
// © Copyright 2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

const char *british_keyboard_map[] =
{
  NULL, NULL, "1",  "2",  "3",  "4",  "5",  "6",
  "7",  "8",  "9",  "0",  "-",  "=",  NULL, NULL,
  "q",  "w",  "e",  "r",  "t",  "y",  "u",  "i",
  "o",  "p",  "[",  "]",  NULL, NULL, "a",  "s",
  "d",  "f",  "g",  "h",  "j",  "k",  "l",  ";",
  NULL, "#",  NULL, "\"", "z",  "x",  "c",  "v",
  "b",  "n",  "m",  ",",  ".",  "/",  NULL, "*",
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, "-",  NULL, NULL, NULL, "+",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, "<",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL
};

const char *british_keyboard_map_shift[] =
{
  NULL, NULL, "!",  "\"",  "#",  "$",  "%",  "&",
  "/",  "(",  ")",  "=",  "?",  "`",  NULL, NULL,
  "Q",  "W",  "E",  "R",  "T",  "Y",  "U",  "I",
  "O",  "P",  NULL, "^",  NULL, NULL, "A",  "S",
  "D",  "F",  "G",  "H",  "J",  "K",  "L",  NULL,
  NULL, NULL, NULL, "*",  "Z",  "X",  "C",  "V",
  "B",  "N",  "M",  ";",  ":",  "_",  NULL, "*",
  NULL, " ",  NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, "-",  NULL, NULL, NULL, "+",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, ">",  NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL
};

const char *british_keyboard_map_altgr[] =
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
