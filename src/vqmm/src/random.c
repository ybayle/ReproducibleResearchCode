/* 	@(#)$Id: random.c,v 1.3 1995/03/02 14:50:57 lmn Exp tl $	 */
/* 	Thibault Langlois (tl@inesc.pt)	 */

#ifndef lint
static char vcid[] = "@(#)$Id: random.c,v 1.3 1995/03/02 14:50:57 lmn Exp tl $";
#endif /* lint */
/*
   $Log: random.c,v $
 * Revision 1.3  1995/03/02  14:50:57  lmn
 *  C++ Adaptation.
 *  Added #include <error.h>
 *
 * Revision 1.2  1995/02/22  10:02:02  tl
 * + New function: RandomSubSet.
 * + Now, in function RandomInterval2, if min > max then min and max
 * values are swapped.
 *
*/
/*######################################################################

     Copyright (C) 1995  Thibault Langlois

     -------------------------------------------------------------------
     Thibault LANGLOIS
     INESC,				      Tel. +351 (1)3100315
     Apartado 13069			      Fax  +351 (1)525843
     1000 Lisboa 			      Email Thibault.Langlois@inesc.pt
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

#include <math.h>
#include <sys/time.h>
/*#include <ctype.h>
  #include <sys/types.h>*/
#ifdef __APPLE__ // mac os x specific
	#include "sys/malloc.h"
#else
	#include <malloc.h>
#endif
#include <stdlib.h>

#include "define.h"

/* ------------------------------------------------------------------------ */
/* Function that initialize the random generator using the current time.    */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@node  random, matrand, matrix, Top
@comment  node-name,  next,  previous,  up
@chapter The @code{random} module

This module is a set of functions that generate random numbers in
various ways.  
First of all, here is the function that is used to initialize the random
number generator:

@deftypefun void RandomInitGenerator ()
This function get system's time and uses this number to initialize the
random number generator.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void RandomInitGenerator()
{
    int i;
    i = time(NULL);
    srand(i);
}

void RandomInitGenerator1(int i)
{
    srand(i);
}

/********************************************************************/
/*   Fonction qui rend un nombre au hasard (real) dans              */
/*   l'intervalle [-range , +range].                                */
/********************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that returns a random number.                                   */
/* Argument: bound the number returned is in [-bound, bound].               */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun real RandomInterval1 (real @var{bound})
This function returns a random @code{real} number between -@var{bound}
and +@var{bound} according to a uniform law. If @var{bound} is negative
or zero, 0.0 is retuned.
@end deftypefun
texi(end)---------------------------------------------------------------- */
real RandomInterval1(real bound)
{
      real f;
      
      if (bound <= 0.0) return(0.0);
      f = (real) ((((real) (rand() % 10000) / 10000.0) * 2 * bound) - bound);
      return(f);
}

/*************************************************************************/
/* RandomInterval2():                                                    */
/* Fonction qui rend un real tire au hasard selon une loi uniforme       */
/* sur l'intervale specifie.                                             */
/* Arguments : 1. minimum de l'intervalle.                               */
/*             2. maximum de l'intervalle.                               */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that returns a random (real) number between two bounds: [min,   */
/* max].                                                                    */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun real RandomInterval2 (real @var{low}, real @var{high})
Function that returns a random number between @var{low} and @var{high}
according to a uniform law. If @var{min} is superior to @var{max},
@var{min} and @var{max} are swapped. 
@end deftypefun
texi(end)---------------------------------------------------------------- */
real RandomInterval2(real min,
		     real max)
{
      real f;

      if (max < min)
      {
	  f=min; min=max; max=f;
      }
      if (min == max) return(min);
      f = (rand() % 10000) * ((max - min) / 10000.0) + min;
      return(f);
}

/* ------------------------------------------------------------------------ */
/* Function that returns a randomly 1.0 or 0.0.                             */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun real RandomBit ()
This function returns randomly 0.0 or 1.0, according to a uniform law.
@end deftypefun
texi(end)---------------------------------------------------------------- */
real RandomBit(void)
{
     real f;
     f = RandomInterval1(1.0);
     if (f > 0) return(1.0);
     else return(0.0);
}


/*************************************************************************/
/* RandomNormal():                                                       */
/* Fonction qui rend un real tire au hasard selon une loi normale de     */
/* moyenne nulle et d'ecart-type egal a 1.                               */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that returns a real number according to the normal law N(0,1).  */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun real RandomNormal ()
This function returns a random number according to a normal law with
mean equal to 0 and standard deviation equal to 1.0.
@end deftypefun
texi(end)---------------------------------------------------------------- */
real RandomNormal()
{
     int i;
     real x;
     i=0;x=0.0;
     while (i < 12)
     {
           x = x + RandomInterval1(0.5) + 0.5; i++;
     }
     x = x - 6.0;
     return(x);     
}

#ifdef TEST
int
main(int argc, char ** argv)
{
    real f;
    int i;
    int j;

    for (j=0; j<10; j++)
    {
	RandomInitGenerator();
	printf("Moyenne d'une loi normale centree de variance 1 produite par la\n"
	       "fonction RandomNormal() : ");
	f = 0.0;
	for (i=0; i<1000000; i++)
	{
	    f += RandomNormal();
	}
	printf("%f\n", f/1000000.0);
	
	printf("Moyenne d'une loi uniforme dans l'intervale [-1,1] produite par\n"
	       "la fonction RandomInterval1() : ");
	f = 0.0;
	for (i=0; i<1000000; i++)
	{
	    f += RandomInterval1(1.0);
	}
	printf("%f\n", f/1000000.0);
    }
}
#endif







