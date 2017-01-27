
/*
   $Log: numerica2.h,v $
 * Revision 1.1  1995/03/01  10:00:28  tl
 * Initial revision
 *
*/
/*######################################################################

     This file contains functions for matrix computing.
     Copyright (C) 1994  Thibault Langlois

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

#ifndef NUMERICA2_DEFINED
#define NUMERICA2_DEFINED

#include "define.h"
#include "matrix.h"

/*************************************************************************/
/* Fonction donnant le determinant de la matrix M passee en parametre.   */
/* M doit etre carree.                                                   */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that returns the determinant of a matrix.                       */
/* ------------------------------------------------------------------------ */
real MatDet(matrix *m);
/*************************************************************************/
/* Fonction donnant la co-matrice de la matrix M passee en parametre.    */
/* M doit etre carree. Une nouvelle matrice est cree.                    */
/*************************************************************************/
matrix *MatCo(matrix *m);
/*************************************************************************/
/* Fonction normant les lignes d'une matrice (||ligne|| = 1).            */
/* Le parametre est un pointeur sur la matrice. Celle-ci est modifiee.   */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that transforms a matrix such that ||row|| = 1 for all rows.    */
/* ------------------------------------------------------------------------ */
void MatNormLi(matrix *F);
/*************************************************************************/
/* Fonction normant les lignes d'une matrice (||colonne|| = 1).          */
/* Le parametre est un pointeur sur la matrice. Celle-ci est modifiee.   */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that transforms a matrix such that ||column|| = 1,              */
/* for all columns.                                                         */
/* ------------------------------------------------------------------------ */
void MatNormCo(matrix *M);
/*************************************************************************/
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* ???????????????????????????????????????????????????????????????????????? */
/* ------------------------------------------------------------------------ */
matrix *MatNormalize(matrix *M);
/*************************************************************************/
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that returns a matrix computed from matrix M such that:         */
/*            NewMatrix = (M - Mean(M))/Std(M)                              */
/* ------------------------------------------------------------------------ */
matrix *MatCR(matrix *M);
/********************************************************************/
/* Fonction qui rend la moyenne des colonnes d'une matrice.         */
/********************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that returns a one-line matrix equal to the mean of the columns */
/* of the input matrix.                                                     */
/* ------------------------------------------------------------------------ */ 
matrix *MatMean(matrix *M);
/*************************************************************************/
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that returns a one-line matrix equal to the standard deviation  */
/*  of the columns of the input matrix.                                     */
/* ------------------------------------------------------------------------ */ 
matrix *MatStd(matrix *M);
matrix *MatMedian(matrix *M);
/* ------------------------------------------------------------------------ */
/* Function that returns a matrix that contains on first line the min(M),   */
/* on second line max(M), on third line mean(M) and on the fourth line      */
/* std(M).                                                                  */
/* ------------------------------------------------------------------------ */
matrix *MatStat(matrix *M);
/* ------------------------------------------------------------------------ */
/* Function that computes statistics per class in a data set. Classes are   */
/* recognized with an "output" matrix which has different lines for each    */
/* class. Min, Max, Mean, and Std are computed for each class. The output   */
/* to an opened stream.                                                     */
/* ------------------------------------------------------------------------ */
void StatPerClass(FILE *stream,
		  matrix *inputs, 
		  matrix *outputs);
/* ------------------------------------------------------------------------ */
/* This function returns the total squared error between two matrices. The  */
/* formula is : sum_i sum_j (m1_ij -m2_ij)^2.                               */
/* ------------------------------------------------------------------------ */
real MatTotalSquaredError(matrix *M1,
			  matrix *M2);
/* ------------------------------------------------------------------------ */
/* Function that returns the distance between a vector and a row of a       */
/* matrix.                                                                  */
/* Arguments: (1) vector (type matrix) (2) the matrix (3) the row number.   */
/* ------------------------------------------------------------------------ */
real MatDist(matrix *v,
	      matrix *m,
	      int    i);
/* ------------------------------------------------------------------------ */
/* Function that returns the k nearest neighbors of a vector in a data set. */
/* Arguments: (1) the k parameter (2) the vector (type matrix) (3) the data */
/* set (each row is a pattern).                                             */
/* ------------------------------------------------------------------------ */
matrix *KNearestNeighbor(int    k,
			 matrix *v,
			 matrix *m);
/* ------------------------------------------------------------------------ */
/* Function that finds the indices of the first and second nearest          */
/* neighbors of a vector in a data set.                                     */
/* Arguments: (1) the vector (2) the data set (each line is a pattern)      */
/* (3) the indice of the first nearest neighbor (on output) (4) the indice  */
/* of the second nearest neighbor.                                          */
/* ------------------------------------------------------------------------ */
void FirstSecondNearestNeighbors(matrix *v,		/* center */
				 matrix *m,		/* data set */
				 int    *i1,
				 int    *i2);
/*
----------------------------------------------------------------------
This function executes a stocastic version of the k-Mean
algorithm. The arguments are: 
       + the data set, 
       + a matrix whose size correspond to the desired number of centers 
       + the number of iterations to do. 

Centers are initialized to points from the data set, randomly. 
Note: the points in the data set must be considered randomly
in the procedure that updates the position of the centers.
---------------------------------------------------------------------- */

void StochastickMeans(matrix *set, 
		      matrix *centers, 
		      int    maxit);
/*
----------------------------------------------------------------------
This function executes the k-Means algorithm. The arguments are the
data set and a matrix whose size correspond to the desired number of
centers. The algorithm stops when the centers do not move.
---------------------------------------------------------------------- */

void kMeans(const matrix *set, 
	    matrix *centers,
	    int quiet,
	    real epsilon);

#endif



