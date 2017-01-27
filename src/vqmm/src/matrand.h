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

#ifndef _MATRAND_H
#define _MATRAND_H 

#include "define.h"


/*************************************************************************/
/* MatRandomNormal():                                                    */
/* Fonction qui initialise une matrix suivant une variable aleatoire     */
/* gaussienne de moyenne nulle et d'ecart-type specifie.                 */
/* Arguments: 1. pointeur sur la matrix (deja allouee).                  */
/*            2. ecart-type.                                             */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that initialize a matrix according to a normal law with mean    */
/* = 0, and sdv specified by the second argument.                           */
/* ------------------------------------------------------------------------ */
void MatRandomNormal(matrix *M,
		     real  range);

/* ------------------------------------------------------------------------ */
/* Function that initialize a matrix according to e uniform law. Two bounds */
/* are specified. Arguments : (1) the matrix (2) low bound (3) high bound.  */
/* ------------------------------------------------------------------------ */
void MatRandomUniform2(matrix *M,
		       real  min,
		       real  max);

/* ------------------------------------------------------------------------ */
/* Function that initialize a matrix according to e uniform law.            */
/* Numbers are in [-bound, bound].                                          */
/* Arguments : (1) the matrix (2) bound.                                    */
/* ------------------------------------------------------------------------ */
void MatRandomUniform1(matrix *M,
		       real  range);

/* ------------------------------------------------------------------------ */
/* This function fills up the second argument (matrix) with lines of the    */
/* first argument. Lines are chosen randomly and only once.                 */
/* Arguments: (1) the data set (each line is a pattern) (2) the random      */
/* sub-set.                                                                 */
/* ------------------------------------------------------------------------ */
void RandomSubSet(const matrix *m,
		  matrix *q);

void RandomSubSets(const matrix *m1,
		   matrix *q,
		   const matrix *m2,
		   matrix *r);

#endif







