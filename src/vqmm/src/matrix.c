/* 	$Id: matrix.c,v 1.10 1995/03/08 13:15:50 tl Exp tl $	 */

#ifndef lint
static char vcid[] = "$Id: matrix.c,v 1.10 1995/03/08 13:15:50 tl Exp tl $";
#endif /* lint */
/*
   $Log: matrix.c,v $
 * Revision 1.10  1995/03/08  13:15:50  tl
 * + Problem with MatFrees (variadic arguments) fixed.
 *
 * Revision 1.9  1995/02/28  11:12:42  tl
 * + Small changes to make this file compilable by g++. I was using a
 * variable `class' but C++ did not like... :-}. Anyway, it is not yet
 * C++ compilable.
 *
 * Revision 1.8  1995/02/21  18:08:10  tl
 * + New functions: MatCopyLine. MatRemoveDuplicateRows, MatSwapLines,
 * MatSort, MatPrintRows, MatPutRow, MatPutRowF, MatEqual and MatEqualLines.
 * + Now MatPrint checks if its argument is NULL.
 * + Now MatSqr2 is called MatSqrCol, function MatTrace3 is called
 * MatTransS, function MatMaxCo is called MatMaxColumns, MatMinCo is
 * called MatMinColumns, MatMaxLi is called MatMaxRow and MatMinLi is called
 * MatMinRow.
 * + Functions MatDet, MatCo, inv, MatNormLi, MatNormCo, MatNormalize,
 * MatCR, MatMean, MatStd, MatDist, kppv, firstnn have moved to other
 * module (see index of functions).
 * + Functions RandomInitGenerator, RandomInterval1, RandomBit,
 * RandomInterval2, RandomNormal, MatRandomNormal, MatRandomUniform2 and
 * MatRandomUniform2 have moved to their own module: random.c
 * + Use MAXREAL instead of MAXFLOAT
 * + Now functions MatLoad has new features (and, maybe new bugs :-), it
 * can load octave data files etc...
 *
 * Revision 1.7  1994/10/03  10:10:33  tl
 * + Change of error message in function MatAlloc.
 *
 * Revision 1.6  1994/09/30  13:52:56  tl
 * + The macro ERROR has been replaced by a function Error().
 * + A bug was fixed in MatFrees(). (First matrix of the list was not
 * freed).
 *
 * Revision 1.5  1994/06/30  15:42:35  tl
 * + The main change for this version is the use of a new type "real"
 * instead of type "float". This type is defined in file inc/define.h. It
 * allows to use either float or double changing only the definition of
 * "real". Matrices are saved in files in float format (this may be
 * changed in the future).
 * + A few changes and code cleaning has been made to comply the ansi c
 * standard.
 *
 * Revision 1.4  1994/06/20  13:53:28  tl
 * + New functions: MatEqualLine() and MatAllDiff().
 *
 * Revision 1.3  1994/05/28  17:54:21  tl
 * + Now function MatShift() is called MatShiftUp2(). An other function
 * MatShiftUp() that has only one argument has been created.
 *
*/
/*######################################################################

     This file contains functions for matrix computing.
     Copyright (C) 1994  Thibault Langlois
     
     Version: 1.02    Date: 19/05/94
     New function: MatShift(matrix *M1, matrix *M2) thanks to Ze Amaral !

     Version: 1.01    Date: 03/05/94
     Toward standardization of function names. 
     The function init_rand() is now RandomInitGenerator()
     real my_rand(real range) -> real RandomInterval1(real range)
     real rand_bit(void) -> real RandomBit(void)
     real rand_interval(real min,... -> real RandomInterval2(real min,...
     real rand_normal() -> real RandomNormal()

     Version: 1.0     Date: 29/03/94

     -------------------------------------------------------------------
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

/* ------------------------------------------------------------------------ */
/* matrix.c : Functions for matrix computing. The type "matrix" is defined  */
/* as a structure. An element of a matrix is accessed using a macro :       */
/* Mat(M,i,j), where M is the matrix, i is the line number and j is the     */
/* column number. Vectors are represented with one-column matrices :        */
/* Mat(V,i,0). First line's indice is 0. First column indice is 0.          */
/* Function names have a syntax : <object><action> example : MatCopy(),     */
/* MatPrint(), etc...                                                       */
/* ------------------------------------------------------------------------ */

/*************************************************************************/
/*                                                                       */
/*  matrix.c : Fonctions pour la manipulation de matrices. Le type       */
/*              matrice est defini par une structure. Un element de la   */
/*              matrice est designe a l'aide de la macro Mat(M,i,j).     */
/*              i correspond a la ligne et j a la colonne.               */
/*              Les vecteurs sont des matrices a une colone:             */
/*              Mat(V,i,0).                                              */
/*              La syntaxe des noms de fonction est <objet><action>,     */
/*              par exemple pour les fonctions manipulant des matrices:  */
/*              MatCopy(), MatPrint() etc...                             */
/*                                                                       */
/*  Thibault Langlois                                                    */
/*************************************************************************/


#include <stdio.h>
/*#include <stdlib.h>*/
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
/*  mac os x specific */
#ifdef __APPLE__ 
	#include "sys/malloc.h"
#else
	#include <malloc.h>
#endif
#include <stdarg.h>

#include <limits.h> 
#include <float.h> 

#include "define.h"           /* defines some constants */
#include "error.h"
#include "iof.h"	      /* needs these input/ouput functions */
#include "matrix.h"


			      /* The matrix structure is defined in */
			      /* ../inc/matric.h */
/*
typedef struct
	{
	   int imax;
	   int jmax;
	   real *values;
	} matrix;
*/
			      /* A macro to access values in the matrix */
/*
#define Mat(m,i,j) (*(m->values+((i)*m->jmax)+(j)))
*/

/* ------------------------------------------------------------------------ */
/* This function allocates memory for a matrix. Arguments are number of     */
/* lines and number of columns. A pointer matrix * is returned.             */
/* ------------------------------------------------------------------------ */

/*************************************************************************/
/*                                                                       */
/* Fonction qui rend un pointeur sur une matrice dont la dimension       */
/* correspond aux arguments (lignes,colonnes). L'allocation dynamique    */
/* est faite et les champs imax et jmax sont initialises.                */
/*                                                                       */
/*************************************************************************/

/*texi(um)---------------------------------------------------------------
@node  matrix, random, iof, Top
@comment  node-name,  next,  previous,  up
@chapter The @code{matrix} module

This chapter describes version @value{matrix-version} of module @code{matrix}.
This module brings together functions for basic matrix computation. A
type @code{matrix} is defined, functions for matrix allocation and
de-allocation are provided. Functions for copying matrices and manipulating
parts of a matrix etc...
 
@menu
* Type matrix::                 Definition of type @code{matrix}
* Allocation::                  Constructor and Destructors
* Copy & Manipulation::         Matrix Copy and Manipulation
* Print/Save/Load::             Printing, Saving and Loading
* Comparison::                  Comparison of matrices
* Initialization::              Initialization of matrices
* Arithmetics::                 Arithmetic functions
* Trace & Diagonal::            Trace & Diagonal
* Min & Max::                   Minima and Maxima in matrices
@end menu

texi(end)---------------------------------------------------------------- */

/*texi(um)---------------------------------------------------------------
@node   Type matrix, Allocation, matrix, matrix
@comment  node-name,  next,  previous,  up
@section Type @code{matrix}

@subsection Definition

The type @code{matrix} is defined as a structure that contains two integers
and a pointer on type @code{real} that contains matrix's components.
The two integers @code{imax} and @code{jmax} are matrix's
dimensions. @code{imax} is the number of lines and @code{jmax} the number
of columns. Values of a matrix are accessed with a macro @code{Mat}. Here
is the definition of the type @code{matrix}:

@example
typedef struct s_matrix
	@{
	   int imax;
	   int jmax;
	   real *values;
	@} matrix;
@end example

@defmac Mat (M, i, j)
Let @var{M} be a variable of type @code{matrix *}. An element of the matrix
is accessed with the macro @code{Mat}: @code{Mat(@var{M},@var{i},@var{j})}
gives the element of matrix @var{M} located at line @var{i} and column
@var{j}. Element @code{Mat(@var{M},0,0)} is the the element situated at
first line and first column of the matrix. The definition of the macro is:
@code{#define Mat(m,i,j) (*((m)->values+((i)*(m)->jmax)+(j)))}. 
@end defmac

@subsection Vectors

Vectors are defined as single-column matrices. In the case of vectors, the
third argument is always equal to 0.

texi(end)---------------------------------------------------------------- */

/*texi(um)---------------------------------------------------------------

@node  Allocation, Copy & Manipulation, Type matrix, matrix
@comment  node-name,  next,  previous,  up
@section Allocation

Matrix allocation is made with the function @code{MatAlloc}. Three
functions are provided to free memory associated to a matrix.

@deftypefun {matrix *} MatAlloc (int @var{n}, int @var{m})
This function allocates memory for a @code{matrix} structure and a space
for the contents of the matrix. @var{n} is the number of lines and
@var{m} is the number of columns. Elements of the matrix are initialized
with 0. Here are two examples: 
@example
matrix *M, *V;
M = MatAlloc(4,3);       // Allocation of 4 x 3 matrix. 
V = MatAlloc(5,0);       // Allocation of 5 elements vector. 
@end example
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatAlloc(int imax, int jmax)
{
	matrix *m;
	real *values;
	int i=0,j=0;
	m = (matrix *) malloc(sizeof(matrix));
        if (m == NULL) 
	{
	    printf("Allocation error: matrix %dx%d.\n",imax,jmax);
	    Error("MatAlloc","Not enough space for dynamic allocation 1");
	}
	m->imax = imax; m->jmax = jmax;
 	values = (real *) calloc(imax*jmax,sizeof(real));
	if (values == NULL)
	{
	    printf("Allocation error: matrix %dx%d.\n",imax,jmax);
	    Error("MatAlloc","Not enough space for dynamic allocation");
	}
	m->values = (real *) values;
	while (i < m->imax)
	{
	      while (j < m->jmax)
	      {
		    Mat(m,i,j) = 0.0;
		    j++;
	      }
	      j=0; i++;
	}
	return(m);
}

/*************************************************************************/
/*                                                                       */
/* Fonction qui libere la memoire associee a une matrix.                */
/*                                                                       */
/*************************************************************************/

/* ------------------------------------------------------------------------ */
/* Functions that free memory associated to a matrix (MatFree) or several   */
/* matrices (MatFrees). For MatFrees the last argument must be a 0.         */
/* Caution ! The memory is not cleaned and the pointer remains with the     */ 
/* value.                                                                   */
/* ------------------------------------------------------------------------ */

/*texi(um)---------------------------------------------------------------
@deftypefun void MatFree (matrix * @var{m})
Function that frees the memory associated to a matrix.
@end deftypefun

@deftypefun void MatFree2 (matrix ** @var{m})
This function receives the adress of a pointer on a matrix. It frees the
matrix and sets the pointer @code{*@var{m}} to @code{NULL}.
@end deftypefun

@deftypefun void MatFrees (@{@code{matrix *} @var{m}@}+, @code{0})
This is a version of @code{MatFree} that accept any number of matrix
@var{m} in argument. Each matrice is freed using @code{MatFree}. The
last argument @emph{must} be 0 or @code{NULL}.
@end deftypefun

texi(end)---------------------------------------------------------------- */

void MatFree(matrix *m)
{
    free(m->values);
    free(m);
}

void MatFree2(matrix **m)
{
    free((*m)->values);
    free(*m);
    *m = (matrix *) 0;
}

#define UNIX
#ifdef UNIX
void MatFrees(matrix *m,...)
{
     va_list parptr;
     va_start(parptr,m);
     MatFree(m);
     m = va_arg(parptr, matrix *);
     while (m != 0)
     {
           MatFree(m);
           m = va_arg(parptr, matrix *);
     } 
     va_end(parptr);
}
#endif

#ifdef MSDOS
void MatFrees(m,...)
matrix *m;
{
     va_list parptr;
     va_start(parptr,m);
     MatFree(m);
     m = va_arg(parptr, matrix *);
     while (m != 0)
     {
           MatFree(m);
           m = va_arg(parptr, matrix *);
     }
     va_end(parptr);
}
#endif

/*************************************************************************/
/*                                                                       */
/* Fonction qui copie une matrix.                                       */
/*                                                                       */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function which copies a matrix to an other. Both matrices must be        */
/* allocated. Copies M1 TO M2.                                              */
/* ------------------------------------------------------------------------ */

/*texi(um)---------------------------------------------------------------

@node   Copy & Manipulation, Print/Save/Load, Allocation, matrix
@comment  node-name,  next,  previous,  up
@section Matrix Copy and Manipulation

This section describes functions for copying matrices of part of a
matrix. Various functions that transpose a matrix are provided

@subsection Copy

@deftypefun void MatCopy (matrix * @var{m1}, matrix * @var{m2})
This function copies the contents of matrix @var{m1} into matrix
@var{m2}. Both matrices must have the same size. Both matrices must be
defined (even destination matrix).
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatCopy(matrix *M1, 
	     matrix *M2)
{
        int i,j;

        if ((M1 == NULL) || (M2 == NULL)) Error("MatCopy","undefined matrix");
        if ((M1->imax != M2->imax) || (M1->jmax != M2->jmax))
           Error("MatCopy","matrix with different dimensions");
        i=0;
        while (i < M1->imax)
        {
              j=0;
              while (j < M1->jmax)
              {
                    Mat(M2,i,j) = Mat(M1,i,j); j++;
              }
              i++;
        }
}

/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatCopy2 (matrix * @var{m})
Function that return a copy of matrix @var{m}. A new matrix with same
size than @var{m} is allocated and the contents of @var{m} is copied
into it.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatCopy2(matrix *M)
{
    matrix *CopieDeM;
    int i,j,n,p;
    n = M->imax;
    p = M->jmax;
    CopieDeM = MatAlloc(n,p);
    for(i=0;i<n;i++)
	for(j=0;j<p;j++)
	    Mat(CopieDeM,i,j) = Mat(M,i,j);
    return(CopieDeM);
}

/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */

matrix *
MatDuplicate(matrix *m)
{
    matrix *m2;
    
    m2 = MatAlloc(m->imax, m->jmax);
    MatCopy(m,m2);
    return(m2);
}

/* ------------------------------------------------------------------------ */
/* Function that copies a line from a matrix to another matrix. The line    */
/* may correspond a part of the line of the destination matrix.             */
/* Arguments: (1) source matrix (2) line number in source matrix            */
/* (3) destination matrix (4) line number in destination matrix (5) column  */
/* number to start from in destination matrix.                              */
/* ------------------------------------------------------------------------ */

/*texi(um)---------------------------------------------------------------
@deftypefun void MatCopyLine (matrix * @var{m1}, int @var{line1}, 
            matrix * @var{m2}, int @var{line2}, int @var{fromCol})
This function copies a line from matrix @var{m1} to matrix @var{m2}. The
line may correspond to a part of a line of destination matrix @var{m2}. In this
case @var{fromCol} is different from 0 and contains the column number
from which the data coming from @var{m1} should be placed. @var{line2}
indicates the line number in @var{m2} where data will be copied. 
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatCopyLine(matrix *M1,
		 int lineNo1,
		 matrix *M2,
		 int lineNo2,
		 int fromCol)
{
    int j;
				/* Check arguments */
    if ((M1 == NULL) || (M2 == NULL))
	Error("MatCopyLine","Matrices should be defined");
    if ((lineNo1 < 0) || (lineNo1 >= M1->imax))
	Error("MatCopyLine","First indice out of range");
    if ((lineNo2 < 0) || (lineNo2 >= M2->imax))
	Error("MatCopyLine","Second indice out of range");
    if ((M2->jmax - fromCol) < (M1->jmax))
	Error("MatCopyLine","Destination matrix cannot contain source matrix");

    for(j=0; j<M1->jmax; j++)
    {
	Mat(M2,lineNo2,j+fromCol) = Mat(M1,lineNo1,j);
    }
}

void MatCopyColumn(matrix *M1,
		   int columnNo1,
		   matrix *M2,
		   int columnNo2,
		   int fromLine)
{
    int i;
				/* Check arguments */
    if ((M1 == NULL) || (M2 == NULL))
	Error("MatCopyColumn","Matrices should be defined");
    if ((columnNo1 < 0) || (columnNo1 >= M1->jmax))
	Error("MatCopyColumn","First indice out of range");
    if ((columnNo2 < 0) || (columnNo2 >= M2->jmax))
	Error("MatCopyColumn","Second indice out of range");
    if ((M2->imax - fromLine) < (M1->imax))
	Error("MatCopyColumn","Destination matrix cannot contain source matrix");

    for(i=0; i<M1->imax; i++)
    {
	Mat(M2,i+fromLine,columnNo2) = Mat(M1,i,columnNo1);
    }
}

/* ------------------------------------------------------------------------ */
/* Function that returns a matrix formed by the lines contained in M but    */
/* without repetition of lines.                                             */
/* ------------------------------------------------------------------------ */

/*texi(um)---------------------------------------------------------------
@subsection Swaping lines, Sorting and Shifting

@deftypefun {matrix *} MatRemoveDuplicateRows (matrix * @var{m})
Function that returns a matrix formed by the lines contained in @var{m}
without duplicated lines.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatRemoveDuplicateRows(matrix *M)
{
    int i,j;
    matrix *row;
    matrix *result1;
    matrix *result2;
    int theClass;
    int equal;

    row = MatAlloc(1,M->jmax);
    result1 = MatAlloc(M->imax,1);
    MatFill(result1, -1.0);

    theClass = 1;
    Mat(result1,0,0) = 0.0;

    for(i=1;i<M->imax;i++)
    {
	equal = FALSE;
	for(j=0;j<i;j++)
	{
	    if (MatEqualLine(M,i,j))
		equal = TRUE;
	}
	if (!equal)
	{
	    Mat(result1,theClass,0) = i;
	    theClass++;
	}
    }
    result2 = MatAlloc(theClass,M->jmax);
    for (i=0;i<theClass;i++)
	MatCopyLine(M,(int) Mat(result1,i,0),result2,i,0);
    MatFree(result1);
    return(result2);
}

/* ------------------------------------------------------------------------ */
/* Function that swaps two lines in a matrix.                               */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun void MatSwapLines (matrix * @var{m}, int @var{i1}, int @var{i2})
This function swaps two lines of matrix @var{m}. @var{i1} and @var{i2}
are indices of lines to be swaped. 
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatSwapLines(matrix *M,
		  int i1,
		  int i2)
{
    int j;
    float f;

    for (j=0; j<M->jmax; j++)
    {
	f = Mat(M,i1,j);
	Mat(M,i1,j) = Mat(M,i2,j);
	Mat(M,i2,j) = f;
    }
}

/* ------------------------------------------------------------------------ */
/* Function that sorts the lines of a matrix in increasing order with       */
/* respect of the first colummn.                                            */
/* The algorithm is "bubble sort".                                           */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun void MatSort (matrix * @var{m})
Function that sorts lines of matrix @var{m} in increasing order with
respect to the first column. The algorithm used for sorting is "bubble
sort" and therefore is not the most efficient...
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatSort(matrix *M)
{
    int i;
    int modified;

				/* Check argument */
    if (M == NULL) Error("MatSort","Matrix should be defined");

    do 
    {
	modified = FALSE;
	for(i=0; i<M->imax-1; i++)
	{
	    if(Mat(M,i,0) > Mat(M,i+1,0))
	    {
		MatSwapLines(M,i,i+1);
		modified = TRUE;
	    }
	}
    } while(modified);
}

/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */

void MatSortColumn(matrix *M, const int col)
{
    int i;
    int modified;

				/* Check argument */
    if (M == NULL) Error("MatSort","Matrix should be defined");

    do 
    {
	modified = FALSE;
	for(i=0; i<M->imax-1; i++)
	{
	    if(Mat(M,i,col) > Mat(M,i+1,col))
	    {
		MatSwapLines(M,i,i+1);
		modified = TRUE;
	    }
	}
    } while(modified);
}


/********************************************************************/
/*                                                                  */
/* Function that shifts all the lines of a matrix (M1) one line up  */
/* and changes the last line of this matix (M1) to a new one (M2).  */
/*                                                                  */
/********************************************************************/

/*texi(um)---------------------------------------------------------------
@deftypefun void MatShiftUp (matrix * @var{m})
This function shifts lines of matrix @var{m} one line up. First line of
@var{m} is lost. 
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatShiftUp(matrix *M1)
{
        int i,j,k;

        if ((M1 == NULL)) Error("MatShiftUp","undefined matrix");
        i=0;
	k= M1->imax - 1;
        while (i < k)
        {
              j=0;
              while (j < M1->jmax)
              {
                    Mat(M1,i,j) = Mat(M1,i+1,j); j++;
              }
              i++;
        }
}

/*texi(um)---------------------------------------------------------------
@deftypefun void MatShiftUp2 (matrix * @var{m1}, matrix * @var{m2})
Function that shifts up @var{m1} one line (using @code{MatShiftUp}) and
fills the last line of @var{m1} with the last line of @var{m2}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatShiftUp2(matrix *M1,
		 matrix *M2)
{
    int i,j;
    
    if ((M1 == NULL) || (M2 == NULL)) Error("MatShiftUp2",
					    "undefined matrix");
    if (M1->jmax != M2->jmax)
	Error("MatShiftUp2","matrices with different dimensions");
    MatShiftUp(M1);
    i = M1->imax - 1;
    j=0;
    while (j < M1->jmax)
    {
	Mat(M1,i,j) = Mat(M2,i,j); j++;
    }
}

/*************************************************************************/
/*                                                                       */
/* Fonction donnant la transposee de la matrix M passee en parametre.   */
/* Une nouvelle matrix est cree.                                        */
/*                                                                       */
/*************************************************************************/

/*texi(um)---------------------------------------------------------------
@subsection Transposing

@deftypefun void MatTransS (matrix * @var{m})
This function transpose matrix @var{m}. Lines and columns of @var{m} are
swaped. This function works @emph{only} with squared matrices.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatTransS(matrix *m)
{
	real temp;
	int i=0,j=0;

	if (m->imax != m->jmax)
	{puts("*** Error la transposee est calculee qu'avec des matrixs carrees.");
	printf("    imax %d jmax %d \n",m->imax,m->jmax); }
	while (i < m->imax)
	{
	      j=i+1;
	      while (j < m->jmax)
	      {
		    temp = Mat(m,i,j);
		    Mat(m,i,j) = Mat(m,j,i);
		    Mat(m,j,i) = temp; j++;
	      }
	      j=0; i++;
	}
}

/*texi(um)---------------------------------------------------------------
@deftypefun void MatTrans (matrix ** @var{m})
This function takes as argument the adress of a matrix. It creates a new
matrix which is the transposed of @var{m} and makes the adress of
@var{m} point to this new matrix. The old matrix is destroyed.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatTrans(matrix **m)
{
        matrix *m1;
       
       	m1 = MatTrans2(*m);
	MatFree(*m);
	*m = m1;
}

#define EBUG
/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatTrans2 (matrix * @var{m})
Function that returns a new matrix that contains the transpose of
@var{m}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatTrans2(matrix *m)
{
	matrix *m1;
	int i,j;

	i = m->jmax; 
 	j = m->imax;
#ifdef DEBUG
printf("%d %d \n", i,j);
#endif
	m1 = MatAlloc(i,j); 

	i=0;j=0;
	while (j < m->jmax)
	{
	      i=0;
	      while (i < m->imax)
	      {
		    Mat(m1,j,i) = Mat(m,i,j);
		    i++;
	      }
	      j++;
	}
	return(m1);
}
#undef DEBUG

/*************************************************************************/
/*                                                                       */
/* Affiche la matrix passee en parametre.                               */
/*                                                                       */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that prints a matrix on the standard output.                    */
/* ------------------------------------------------------------------------ */

/*texi(um)---------------------------------------------------------------
@node  Print/Save/Load, Comparison, Copy & Manipulation, matrix
@comment  node-name,  next,  previous,  up
@section Printing, Saving and Loading

@deftypefun void MatPrint (matrix * @var{m})
Function that prints matrix @var{m} contents on @code{stdout}. First the
dimensions of the matrix is printed then each matrix's line is printed
on a separated line.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatPrint(matrix *m)
{
     int i=0,j=0;
     
     if (m != NULL)
     {
	 printf("%dx%d \n",m->imax,m->jmax);
	 while (i < m->imax)
	 {
	     printf("%d: ",i);
	     while (j < m->jmax)
	     {
		 /* printf(" [%p] %5.5e",m->values+(i*m->imax)+j,Mat(m,i,j));*/
		 printf(" %5.5e",Mat(m,i,j));
		 j++;
	     }
	     j=0; i++;
	     printf("\n");
	 }
	 printf("\n");
     }
}

/* ------------------------------------------------------------------------ */
/* Function that prints a line of a matrix on stdout.                       */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun void MatPrintRow (matrix * @var{m}, int @var{i})
Function that prints the @var{i}-th line of matrix @var{m} on
@code{stdout}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatPrintRow(matrix *m,
		  int line)
{
     int i=0,j=0;

     while (j < m->jmax)
     {
	 printf(" %5.5e",Mat(m,line,j));
	 j++;
     }
     printf("\n");
}

/*texi(um)---------------------------------------------------------------
@deftypefun void MatPutRow (FILE * @var{stream}, matrix * @var{m}, int @var{i})
This function prints the @var{i}-th line of matrix @var{m} on steam
@var{stream}. The format used is: @code{"%5.5e"} for each number. 
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatPutRow(FILE *stream,
	       matrix *m,
	       int line)
{
     int i=0,j=0;

     while (j < m->jmax)
     {
	 fprintf(stream," %5.5e",Mat(m,line,j));
	 j++;
     }
     fprintf(stream,"\n");
}

/*texi(um)---------------------------------------------------------------
@deftypefun void MatPutRowF (FILE * @var{stream}, char * @var{format}, matrix * @var{m}, int @var{i})
Function that prints the @var{i}-th line of matrix @var{m} on stream
@var{stream} using format @var{format} for each number. Example:
@example
MatPutRowF(stdout, "%5.2f %%", M, 0)
@end example
prints the first line of @code{M} on @code{stdout} using the specified
format string for each number. The output may be:
@example 
5.00 %  12.34 %  98.76 %
@end example
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatPutRowF(FILE *stream,
		char *format,
		matrix *m,
		int line)
{
     int i=0,j=0;

     while (j < m->jmax)
     {
	 fprintf(stream,format,Mat(m,line,j));
	 j++;
     }
}

/********************************************************************/
/*                                                                  */
/* MatLoad() et MatSave():                                          */
/* Fonctions qui lisent ou ecrivent une matrix dans un fichier     */
/*                                                                  */
/* Arguments : 1. pointeur sur une matrix deja allouee.            */
/*             2. pointeur sur le nom du fichier (char *).          */
/*                                                                  */
/********************************************************************/

/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */

int MatLoad2(matrix *M,
	     char   *filename)
{
     int i,j,stat;
     float f1;
     FILE *F;

     if (M == NULL) 
        Error("MatLoad","undefined matrix.");
     F = FileOpen(filename,"r");
     if (F == NULL) 
     {
        Warning("MatLoad","fichier absent.");
        return(EOF);
     }
     i=0;
     while (i < M->imax)
     {
           j=0;
           while (j < M->jmax)
           {
                 stat = fscanf(F,"%e",&f1);
                 if (stat == EOF) Error("MatLoad, scanf",filename);
                 Mat(M,i,j) = f1; j++;
           }
           i++;
     }
     FileClose(F);
     return(0);
}

#define EBUG
/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */

int GetBlanks(FILE *stream,
	      char *c)
{
    int stat;
    do 
    {
	stat = fscanf(stream,"%c",c);
#ifdef DEBUG
	printf("GB[(%c) %d %d]",*c,*c,stat);
#endif
    } while (((*c == ' ') || (*c == 9)) && (stat == 1));
    return(stat);
}


/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */
#define EBUG
void 
MatFindDimensions(char *fileName,
		  int *nbRows,
		  int *nbCols)
{
    FILE *F;
    char c, last_c;
    int stat;
    int columns;

    *nbCols = 0;
    *nbRows = 0;
    F = FileOpen(fileName,"r");
    if (F == NULL) 
    {
        Error("MatFindDimensions","fichier absent.");
    }
				/* Skip blanks at the begining of line */
    stat = fscanf(F,"%c",&c);
#ifdef DEBUG
    printf("0[(%c) %d %d]",c,c,stat);
#endif
    if ((c == ' ') || (c == 9))
    {
	stat = GetBlanks(F, &c);
    }
    do 
    {
	stat = fscanf(F,"%c",&c);
#ifdef DEBUG
	printf("1[(%c) %d %d]",c,c,stat);
#endif
	if ((c == ' ') || (c == 9))
	{
	    stat = GetBlanks(F, &c);
	    if ((c != 10) && (stat == 1))
		(*nbCols)++;
	}
    } while ((c != 10) && (stat == 1));
    (*nbCols)++;
    (*nbRows)++;
#ifdef DEBUG
    printf("\n%d %d\n",*nbRows,*nbCols);
#endif

    do				/* Find number of lines */
    {
	/* Skip blanks at the begining of line */
	last_c = c;
	stat = fscanf(F,"%c",&c);
				/* Hint for one-digit integers */
	if (((c == ' ') || (c == 9)) && 
	    ((last_c != ' ') && (last_c != 9) && (last_c != 10)))
	    columns = 1;
	else 
	    columns = 0;
				/* --------------------------- */
#ifdef DEBUG
	printf("1.1[(%c) %d %d]",c,c,stat);
#endif
	if ((c == ' ') || (c == 9))
	{
	    stat = GetBlanks(F, &c);
	}
	if (stat == 1)
	{
	    do
	    {
		last_c = c;
		stat = fscanf(F,"%c",&c);
#ifdef DEBUG
		printf("3[(%c) %d %d]",c,c,stat);
#endif
		if ((c == ' ') || (c == 9))
		{
		    stat = GetBlanks(F, &c);
		    if ((c != 10) && (stat == 1))
			columns++;
		}	    
	    } while ((c != 10) && (stat == 1));
	    columns++;
	    if (columns != *nbCols) 
	    {
		printf("columns = %d should be %d\n",columns, *nbCols);
		Error("MatFindDimensions","Wrong number of columns in file");
	    }
	}
	(*nbRows)++;
	last_c = c;
	stat = fscanf(F,"%c",&c);
#ifdef DEBUG
	printf("**5**[(%c) %d %d]",c,c,stat);
#endif
    } while ((c != 10) && (stat == 1));
    FileClose(F);
#ifdef DEBUG
    printf("\nRows: %d Cols: %d\n",*nbRows,*nbCols);
#endif
    if ((*nbCols <= 0) || (*nbRows <= 0))
	Error("MatFindDimensions","Bad file format.");
}
#undef DEBUG

/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */

int OctaveFormat(char *filename)
{
    char c;
    FILE *F;
    int stat;

    F = FileOpen(filename,"r");
    stat = fscanf(F,"%c",&c);
    FileClose(F);
    if (c == '#')
    {
	return(TRUE);
    }
    return(FALSE);
}

/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */

void MatFindOctaveDimensions(FILE *F,
			    int *nbRows,
			    int *nbCols)
{
    char c;
    char line[1000];
    char * l;
    char s1[100], s2[100], s3[100];
    int stat;

    l = fgets(line, 999, F);
    stat = fscanf(F, "%s %s %s", s1, s2, s3);
#ifdef DEBUG
    printf("%s %s %s\n", s1, s2, s3);
#endif
    stat = fscanf(F, "%s %s %s", s1, s2, s3);
#ifdef DEBUG
    printf("%s %s %s\n", s1, s2, s3);
#endif
    stat = fscanf(F, "%s %s %d", s1, s2, nbRows);
#ifdef DEBUG
    printf("%s %s %d\n", s1, s2, *nbRows);
#endif
    stat = fscanf(F, "%s %s %d", s1, s2, nbCols);
#ifdef DEBUG
    printf("%s %s %d\n", s1, s2, *nbCols);
#endif
}

#define EBUG
/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatLoad (matrix * @var{m}, char *@var{file-name})
This function reads a matrix in file @var{file-name}.
The argument @var{m} may or may not be allocated. The file may be in
@code{Octave} format or @sc{ascii} format.

@itemize @bullet
@item
If @var{m} is not allocated
it @emph{must} be equal to @code{NULL}. In this case, the
function finds the dimensions of the matrix stored in @var{file-name}. A
new matrix is allocated filled with file's contents and @strong{returned}. When
file is in @code{Octave} format, the header is read and dimension
described in file are used to create a new matrix. 
@item
If matrix @var{m} is already allocated, file @var{file-name} is read and
matrix @var{m} is filled-up with values read from file.
The value returned is @code{NULL}. If @var{file-name} is in
@code{Octave} format, matrix  dimensions are read from file and compared
to matrix @var{m}'s dimension. If they are different, the @code{Error}
function is called.
@end itemize
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatLoad(matrix *M,
		char   *filename)
{
     int i,j,stat;
     float f1;
     FILE *F;
     int rows,cols;
     int newMatrix = FALSE;
				/* Is it an octave file ? */
     if (OctaveFormat(filename))
     {
#ifdef DEBUG
	 puts("file is octave");
#endif
	 F = FileOpen(filename,"r");
	 if (F == NULL) 
	 {
	     Warning("MatLoad","fichier absent.");
	     return(NULL);
	 }

	 MatFindOctaveDimensions(F, &rows, &cols);
	 if (M != NULL)
	 {
	     if ((rows != M->imax) || (cols != M->jmax))
	     {
		 fprintf(stderr, "Reading octave format, dimensions in file (%d x %d) are\n"
		   "different from dimensions in input matrix (%d x %d)\n",
			rows,cols,M->imax,M->jmax);
		 Error("MatLoad", "Octave format different dimensions");
	     }
	 }
	 else
	 {
	     M = MatAlloc(rows,cols);
	 }
	 newMatrix = TRUE;
     }
     else
     {
#ifdef DEBUG
     puts("Not an octave file");
#endif
	 if (M == NULL)		/* Matrix is not allcated so, */
	 {			/* find dimensions and allocate. */
	     /* Warning("MatLoad","undefined matrix");*/
	     MatFindDimensions(filename, &rows, &cols);
#ifdef DEBUG
	     printf("Dimensions %d %d \n", rows, cols);
#endif
	     M = MatAlloc(rows, cols);
	     newMatrix = TRUE;
	 }

	 F = FileOpen(filename,"r");
	 if (F == NULL) 
	 {
	     Warning("MatLoad","fichier absent.");
	     return(NULL);
	 }
     }
     i=0;
     while (i < M->imax)
     {
           j=0;
           while (j < M->jmax)
           {
                 stat = fscanf(F,"%e",&f1);
                 /* printf("%f ", f1); */
                 if (stat == EOF) Error("MatLoad, scanf",filename);
                 Mat(M,i,j) = f1; 
		 j++;
           }
           i++;
           /* printf("\n"); */
     }
     FileClose(F);
     if (newMatrix) return(M);
     else return(0);
}

/********************************************************************/
/*                                                                  */
/* Fonction qui ecrit une matrix dans un fichier.                  */
/*                                                                  */
/********************************************************************/

/*texi(um)---------------------------------------------------------------
@deftypefun void MatSave (matrix * @var{m}, char * @var{file-name})
Function that saves matrix @var{m} contents in file @var{file-name} in
@sc{ascii} format.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatSave(matrix *M,
	     char   *filename)
{
     int i,j,stat;
     float f1;
     FILE *F;

     if (M == NULL) Error("MatSave","undefined matrix");
     F = FileOpen(filename,"w");
     i=0;
     while (i < M->imax)
     {
           j=0;
           while (j < M->jmax)
           {
                 f1 = (float) Mat(M,i,j);
                 stat = fprintf(F,"%e\t",f1);
                 if (stat < 0) Error("MatSave, fprintf",filename);
                 j++;
           }
           fprintf(F,"\n");
           i++;
     }
     FileClose(F);
}

/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */

void MatSaveOctave(matrix *M,
		   char *name,
		   char   *filename)
{
     int i,j,stat;
     float f1;
     FILE *F;

     if (M == NULL) Error("MatSaveOcatve","undefined matrix");
     F = FileOpen(filename,"w");
     fprintf(F,"# name: %s\n",name);
     fprintf(F,"# type: matrix\n# rows %d\n",M->imax);
     fprintf(F,"# columns: %d\n",M->jmax);
     i=0;
     while (i < M->imax)
     {
           j=0;
           while (j < M->jmax)
           {
                 f1 = (float) Mat(M,i,j);
                 stat = fprintf(F,"%e\t",f1);
                 if (stat < 0) Error("MatSave, fprintf",filename);
                 j++;
           }
           fprintf(F,"\n");
           i++;
     }
     FileClose(F);
}

/********************************************************************/
/*                                                                  */
/* MatPut() et MatGet():                                            */
/*   Fonctions ecrivent et lisent une matrix dans le fichier.      */
/*   Arguments : 1. pointeur sur un fichier ouvert.                 */
/*               2. pointeur sur une matrix.                       */
/*                                                                  */
/********************************************************************/
/*texi(um)---------------------------------------------------------------
@deftypefun void MatPutF (FILE * @var{stream}, matrix *@var{m}, char * @var{format})
This function prints matrix @var{m} elements on stream @var{stream}
using format @var{format} for each element. The format string
@emph{must} contain a separator.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatPutF(FILE   *F,
	     matrix *M,
	     char *format)
{
     int i,j;
     float f;

     if (M == NULL) Error("MatPut","undefined matrix");
     i=0;
     while (i < M->imax)
     {
           j=0;
           while (j < M->jmax)
           {
                 f = (float) Mat(M,i,j);
                 fprintf(F,format,f);    /* "\t%e" */
                 j++;
           }
           fprintf(F,"\n");
           i++;
     }
}

/*texi(um)---------------------------------------------------------------
@deftypefun void MatPut (FILE * @var{stream}, matrix * @var{var})
Function that prints matrix @var{m} elements on stream @var{stream}
using format @code{"\t%e"}. It is equavalent to
@code{MatPutF(@var{stream},@var{m},"\t%e")}. 
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatPut(FILE *F,
	    matrix *M)
{
    MatPutF(F,M,"\t%e");
}

/*texi(um)---------------------------------------------------------------
@deftypefun void MatGet (FILE * @var{stream}, matrix * @var{m})
This function read input stream @var{stream} and fills up matrix
@var{m}. The function reads one line after another. Numbers in input
stream @var{stream} @emph{must} be in @sc{ascii} format.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatGet(FILE   *F,
	    matrix *M)
{
  int i,j,stat;
     float f;

     if (M == NULL) Error("MatGet","undefined matrix");
     i=0;
     while (i < M->imax)
     {
           j=0;
           while (j < M->jmax)
           {
                 stat = fscanf(F,"%e",&f);
                 Mat(M,i,j) = f; j++;
           }
           i++;
     }
}



/*texi(um)---------------------------------------------------------------
@node  Comparison, Initialization, Print/Save/Load, matrix
@comment  node-name,  next,  previous,  up
@section Matrix Comparison

@deftypefun int MatEqual (matrix * @var{m1}, matrix * @var{m2})
This function returns @code{TRUE} if matrices @var{m1} and @var{m2} are
equal otherwise returns @code{FALSE}.
@end deftypefun
texi(end)---------------------------------------------------------------- */


int MatEqual(matrix *M1,
	     matrix *M2)
{
    int i1,i2,j;
    int equalLine=TRUE;

    for (i1=0; i1<1; i1++)
    {
	for (i2=0; (i2<M2->imax) && (!equalLine); i2++)
	{
	    equalLine = TRUE;
	    for (j=0; (j < M1->jmax) && (equalLine); j++)
	    {
		if (Mat(M1,i1,j) != Mat(M2,i2,j))
		    equalLine = FALSE;
	    }
	}
    }
    return(equalLine);
}

/*
----------------------------------------------------------------------
Function that returns TRUE if two lines (indiced by i1 and i2) of the
matrix m are equal. Returns FALSE otherwise.
---------------------------------------------------------------------- */
/*texi(um)---------------------------------------------------------------
@deftypefun int MatEqualLine (matrix * @var{m}, int @var{i1}, int @var{i2}) 
Function that checks if two lines of matrix @var{m} are equal. Line
numbers are given by @var{i1} and @var{i2}. Values returned are
@code{TRUE} if lines are equal and @code{FALSE} otherwise.
@end deftypefun
texi(end)---------------------------------------------------------------- */

int MatEqualLine(matrix *m,
		 int    i1,
		 int    i2)
{
    int j;
    int ans;

    ans = TRUE;
    for (j=0;j<m->jmax;j++)
	if (Mat(m,i1,j) != Mat(m,i2,j)) ans = FALSE;
    return(ans);
}
/* ------------------------------------------------------------------------ */
/* Idem but with two different matrices.                                    */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun int MatEqualLines (matrix * @var{m1}, int @var{i1}, matrix * @var{m2}, int @var{i2}) 
This function checks if the @var{i1}-th line of matrix @var{m1} is equal
to the @var{i2}-th line of matrix @var{m2}. The value returned is either
@code{TRUE} if lines are equal either @code{FALSE} if their are not.
@end deftypefun
texi(end)---------------------------------------------------------------- */

int MatEqualLines(matrix *m1,
		  int    i1,
		  matrix *m2,
		  int    i2)
{
    int j;
    int ans;

    ans = TRUE;
    for (j=0;j<m1->jmax;j++)
	if (Mat(m1,i1,j) != Mat(m2,i2,j)) ans = FALSE;
    return(ans);
}

/*
----------------------------------------------------------------------
Function that returns TRUE if all lines of matrix m are
different from each other. Returns FALSE otherwise.
---------------------------------------------------------------------- */

/*texi(um)---------------------------------------------------------------
@deftypefun int MatAllDiff (matrix * @var{m})
Function that returns @code{TRUE} if all lines of matrix @var{m} are
different from each other. Otherwise returns @code{FALSE}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

int MatAllDiff(matrix *m)
{
    int i,k;
    int rep;

    rep = TRUE;
    for (i=0;((i<m->imax) && (rep == TRUE));i++)
    {
	for (k=i+1;((k<m->imax) && (rep == TRUE));k++)
	{
	    rep = TRUE;
	    if (MatEqualLine(m,i,k))
		rep = FALSE;
	}
    }
    return(rep);
}

/********************************************************************/
/*                                                                  */
/* Fonction qui initialise tous les elements de la matrix M a la   */
/* valeur f.                                                        */
/*                                                                  */
/********************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that fills a matrix with a real value.                         */
/* ------------------------------------------------------------------------ */

/*texi(um)---------------------------------------------------------------
@node  Initialization, Arithmetics, Comparison, matrix
@comment  node-name,  next,  previous,  up
@section Initialization

@deftypefun void MatFill (matrix * @var{m}, real @var{f})
Function that fills up the matrix @var{m} with the real number @var{f}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatFill(matrix *M,
	     real  f)
{
     int i,j;
     i=0;
     if (M == NULL) Error("fill","undefined matrix");
     while (i < M->imax)
     {
           j=0;
           while (j < M->jmax)
           {
                 Mat(M,i,j) = f; j++;
           }
           i++;
     }
}

/********************************************************************/
/*                                                                  */
/* Fonction qui met a zero tous les elements de la matrix M.       */
/*                                                                  */
/********************************************************************/
/* ------------------------------------------------------------------------ */
/*  Function that fills a matrix with zeros.                                */
/* ------------------------------------------------------------------------ */

/*texi(um)---------------------------------------------------------------
@deftypefun void MatZeros (matrix * @var{m})
This function sets matrix @var{m} elements to zeros.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatZeros(matrix *M)
{
      MatFill(M,0.0);
}


/*************************************************************************/
/*                                                                       */
/* Addition de deux matrixs.                                            */
/*                                                                       */
/*************************************************************************/
/*texi(um)---------------------------------------------------------------
@node  Arithmetics, Trace & Diagonal, Initialization, matrix
@comment  node-name,  next,  previous,  up
@section Arithmetics

This section describe basic arithmetic functions with matrices. They are
divided in four categories: Addition, Subtraction, Multiplication and
functions that apply another function to elements of a matrix. In each
category, two kinds of operation are described: 
functions that operate on matrices and functions that operate on
scalar and matrix. Sometimes two versions of the same function
co-exist. One version, of type @code{void} return the result in one of
its argument and do not perform memory allocation. The other version,
of type @code{matrix *} allocates memory for the result and returns a
pointer. Function that allocate memory for the result have a name that
end with "2". 

@menu
* Addition::                    Addition of matrices, of matrices and scalars
* Subtraction::                 Subtraction of matrices, of matrices & scalars
* Multiplication::              Multiplication of matrices and friends
* Apply::                       Apply a Function to elements of a matrix
@end menu

@node  Addition, Subtraction, Arithmetics, Arithmetics
@comment  node-name,  next,  previous,  up
@subsection Addition

One function is missing in this category (@code{matrix *
MatAddScal2}). If you need it, do not hesitate: contribute !

@deftypefun void MatAdd (matrix * @var{m1}, matrix * @var{m2})
Function that adds matrix @var{m1} and matrix @var{m2}. The result is
placed in matrix @var{m1}. Both matrices must have same size.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatAdd(matrix *M1,
	    matrix *M2)
{
        int i,j;

        if ((M1 == NULL) || (M2 == NULL)) Error("plus","undefined matrix");
        if ((M1->imax != M2->imax) || (M1->jmax != M2->jmax))
           Error("plus","matrixs de dimensions differentes");
        i=0;
        while (i < M1->imax)
        {
              j=0;
              while (j < M1->jmax)
              {
                    Mat(M1,i,j) = Mat(M1,i,j) + Mat(M2,i,j); j++;
              }
              i++;
        }
}

/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatAdd2(matrix * @var{m1}, matrix * @code{m2})
Function that returns a new matrix result of the addition of matrix
@var{m1} and @var{m2}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatAdd2(matrix *M1,
		matrix *M2)
{
        int i,j;
        matrix *M;

        if ((M1 == NULL) || (M2 == NULL)) Error("MatAdd2","undefined matrix");
        if ((M1->imax != M2->imax) || (M1->jmax != M2->jmax))
           Error("MatAdd2","matrixs de dimensions differentes");
        M = MatAlloc(M1->imax,M1->jmax);
        i=0;
        while (i < M->imax)
        {
              j=0;
              while (j < M->jmax)
              {
                    Mat(M,i,j) = Mat(M1,i,j) + Mat(M2,i,j); j++;
              }
              i++;
        }
        return(M);
}

/*texi(um)---------------------------------------------------------------
@deftypefun void MatAddScal (real @var{f} , matrix * @var{m})
This function adds a scalar @var{f} to every elements  of matrix @var{m}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatAddScal(real  f,
		matrix *M2)
{
        int i,j;

        if ((M2 == NULL)) Error("MatAddScal","undefined matrix");
        i=0;
        while (i < M2->imax)
        {
              j=0;
              while (j < M2->jmax)
              {
                    Mat(M2,i,j) += f; j++;
              }
              i++;
        }
}

/*texi(um)---------------------------------------------------------------
@node  Subtraction, Multiplication, Addition, Arithmetics
@comment  node-name,  next,  previous,  up
@subsection Subtraction

Several functions are missing in this category (@code{void MatSubScal},
@code{void MatSub}). I'm not sure they are useful... Everything can be
done with functions @code{MatAdd} and friends. Anyway, if you need them,
do not hesitate, contribute !

@deftypefun {matrix *} MatSubScal2 (real @var{x}, matrix * @var{m})
Function that returns a new matrix whose elements are the substraction
of elements of matrix @var{m} and scalar @var{x}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatSubScal2(real  f,
		    matrix *M2)
{
        int i,j;
        matrix *M;

        if ((M2 == NULL)) Error("MatSub2","undefined matrix");
        M = MatAlloc(M2->imax,M2->jmax);
        i=0;
        while (i < M->imax)
        {
              j=0;
              while (j < M->jmax)
              {
                    Mat(M,i,j) = f - Mat(M2,i,j); j++;
              }
              i++;
        }
        return(M);
}


/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatSub2 (matrix * @var{m1}, matrix * @var{m2})
This function return new matrix that correspond to the substraction of
matrices @var{m1} and @var{m2}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatSub2(matrix *M1,
		matrix *M2)
{
        int i,j;
        matrix *M;

        if ((M1 == NULL) || (M2 == NULL)) Error("MatSub2","undefined matrix");
        if ((M1->imax != M2->imax) || (M1->jmax != M2->jmax))
           Error("MatSub2","matrixs de dimensions differentes");
        M = MatAlloc(M1->imax,M1->jmax);
        i=0;
        while (i < M->imax)
        {
              j=0;
              while (j < M->jmax)
              {
                    Mat(M,i,j) = Mat(M1,i,j) - Mat(M2,i,j); j++;
              }
              i++;
        }
        return(M);
}

/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */

void
MatSub3(matrix *M1,
	matrix *M2,
	matrix *M3)
{
        int i,j;

        if ((M1 == NULL) || (M2 == NULL) || (M3 == NULL)) 
	    Error("MatSub3","undefined matrix");
        if ((M1->imax != M2->imax) || (M1->jmax != M2->jmax))
           Error("MatSub3","matrix with incompatible dimensions");
        i=0;
        while (i < M3->imax)
        {
              j=0;
              while (j < M3->jmax)
              {
                    Mat(M3,i,j) = Mat(M1,i,j) - Mat(M2,i,j); j++;
              }
              i++;
        }
}


/*************************************************************************/
/*                                                                       */
/* Multiplication d'une matrix par un scalaire.                         */
/* Arguments : reel, pointeur sur la matrix.                            */
/*                                                                       */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that multiplies each element of a matrix by a real value.      */
/* ------------------------------------------------------------------------ */

/*texi(um)---------------------------------------------------------------
@node  Multiplication, Apply, Subtraction, Arithmetics
@comment  node-name,  next,  previous,  up
@subsection Multiplication

Here, an additional function (@code{MatMultt2}) which performs a
term-by-term multiplication of two matrices is provided.

Also, you will find a function that multiplies a vector by himself and a
function that does the same for a matrix's column.

@deftypefun void MatMulScal (real @var{x}, matrix * @var{m})
Function that multiplies each element of matrix @var{m} with scalar
@var{x}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatMulScal(real  x, 
		matrix *m)
{
     int i=0, j=0;
     while (i < m->imax)
     {
	   while (j < m->jmax)
	   {
		 Mat(m,i,j) = x * Mat(m,i,j); j++;
	   }
	   i++; j=0;
     }
}

/* ------------------------------------------------------------------------ */
/* Idem but a new matrix is created and returned.                           */
/* ------------------------------------------------------------------------ */

/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatMulScal2 (real @var{x}, matrix * @var{m})
This function returns a new matrix whose elements are the result of the
multiplication of elements of matrix @var{m} with scalar @var{x}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatMulScal2(real  x, 
		    matrix *m)
{
     int i=0, j=0;
     matrix *m1;
     m1 = MatAlloc(m->imax,m->jmax);
     while (i < m->imax)
     {
	   while (j < m->jmax)
	   {
		 Mat(m1,i,j) = x * Mat(m,i,j); j++;
	   }
	   i++; j=0;
     }
     return(m1);
}

/* ------------------------------------------------------------------------ */

/*************************************************************************/
/*                                                                       */
/* Multiplication de deux matrixs passees en parametre (pointeurs).     */
/* Rend un pointeur sur la matrix resultat.                             */
/*                                                                       */
/*************************************************************************/

/*texi(um)---------------------------------------------------------------
@deftypefun void MatMul (matrix * @var{m3}, matrix * @var{m1}, matrix * @var{m2})
This function multiplicates matrices @var{m1} and @var{m2} and place the
result in @var{m3}. @var{m3} should be previously allocated and have the
correct dimension to contain the result.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatMul(matrix *M3,
	    matrix *M1,
	    matrix *M2)
{
      int i=0,j=0,k=0;
      int imax1,jmax1,jmax2;

      imax1 = M1->imax;
      jmax1 = M1->jmax;
      jmax2 = M2->jmax;
      if ((M3->imax != M1->imax) || (M3->jmax != M2->jmax))
         Error("MatMul","matrix de dimension incorrecte");
      while (i < imax1)
      {
	    while (j < jmax2)
	    {
		  Mat(M3,i,j) = 0;
		  while (k < jmax1)
		  {
			Mat(M3,i,j) += Mat(M1,i,k) * Mat(M2,k,j);
			k++;
		  }
		  j++;k=0;
	    }
	    i++;j=0;
      }
}

/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatMul2 (matrix * @var{m1}, matrix * @var{m2}) 
Function that returns a new matrix which contains the result of the
multiplication of @var{m1} by @var{m2}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatMul2(matrix *M1, 
		matrix *M2)
{
      matrix *M3;
      int i=0,j=0,k=0;
      int imax1,jmax1,jmax2;

      imax1 = M1->imax;
      jmax1 = M1->jmax;
      jmax2 = M2->jmax;
      M3 = MatAlloc(imax1,jmax2);
      while (i < imax1)
      {
	    while (j < jmax2)
	    {
		  Mat(M3,i,j) = 0;
		  while (k < jmax1)
		  {
			Mat(M3,i,j) += Mat(M1,i,k) * Mat(M2,k,j);
			k++;
		  }
		  j++;k=0;
	    }
	    i++;j=0;
      }
      return(M3);
}

/*************************************************************************/
/*                                                                       */
/* Fonction qui multiplie deux matrixs termes a termes.                 */
/*                                                                       */
/*************************************************************************/
/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatMultt2 (matrix * @var{m1}, matrix * @var{m2})
This function performs a term-by-term multiplication of matrices
@var{m1} and @var{m2}. Matrices @emph{must} have the same size. Returns a
new matrix that contain the result. 
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatMultt2(matrix *M1,
		  matrix *M2)
{
        int i,j;
        matrix *M;

        if ((M1 == NULL) || (M2 == NULL)) Error("kdjghdlfkuh","undefined matrix");
        if ((M1->imax != M2->imax) || (M1->jmax != M2->jmax))
           Error("kdjghdlfkuh","matrixs de dimensions differentes");
        M = MatAlloc(M1->imax,M1->jmax);
        i=0;
        while (i < M->imax)
        {
              j=0;
              while (j < M->jmax)
              {
                    Mat(M,i,j) = Mat(M1,i,j) * Mat(M2,i,j); j++;
              }
              i++;
        }
        return(M);
}
/********************************************************************/
/*                                                                  */
/* Fonction qui multiplie un vecteur par lui meme v * transpose(v)  */
/* et rend le resultat.                                             */
/*                                                                  */
/********************************************************************/

/*texi(um)---------------------------------------------------------------
@deftypefun real MatSqr (matrix * @var{m})
This function take its argument @var{m} as a matrix and returns the sum
of the squared elements of this matrix. Value  
returned: @code{= sum_i sum_j m_ij ^2}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

real MatSqr(matrix *M)
{
      int i,j;
      real c = 0;

      if (M == NULL) Error("MatSqr","undefined matrix");
      for (i=0; i < M->imax; i++)
	  for (j=0; j < M->jmax; j++)
	  { 
	      c += Mat(M,i,j) * Mat(M,i,j); 
	  }
      return(c);
}

/*texi(um)---------------------------------------------------------------
@deftypefun real MatSqrCol (matrix * @var{m}, int @var{j})
This function returns the sum of the squared elements of column @var{j}
in matrix @var{m}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

real MatSqrCol(matrix *M,
	       int    j)
{
      int i=0;
      real c = 0;
      if (M == NULL) Error("MatSqr2","undefined matrix");
      while (i < M->imax)
      {
            c += Mat(M,i,j) * Mat(M,i,j); i++;
      }
      return(c);
}

/*texi(um)---------------------------------------------------------------
@node  Apply,  , Multiplication, Arithmetics
@comment  node-name,  next,  previous,  up
@subsection Apply a Function

@deftypefun void MatApply (double (* @var{f})(), matrix * @var{m})
This function applies function @var{f} to each matrix @var{m}
element. For example:
@example
MatApply(exp, M)
@end example
applies function @code{exp} to each element of @code{M}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void MatApply(double  (* f)(real),
	       matrix *M)
{
        int i,j;

        if (M == NULL) Error("MatApply","undefined matrix");
        i=0;
        while (i < M->imax)
        {
              j=0;
              while (j < M->jmax)
              {
                    Mat(M,i,j) = f(Mat(M,i,j)); j++;
              }
              i++;
        }
} 

/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatApply2 (double (* @var{f})(), matrix * @var{m})
This function does applies function @var{f} to elements of @var{m}. The
result is placed 
in a new matrix that is returned. Matrix @var{m} remains unchanged.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatApply2(double  (* f)(real),
		  matrix *M)
{
        int i,j;
        matrix *M1;

        if (M == NULL) Error("MatApply2","undefined matrix");
        M1 = MatAlloc(M->imax,M->jmax);
        i=0;
        while (i < M->imax)
        {
              j=0;
              while (j < M->jmax)
              {
                    Mat(M1,i,j) = f(Mat(M,i,j)); j++;
              }
              i++;
        }
        return(M1);
}         

/*************************************************************************/
/*                                                                       */
/* Fonction donnant la trace de la matrix M passee en parametre.        */
/* M doit etre carree.                                                   */
/*                                                                       */
/*************************************************************************/

/*texi(um)---------------------------------------------------------------
@node  Trace & Diagonal, Min & Max, Arithmetics, matrix
@comment  node-name,  next,  previous,  up
@section Trace & Diagonal

@deftypefun real MatTrace (matrix * @var{m})
Function that returns the trace of matrix @var{m}. The matrix
@emph{must} be square.
@end deftypefun
texi(end)---------------------------------------------------------------- */

real MatTrace(matrix *m)
{
      int i=0,j=0;
      real t=0.0;

      if (m->imax != m->jmax)
      {puts("*** Error la trace est calculee qu'avec des matrixs carrees.");
       printf("    imax %d jmax %d \n",m->imax,m->jmax); exit(0);
      }
      while (i < m->imax)
      {
	    t += Mat(m,i,j);
	    i++; j++;
      }
      return(t);
}

/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatDiag (matrix * @var{m})
This function returns a single-column matrix that contains the diagonal
of matrix @var{m}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatDiag(matrix *M)
{
        matrix *D;
        int i;

        if (M == NULL)  Error("MatDiag","undefined matrix");
        if (M->imax != M->jmax) Error("MatDiag","matrix non carree");
        D = MatAlloc(M->imax,1);
        i=0;
        while (i < M->imax) { Mat(D,i,0) = Mat(M,i,i); i++; }
        return(D);
}

/********************************************************************/
/*                                                                  */
/* Fonctions min et max pour vecteurs et matrixs.                  */
/*                                                                  */
/********************************************************************/


/*************************************************************************/
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/*texi(um)---------------------------------------------------------------
@node  Min & Max,  , Trace & Diagonal, matrix
@comment  node-name,  next,  previous,  up
@section Minima and Maxima

@deftypefun {matrix *} MatMaxColumns (matrix * @var{m})
This function returns a single-line matrix that contains the maxima of
each column of @var{m}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatMaxColumns(matrix *M)
{
      int i=0;
      int j=0;
      matrix *Max;

      if (M == NULL) Error("MatMaxColumns","undefined matrix");
      Max = MatAlloc(1,M->jmax);
      MatFill(Max,-MAXREAL);
      while (j < M->jmax)
      {
            while (i < M->imax)
            {
                  if (Mat(M,i,j) > Mat(Max,0,j)) Mat(Max,0,j) = Mat(M,i,j);
                  i++;
            }
            i=0; j++;
      }
      return(Max);
}

/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatMinColumns (matrix * @var{m})
This function returns a single-line matrix that contains the minima of
each column of @var{m}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatMinColumns(matrix *M)
{
      int i=0;
      int j=0;
      matrix *Min;

      if (M == NULL) Error("MatMinColumns","undefined matrix");
      Min = MatAlloc(1,M->jmax);
      MatFill(Min,MAXREAL);
      while (j < M->jmax)
      {
            while (i < M->imax)
            {
                  if (Mat(M,i,j) < Mat(Min,0,j)) Mat(Min,0,j) = Mat(M,i,j);
                  i++;
            }
            i=0; j++;
      }
      return(Min);
}
/*************************************************************************/
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatMaxRow (matrix * @var{m})
Function that returns a single-column matrix that contains the maxima of
every @var{m}'s lines.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatMaxRow(matrix *M)
{
      int i=0;
      int j=0;
      matrix *Max;

      if (M == NULL) Error("MatMaxRow","undefined matrix");
      Max = MatAlloc(M->imax,1);
      MatFill(Max,-MAXREAL);
      while (j < M->jmax)
      {
            while (i < M->imax)
            {
                  if (Mat(M,i,j) > Mat(Max,i,0)) Mat(Max,i,0) = Mat(M,i,j);
                  i++;
            }
            i=0; j++;
      }
      return(Max);
}

/*texi(um)---------------------------------------------------------------
@deftypefun {matrix *} MatMinRow (matrix * @var{m})
Function that returns a single-column matrix that contains the minima of
every @var{m}'s lines.
@end deftypefun
texi(end)---------------------------------------------------------------- */

matrix *MatMinRow(matrix *M)
{
      int i=0;
      int j=0;
      matrix *Min;

      if (M == NULL) Error("MatMinRow","undefined matrix");
      Min = MatAlloc(1,M->jmax);
      MatFill(Min,MAXREAL);
      while (j < M->jmax)
      {
            while (i < M->imax)
            {
                  if (Mat(M,i,j) < Mat(Min,i,0)) Mat(Min,i,0) = Mat(M,i,j);
                  i++;
            }
            i=0; j++;
      }
      return(Min);
}

/*************************************************************************/
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/*texi(um)---------------------------------------------------------------
@deftypefun real MatMax (matrix * @var{m})
Function that returns the greatest value in matrix @var{m}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

real MatMax(matrix *M)
{
      int i=0;
      int j=0;
      real max = -MAXREAL;
      if (M == NULL) Error("MatMax","undefined matrix");
      while (j < M->jmax)
      {
            while (i < M->imax)
            {
                  if (Mat(M,i,j) > max) max = Mat(M,i,j);
                  i++;
            }
            i=0; j++;
      }
      return(max);
}



/*texi(um)---------------------------------------------------------------
@deftypefun real MatMin (matrix * @var{m})
Function that returns the smallest value in matrix @var{m}. 
@end deftypefun
texi(end)---------------------------------------------------------------- */

real MatMin(matrix *M)
{
      int i=0;
      int j=0;
      real min = MAXREAL;
      if (M == NULL) Error("MatMin","undefined matrix");
      while (j < M->jmax)
      {
            while (i < M->imax)
            {
                  if (Mat(M,i,j) < min) min = Mat(M,i,j);
                  i++;
            }
            i=0; j++;
      }
      return(min);
}

/*texi(um)---------------------------------------------------------------
@deftypefun real MatMax1 (matrix * @var{m})
This function returns the greatest value in first column of matrix
@var{m}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

real MatMax1(matrix *M)
{
      int i=0;
      real max = -MAXREAL;
      if (M == NULL) Error("MatMax1","undefined matrix");
      while (i < M->imax)
      {
            if (Mat(M,i,0) > max) max = Mat(M,i,0);
            i++;
      }
      return(max);
}

/*texi(um)---------------------------------------------------------------
@deftypefun real MatMin1 (matrix * @var{m})
This function returns the smallest value in first column of matrix
@var{m}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

real MatMin1(matrix *M)
{
      int i=0;
      real min = MAXREAL;

      if (M == NULL) Error("MatMin1","undefined matrix");
      while (i < M->imax)
      {
	  if (Mat(M,i,0) < min) min = Mat(M,i,0);
	  i++;
      }
      return(min);
}

/*texi(um)---------------------------------------------------------------
@deftypefun real MatMaxij (matrix * @var{m}, int * @var{i}, int * @var{j})
@deftypefunx real MatMinij (matrix * @var{m}, int * @var{i}, int * @var{j})
These functions finds the maximum (minimum) value in matrix @var{m}. It
returns this 
value and sets @var{i} and @var{j} to the line and column number where
the maximum (minimum) was found.
@end deftypefun
texi(end)---------------------------------------------------------------- */

real MatMaxij(matrix *M,
	       int    *imax,
	       int    *jmax)
{
      int i=0;
      int j=0;
      real max = -MAXREAL;
      if (M == NULL) Error("MatMax","undefined matrix");
      while (j < M->jmax)
      {
            while (i < M->imax)
            {
                  if (Mat(M,i,j) > max) 
                  {
                     max = Mat(M,i,j);
                     *imax = i; *jmax = j;
		  }
                  i++;
            }
            i=0; j++;
      }
      return(max);
}

/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */

real MatMinij(matrix *M,
	       int    *imin,
	       int    *jmin)
{
      int i=0;
      int j=0;
      real min = MAXREAL;
      if (M == NULL) Error("MatMin","undefined matrix");
      while (j < M->jmax)
      {
            while (i < M->imax)
            {
                  if (Mat(M,i,j) < min) 
                  {
                      min = Mat(M,i,j);
                      *imin = i; *jmin = j;
		  }
                  i++;
            }
            i=0; j++;
      }
      return(min);
}

/* ------------------------------------------------------------------------ */
/* This function returns the maximum value in a row of a matrix and gives   */
/* the column where the maximum was found.                                  */
/* Arguments are: (1) the matrix (2) the row (3) column where maximun was   */
/* found (value updated in function) (pointer on int).                      */
/* ------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun real MatMaxInRow (matrix * @var{m}, int @var{row}, int * @var{col})
This function returns the maximum value in the row @var{row} of  matrix
@var{m} and gives the column @var{col} where the maximum was found.
@end deftypefun
texi(end)---------------------------------------------------------------- */

real MatMaxInRow(matrix *M,
		 int row,
		 int *col)
{
    real f = -MAXREAL;
    int j;
				/* Check arguments */
    if (M == NULL) Error("MatMaxInRow","Matrix should be defined");
    if ((row < 0) || (row >= M->imax))
	Error("MatMaxInRow","row argument out of range");
    
    for(j=0; j<M->jmax; j++)
    {
	if (Mat(M,row,j) > f)
	{
	    f = Mat(M,row,j);
	    *col = j;
	}
    }
    /*    printf(" [col : %d] " , *col);*/
    return(f);
}

/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */

real MatMaxInColumn(matrix *M,
		    int col,
		    int *row)
{
    real f = -MAXREAL;
    int j;
				/* Check arguments */
    if (M == NULL) Error("MatMaxInColumn","Matrix should be defined");
    if ((col < 0) || (col >= M->jmax))
	Error("MatMaxInColumn","column argument out of range");
    /*MatPrint(M);*/
    for(j=0; j<M->imax; j++)
    {
	if (Mat(M,j,col) > f)
	{
	  /*printf (" (%d) ", j);*/
	    f = Mat(M,j,col);
	    *row = j;
	}
    }
    /*printf(" [row : %d col %d] " , *row, col);*/
    return(f);
}


matrix * MatAppendRows(matrix *m1, matrix * m2) {
  matrix * new = MatAlloc(m1->imax + m2->imax, m1->jmax);
  int i, j, k;
  /* printf("m1\n"); MatPrint(m1); */
  for (i = 0; i < m1->imax; i++)
    for (j = 0; j < m1->jmax; j++) {
      Mat(new,i,j) = Mat(m1,i,j);
      /* printf("%d %d %f\n", i,j,Mat(new,i,j)); */
    }
  /* printf("m1\n"); MatPrint(m1); */
  k = i;
  /* printf("new\n"); MatPrint(new); */
  for (i = 0; i < m2->imax; i++)
    for (j = 0; j < m1->jmax; j++)
      Mat(new,k+i,j) = Mat(m2,i,j);
  return new;
}

#define EST
#ifdef TEST
int
main()
{
    matrix *M;

    M = MatLoad((matrix *) 0, "toto.dat");
    printf( "%dxx%d \n", M->imax, M->jmax);

    return 1;
}

#endif
