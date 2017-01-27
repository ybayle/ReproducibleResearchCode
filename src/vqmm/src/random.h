
#ifndef _RANDOM_H
#define _RANDOM_H

#include "define.h"

/*######################################################################

     Copyright (C) 1994  Thibault Langlois

     -------------------------------------------------------------------
     Thibault LANGLOIS
     INESC,				     Tel. +351 (1)3100315
     Apartado 13069			     Fax  +351 (1)525843
     1000 Lisboa 			     Email Thibault.Langlois@inesc.pt
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
/* ------------------------------------------------------------------------ */
/* Function that initialize the random generator using the current time.    */
/* ------------------------------------------------------------------------ */
void RandomInitGenerator();
void RandomInitGenerator1(int seed);
/********************************************************************/
/*   Fonction qui rend un nombre au hasard (real) dans              */
/*   l'intervalle [-range , +range].                                */
/********************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that returns a random number.                                   */
/* Argument: bound the number returned is in [-bound, bound].               */
/* ------------------------------------------------------------------------ */
real RandomInterval1(real bound);
/* ------------------------------------------------------------------------ */
/* Function that returns a randomly 1.0 or 0.0.                             */
/* ------------------------------------------------------------------------ */
real RandomBit(void);
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
real RandomInterval2(real min,
		     real max);
/*************************************************************************/
/* RandomNormal():                                                       */
/* Fonction qui rend un real tire au hasard selon une loi normale de     */
/* moyenne nulle et d'ecart-type egal a 1.                               */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that returns a real number according to the normal law N(0,1).  */
/* ------------------------------------------------------------------------ */
real RandomNormal();

#endif




