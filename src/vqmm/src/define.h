/*######################################################################

     This file contains general purpose definitions.
     Copyright (C) 1994  Thibault Langlois
     
     Version: 1.0
     Date: 29/03/94

     Thibault LANGLOIS
     INESC,					Tel. +351 (1)3100315
     Apartado 13069				Fax  +351 (1)525843
     1000 Lisboa 				Email tl@inesc.pt
     PORTUGAL

     This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License
     as published by the Free Software Foundation; either version 2
     of the License, or (at your option) any later version.
     
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

######################################################################*/

#include <stdio.h>
#include <limits.h>
#include <float.h>

#ifndef DEFINE_H
#define DEFINE_H

#define	MAXDOUBLE	DBL_MAX
#define	MAXFLOAT	FLT_MAX
#define	MINDOUBLE	DBL_MIN
#define	MINFLOAT	FLT_MIN
#define	DMINEXP		DBL_MIN_EXP
#define	FMINEXP		FLT_MIN_EXP
#define	DMAXEXP		DBL_MAX_EXP
#define	FMAXEXP		FLT_MAX_EXP

#define ESC	0x1b			/* Define the escape key	*/
#define TRUE	1			/* Define some handy constants	*/
#define FALSE	0			/* Define some handy constants	*/
#define PI M_PI
/*#define ON	1	*/		/* Define some handy constants	*/
/*#define OFF	0	*/		/* Define some handy constants	*/

/*#define NB_COL 80*/
/*#define NB_LIG 25*/

				/* These macros are usefule for the def- */
#define DOUBLE 1		/* inition of the type real. The macro  */
#define FLOAT 2			/* REAL_NUMBER must be defined to be either */
				/* DOUBLE or FLOAT. */
#define REAL_NUMBER DOUBLE


#if REAL_NUMBER == FLOAT
typedef float real; 
#define FLOAT_DEFINED
#define MAXREAL MAXFLOAT
#define MINREAL MINFLOAT
#endif

#if REAL_NUMBER == DOUBLE
typedef double real;
#define DOUBLE_DEFINED
#define MAXREAL MAXDOUBLE
#define MINREAL MINDOUBLE
#endif

#define sqr(a)  ((a)*(a))
#define bip     putchar(7)
#define PAUSE   getc(stdin)

#define NIL 0

#endif




