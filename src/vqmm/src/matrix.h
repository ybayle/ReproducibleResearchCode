/* 	$Id: matrix.h,v 1.6 1995/02/21 18:56:38 tl Exp tl $	 */
/*
   $Log: matrix.h,v $
 * Revision 1.6  1995/02/21  18:56:38  tl
 * Changes reflects changes from version 1.7 to version 1.8 of matrix.c
 *
 * Revision 1.5  1994/06/30  15:46:55  tl
 * + The main change for this version is the use of a new type "real"
 * instead of type "float". This type is defined in file inc/define.h. It
 * allows to use either float or double changing only the definition of
 * "real". Matrices are saved in files in float format (this may be
 * changed in the future).
 * + A few changes and code cleaning has been made to comply the ansi c
 * standard.
 *
 * Revision 1.4  1994/06/20  14:17:14  tl
 * + New functions: MatEqualLine() and MatAllDiff().
 *
 * Revision 1.3  1994/05/28  17:58:59  tl
 * + New headers for functions MatShiftUp2() ansd MatShiftUp().
 *
 * Revision 1.2  1994/05/25  15:04:31  tl
 * + Now files matrix.h, mlp.h, iof.h may be included in any order.
 *
*/
/*######################################################################

     This file contains functions for matrix computing (headers).
     Copyright (C) 1994  Thibault Langlois
     
     Version: 1.0     Date: 29/03/94
     Version: 1.01    Date: 03/05/94
     see matrix.c
     Version: 1.02    Date: 05/05/94
     Small bug fixed.

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

#ifndef MATRIX_DEFINED
#define MATRIX_DEFINED

#include <stdarg.h>
#include "define.h"

 /* Type matrix : */
typedef struct matrix
	{
	   int imax;
	   int jmax;
	   real *values;
	} matrix;

 /* Macro pour acceder a une valeur de la matrix */
#define Mat(m,i,j) (*((m)->values+((i)*(m)->jmax)+(j)))

/*************************************************************************/
/*                                                                       */
/*  matrix.c : Fonctions pour la manipulation de matrices. Le type      */
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
/*************************************************************************/
/*                                                                       */
/*                                                                       */
/*************************************************************************/


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

matrix *MatAlloc(int imax, int jmax);

/*************************************************************************/
/*                                                                       */
/* Fonction qui libere la memoire associee a une matrix.                */
/*                                                                       */
/*************************************************************************/

/* ------------------------------------------------------------------------ */
/* Functions that free memory associated to a matrix (MatFree) or several   */
/* matrices (MatFrees). For MatFrees the last argument must be a 0.         */
/* Caution ! The memory is not cleaned ans the pointer remains with the     */ 
/* value.                                                                   */
/* ------------------------------------------------------------------------ */

void MatFree(matrix *m);

void MatFree2(matrix **m);

#define UNIX
#ifdef UNIX
void MatFrees(matrix *m,...);
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

void MatCopy(matrix *M1, 
	     matrix *M2);
matrix *
MatDuplicate(matrix *m);

/* ------------------------------------------------------------------------ */
/* Function that copies a line from a matrix to another matrix. The line    */
/* may correspond a part of the line of the destination matrix.             */
/* Arguments: (1) source matrix (2) line number in source matrix            */
/* (3) destination matrix (4) line number in destination matrix (5) column  */
/* number to start from in destination matrix.                              */
/* ------------------------------------------------------------------------ */
void MatCopyLine(matrix *M1,
		 int lineNo1,
		 matrix *M2,
		 int lineNo2,
		 int fromCol);

void MatCopyColumn(matrix *M1,
		   int columnNo1,
		   matrix *M2,
		   int columnNo2,
		   int fromLine);

/* ------------------------------------------------------------------------ */
/* Function that returns a matrix formed by the lines contained in M but    */
/* without repetition of lines.                                             */
/* ------------------------------------------------------------------------ */
matrix *MatRemoveDuplicateRows(matrix *M);

/* ------------------------------------------------------------------------ */
/* Function that swaps two lines in a matrix.                               */
/* ------------------------------------------------------------------------ */
void MatSwapLines(matrix *M,
		  int i1,
		  int i2);

/* ------------------------------------------------------------------------ */
/* Function that sorts the lines of a matrix in increasing order with       */
/* respect of the first colummn.                                            */
/* The algorithm is "bubble sort".                                           */
/* ------------------------------------------------------------------------ */
void MatSort(matrix *M);
void MatSortColumn(matrix *M, const int col);

/* ------------------------------------------------------------------------ */
/* Idem but the destination matrix is created and returned (pointer).       */
/* ------------------------------------------------------------------------ */

matrix *MatCopy2(matrix *M);

/********************************************************************/
/*                                                                  */
/* Function that shifts all the lines of a matrix (M1) one line up  */
/* and changes the last line of this matix (M1) to a new one (M2).  */
/*                                                                  */
/********************************************************************/


void MatShiftUp(matrix *M1);

void MatShiftUp2(matrix *M1,
		 matrix *M2);

/*************************************************************************/
/*                                                                       */
/* Affiche la matrix passee en parametre.                               */
/*                                                                       */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that prints a matrix on the standard output.                    */
/* ------------------------------------------------------------------------ */

void MatPrint(matrix *m);

/* ------------------------------------------------------------------------ */
/* Function that prints a line of a matrix on stdout.                       */
/* ------------------------------------------------------------------------ */
void MatPrintRow(matrix *m,
		  int line);

void MatPutRow(FILE *stream,
	       matrix *m,
	       int line);

void MatPutRowF(FILE *stream,
		char *format,
		matrix *m,
		int line);

/********************************************************************/
/*                                                                  */
/* Fonction qui initialise tous les elements de la matrix M a la   */
/* valeur f.                                                        */
/*                                                                  */
/********************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that fills a matrix with a real value.                         */
/* ------------------------------------------------------------------------ */

void MatFill(matrix *M,
	     real  f);

/********************************************************************/
/*                                                                  */
/* Fonction qui met a zero tous les elements de la matrix M.       */
/*                                                                  */
/********************************************************************/
/* ------------------------------------------------------------------------ */
/*  Function that fills a matrix with zeros.                                */
/* ------------------------------------------------------------------------ */

void MatZeros(matrix *M);



int MatEqual(matrix *M1,
	     matrix *M2);

/*
----------------------------------------------------------------------
Function that returns TRUE if two lines (indiced by i1 and i2) of the
matrix m are equal. Returns FALSE otherwise.
---------------------------------------------------------------------- */
int MatEqualLine(matrix *m,
		 int    i1,
		 int    i2);

/* ------------------------------------------------------------------------ */
/* Idem but with two different matrices.                                    */
/* ------------------------------------------------------------------------ */
int MatEqualLines(matrix *m1,
		  int    i1,
		  matrix *m2,
		  int    i2);

/*
----------------------------------------------------------------------
Function that returns TRUE if all lines of matrix m are
different from each other. Returns FALSE otherwise.
---------------------------------------------------------------------- */

int MatAllDiff(matrix *m);

/*************************************************************************/
/*                                                                       */
/* Multiplication d'une matrix par un scalaire.                         */
/* Arguments : reel, pointeur sur la matrix.                            */
/*                                                                       */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that multiplies each element of a matrix by a real value.      */
/* ------------------------------------------------------------------------ */

void MatMulScal(real  x, 
		matrix *m);

/* ------------------------------------------------------------------------ */
/* Idem but a new matrix is created and returned.                           */
/* ------------------------------------------------------------------------ */

matrix *MatMulScal2(real  x, 
		    matrix *m);

/* ------------------------------------------------------------------------ */
matrix *MatSubScal2(real  f,
		    matrix *M2);

/*************************************************************************/
/*                                                                       */
/* Multiplication de deux matrixs passees en parametre (pointeurs).     */
/* Rend un pointeur sur la matrix resultat.                             */
/*                                                                       */
/*************************************************************************/

void MatMul(matrix *M3,
	    matrix *M1,
	    matrix *M2);

matrix *MatMul2(matrix *M1, 
		matrix *M2);

/*************************************************************************/
/*                                                                       */
/* Fonction qui multiplie deux matrixs termes a termes.                 */
/*                                                                       */
/*************************************************************************/
matrix *MatMultt2(matrix *M1,
		  matrix *M2);

/*************************************************************************/
/*                                                                       */
/* Addition de deux matrixs.                                            */
/*                                                                       */
/*************************************************************************/
void MatAdd(matrix *M1,
	    matrix *M2);

matrix *MatAdd2(matrix *M1,
		matrix *M2);

void MatAddScal(real  f,
		matrix *M2);

matrix *MatSub2(matrix *M1,
		matrix *M2);
void
MatSub3(matrix *M1,
	matrix *M2,
	matrix *M3);

/********************************************************************/
/*                                                                  */
/* Fonction qui multiplie un vecteur par lui meme v * transpose(v)  */
/* et rend le resultat.                                             */
/*                                                                  */
/********************************************************************/

real MatSqr(matrix *M);

real MatSqrCol(matrix *M,
	       int    j);

/*************************************************************************/
/*                                                                       */
/* Fonction donnant la trace de la matrix M passee en parametre.        */
/* M doit etre carree.                                                   */
/*                                                                       */
/*************************************************************************/

real MatTrace(matrix *m);

/*************************************************************************/
/*                                                                       */
/* Fonction donnant la transposee de la matrix M passee en parametre.   */
/* Une nouvelle matrix est cree.                                        */
/*                                                                       */
/*************************************************************************/

void MatTransS(matrix *m);

void MatTrans(matrix **m);

matrix *MatTrans2(matrix *m);

/********************************************************************/
/*                                                                  */
/* Fonctions min et max pour vecteurs et matrixs.                  */
/*                                                                  */
/********************************************************************/


/*************************************************************************/
/*                                                                       */
/*                                                                       */
/*************************************************************************/
matrix *MatMaxColumns(matrix *M);

matrix *MatMinColumns(matrix *M);

/*************************************************************************/
/*                                                                       */
/*                                                                       */
/*************************************************************************/
matrix *MatMaxRow(matrix *M);

matrix *MatMinRow(matrix *M);

/*************************************************************************/
/*                                                                       */
/*                                                                       */
/*************************************************************************/
real MatMax(matrix *M);

real MatMax1(matrix *M);

real MatMin1(matrix *M);


real MatMin(matrix *M);

real MatMaxij(matrix *M,
	       int    *imax,
	       int    *jmax);

real MatMinij(matrix *M,
	       int    *imin,
	       int    *jmin);

/* ------------------------------------------------------------------------ */
/* This function returns the maximum value in a row of a matrix and gives   */
/* the column where the maximum was found.                                  */
/* Arguments are: (1) the matrix (2) the row (3) column where maximun was   */
/* found (value updated in function) (pointer on int).                      */
/* ------------------------------------------------------------------------ */
real MatMaxInRow(matrix *M,
		 int row,
		 int *col);

real MatMaxInColumn(matrix *M,
		    int col,
		    int *row);

/***************************************************************/
/*************************************************************************/
/*                                                                       */
/*                                                                       */
/*************************************************************************/
matrix *MatDiag(matrix *M);

/*************************************************************************/
/*                                                                       */
/*                                                                       */
/*************************************************************************/
void MatApply(double  (* f)(real),
	       matrix *M);

matrix *MatApply2(double  (* f)(real),
		  matrix *M);


/********************************************************************/
/*                                                                  */
/* MatLoad() et MatSave():                                          */
/* Fonctions qui lisent ou ecrivent une matrix dans un fichier     */
/*                                                                  */
/* Arguments : 1. pointeur sur une matrix deja allouee.            */
/*             2. pointeur sur le nom du fichier (char *).          */
/*                                                                  */
/********************************************************************/

				/* THESE FUNCTIONS SHOULD NOT BE USED */

/* int MatLoad2(matrix *M, */
/* 	     char   *filename) */

/* int GetBlanks(FILE *stream, */
/* 	      char *c) */

/* void MatFindDimensions(char *fileName, */
/* 		       int *nbRows, */
/* 		       int *nbCols); */

/* int OctaveFormat(char *filename); */

/* void MatFindOctaveDimensions(FILE *F, */
/* 			    int *nbRows, */
/* 			    int *nbCols); */

matrix *MatLoad(matrix *M,
		char   *filename);

/********************************************************************/
/*                                                                  */
/* Fonction qui ecrit une matrix dans un fichier.                  */
/*                                                                  */
/********************************************************************/

void MatSave(matrix *M,
	     char   *filename);

void MatSaveOctave(matrix *M,
		   char *name,
		   char   *filename);

/********************************************************************/
/*                                                                  */
/* MatPut() et MatGet():                                            */
/*   Fonctions ecrivent et lisent une matrix dans le fichier.      */
/*   Arguments : 1. pointeur sur un fichier ouvert.                 */
/*               2. pointeur sur une matrix.                       */
/*                                                                  */
/********************************************************************/
void MatPutF(FILE   *F,
	     matrix *M,
	     char *format);

void MatPut(FILE *F,
	    matrix *M);

void MatGet(FILE   *F,
	    matrix *M);


matrix * MatAppendRows(matrix *m1, matrix * m2);


#endif










