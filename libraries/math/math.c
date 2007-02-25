/* $Id$ */
/* Abstract: Library providing wrappers to this architecture's FPU
             functions. */
/* Author: Henrik Hallin <hal@chaosdev.org> */

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

#include <math/math.h>

/* Used for unwanted data. */

static double crap;

/* Macro for calculating sin and cos. */

#define SINCOS_CODE \
  long double local_sin; \
  long double local_cos; \
  asm volatile \
    ("fsincos\n \
      fnstsw %%ax\n \
      testl $0x400, %%eax\n \
      jz 1f\n \
      fldpi\n \
      fadd %%st(0)\n \
      fxch %%st(1)\n \
      2:\n \
      fprem1\n \
      fnstsw %%ax\n \
      testl $0x400, %%eax\n \
      jnz 2b\n \
      fstp %%st(1)\n \
      fsincos\n \
      1:" \
     : "=t" (local_cos), "=u" (local_sin) \
     : "0" (angle)); \
  *sin = local_sin; \
  *cos = local_cos;

/* End of macro. */

/* Inlines. */

static inline void sincos_float (float angle, float *sin, float *cos)
{
  SINCOS_CODE;
}

static inline void sincos_double (double angle, double *sin, double *cos)
{
  SINCOS_CODE;
}

static inline void sincos_long_double (long double angle,
                                       long double *sin,
                                       long double *cos)
{
  SINCOS_CODE;
}

/* Exported function. */
/* FIXME: Support other types than double. */

double math_sine (double angle)
{
  double return_value;

  sincos_double (angle, &return_value, &crap);
  return return_value;
}

double math_cosine (double angle)
{
  double return_value;

  sincos_double (angle, &crap, &return_value);
  return return_value;
}
