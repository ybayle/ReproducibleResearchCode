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
#include <ctype.h>
#include <sys/types.h>
#ifdef __APPLE__ // mac os x specific
	#include "sys/malloc.h"
#else
	#include <malloc.h>
#endif

#include "define.h"
#include "error.h"
#include "random.h"
#include "matrix.h"


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
/*texi(um)---------------------------------------------------------------
@node  matrand, lae, random, Top
@comment  node-name,  next,  previous,  up
@chapter The @code{matrand} module
This module provides functions for initializing matrix randomly and to
extract random subsets from a matrix. This module requires the following
modules: 
@enumerate
@item @code{matrix}
@item @code{random}
@end enumerate

@deftypefun void MatRandomNormal (matrix * @var{m}, real @var{std})
This function initialize matrix @var{m} wtih random numbers that follow
a normal law centered on 0 with standard deviation equal to @var{std}.
@end deftypefun
texi(end)---------------------------------------------------------------- */
void MatRandomNormal(matrix *M,
		     real  range)
{
        int i,j;
        extern real RandomNormal();

        if (M == NULL) Error("MatRandomNormal","undefined matrix");
        i=0;
        while (i < M->imax)
        {
              j=0;
              while (j < M->jmax)
              {
                    Mat(M,i,j) = RandomNormal() * range; j++;
              }
              i++;
        }
}   
/* ------------------------------------------------------------------------ */
/* Function that initialize a matrix according to e uniform law. Two bounds */
/* are specified. Arguments : (1) the matrix (2) low bound (3) high bound.  */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun void MatRandomUniform2 (matrix * @var{m}, real @var{low}, real @var{high})
Function that initialize matrix @var{m} with uniform random numbers that belong
to the interval [@var{low}, @var{high}]. 
@end deftypefun
texi(end)---------------------------------------------------------------- */
void MatRandomUniform2(matrix *M,
		       real  min,
		       real  max)
{
        int i,j;

        if (M == NULL) Error("MatRandomUniform2","undefined matrix");
        i=0;
        while (i < M->imax)
        {
              j=0;
              while (j < M->jmax)
              {
                    Mat(M,i,j) = RandomInterval2(min,max); j++;
              }
              i++;
        }
}   
/* ------------------------------------------------------------------------ */
/* Function that initialize a matrix according to e uniform law.            */
/* Numbers are in [-bound, bound].                                          */
/* Arguments : (1) the matrix (2) bound.                                    */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun void MatRandomUniform1 (matrix * @var{m}, real @var{r})
This function initialize matrix @var{m} with random numbers according to
a uniform law, in interval [-@var{r}, @var{r}].
@end deftypefun
texi(end)---------------------------------------------------------------- */
void MatRandomUniform1(matrix *M,
		       real  range)
{
        int i,j;

        if (M == NULL) Error("MatRandomUniform1","undefined matrix");
        i=0;
        while (i < M->imax)
        {
              j=0;
              while (j < M->jmax)
              {
                    Mat(M,i,j) = RandomInterval1(range); j++;
              }
              i++;
        }
}   

/* ------------------------------------------------------------------------ */
/* This function fills up the second argument (matrix) with lines of the    */
/* first argument. Lines are chosen randomly and only once.                 */
/* Arguments: (1) the data set (each line is a pattern) (2) the random      */
/* sub-set.                                                                 */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun void RandomSubSet (const matrix * @var{m}, matrix * @var{q})
This function fills up the second matrix @var{q} with lines from the
first natrix @var{m}. Lines are chosen randomly and only once.
@end deftypefun
texi(end)---------------------------------------------------------------- */
void RandomSubSet(const matrix *m,
		  matrix *q)
{
    int i,j,k;
    matrix *p;

    p = MatAlloc(m->imax,1);
    MatFill(p,-1.0);
    for(i=0;i<q->imax;i++)
    {
	do
	    k = (int) RandomInterval2(0.0,m->imax);
	while (Mat(p,k,0) != -1);
	for (j=0;j<m->jmax;j++)
	{
	    Mat(q,i,j) = Mat(m,k,j);
	}
	Mat(p,k,0) = 0.0;
    }
    MatFree(p);
}

/*texi(um)---------------------------------------------------------------
@deftypefun void RandomSubSets (const matrix * @var{m1}, matrix * @var{q1}, const matrix * @var{m2}, matrix * @var{q2})
This function is similar to previous one. It creates a subset of matrix
@var{m1} in matrix @var{q1}. But lines of @var{m2} corresponding (same
indice) to lines chosen in @var{m1} are copied to @var{q2}. This
function is useful when one needs a subset of a set split in two
matrices. For example when @var{m1} and @var{m2} correspond to @code{x}
and @code{f(x)}, this function may be used to extract a subset of the
two matrices.
@end deftypefun
texi(end)---------------------------------------------------------------- */
void RandomSubSets(const matrix *m1,
		   matrix *q,
		   const matrix *m2,
		   matrix *r)
{
    int i,j,k;
    matrix *p;

    p = MatAlloc(m1->imax,1);
    MatFill(p,-1.0);
    for(i=0;i<q->imax;i++)
    {
	do
	    k = (int) RandomInterval2(0.0,m1->imax);
	while (Mat(p,k,0) != -1);
	for (j=0;j<m1->jmax;j++)
	{
	    Mat(q,i,j) = Mat(m1,k,j);
	}
	for (j=0;j<m2->jmax;j++)
	{
	    Mat(r,i,j) = Mat(m2,k,j);
	}
	Mat(p,k,0) = 0.0;
    }
    MatFree(p);
}










