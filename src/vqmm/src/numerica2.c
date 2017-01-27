/* 	@(#)$Id: numerica2.c,v 1.4 1995/03/16 10:52:41 lmn Exp tl $	 */
/* 	Thibault Langlois (tl@inesc.pt)	 */

#ifndef lint
static char vcid[] = "@(#)$Id: numerica2.c,v 1.4 1995/03/16 10:52:41 lmn Exp tl $";
#endif /* lint */
/*
   $Log: numerica2.c,v $
 * Revision 1.4  1995/03/16  10:52:41  lmn
 * NIL
 *
 * Revision 1.3  1995/03/02  14:35:30  lmn
 *  C++ Adaptation.
 *  Variable class was changed into theClass.
 *
 * Revision 1.2  1995/03/01  09:57:40  tl
 * + New functions MatStat and StatPerClass.
 *
*/
/*######################################################################

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
#include <ctype.h>
#include <sys/types.h>
#include <math.h>
/* #include <values.h> */
#include <stdio.h>
#ifdef __APPLE__ // mac os x specific
	#include "sys/malloc.h"
#else
	#include <malloc.h>
#endif
#include <stdlib.h>

#include "define.h"
#include "matrix.h"
#include "random.h"
#include "matrand.h"
#include "error.h"
#include "numerica2.h"

/*************************************************************************/
/* Fonction donnant le determinant de la matrix M passee en parametre.   */
/* M doit etre carree.                                                   */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that returns the determinant of a matrix.                       */
/* ------------------------------------------------------------------------ */
real 
MatDet(matrix *m)
{
      int i=0,j=1;
      int k=0,ii=0,alpha=1;
      real d=0;
      matrix *m1;

      if (m->imax != m->jmax)
	  Error("MatDet","The matrix should be square");
      if (m->imax > 1)
      {
	 while (k < m->imax)
	 {
	       m1 = MatAlloc(m->imax-1,m->jmax-1);
	       while (i < m->imax)
	       {
		     if (i != k)
		     {
			while (j < m->jmax)
			{
			      Mat(m1,ii,j-1) = Mat(m,i,j);
			      j++;
			}
			ii++;
		     }
		     j=1;i++;
	       }
	       d = d + Mat(m,k,0) * alpha * MatDet(m1);
	       alpha *= -1;
	       MatFree(m1);
	       k++;i=0;ii=0;
	 }
	 return(d);
      } else return(Mat(m,0,0));
}


/*************************************************************************/
/* Fonction donnant la co-matrice de la matrix M passee en parametre.    */
/* M doit etre carree. Une nouvelle matrice est cree.                    */
/*************************************************************************/
matrix *
MatCo(matrix *m)
{
	int i=0,j=0,k=0,l=0,ii=0,jj=0;
	matrix *co_matrix, *m1;

	if (m->imax != m->jmax)
	  Error("MatCo","The matrix should be square");

	co_matrix = MatAlloc(m->imax,m->jmax);
	m1 = MatAlloc(m->imax-1,m->jmax-1);
	while (k < m->imax)
	{
	      while (l < m->jmax)
	      {
		    while (i < m->imax)
		    {
			  if (i != k)
			  {
			     while (j < m->jmax)
			     {
				   if (j != l)
				   {
				      Mat(m1,ii,jj) = Mat(m,i,j);
				      jj++;
				   }
				   j++;
			     }
			     jj=0; j=0; ii++;
			  }
			  i++;
		    }
		    i=0; ii=0;
		    Mat(co_matrix,k,l) = pow(-1,k+l+2) * MatDet(m1);
		    l++;
	      }
	      l=0; k++;
	}
	MatFree(m1);
	return(co_matrix);
}

/*************************************************************************/
/* Fonction donnant l'inverse de la matrice M passee en parametre.       */
/* M doit etre carree. Une nouvelle matrice est cree.                    */
/*************************************************************************/
/*
matrix *inv(matrix *m)
{
	real determinant;
	matrix *m1, *m2;

	if (m->imax != m->jmax)
	{puts("*** Error la inverse est calculee qu'avec des matrixs carrees.");
	printf("    imax %d jmax %d \n",m->imax,m->jmax); }
	determinant = MatDet(m);
	if (determinant == 0) {puts("Matrix non inversible !!!");return(0);}
	m1 = MatCo(m);
	m2 = MatTrans2(m1);
	MatMulScal(1/determinant,m2);
	MatFree(m1);
	return(m2);
}
*/

/*************************************************************************/
/* Fonction normant les lignes d'une matrice (||ligne|| = 1).            */
/* Le parametre est un pointeur sur la matrice. Celle-ci est modifiee.   */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that transforms a matrix such that ||row|| = 1 for all rows.    */
/* ------------------------------------------------------------------------ */
void 
MatNormLi(matrix *F)
{
     real norme=0.0;
     int i=0,j=0;

     while (i < F->imax)
     {
           while (j < F->jmax)
           {
                 norme = norme + pow(Mat(F,i,j),2); j++;
           }
           norme = (real) sqrt((double)norme);
           j = 0;
           while (j < F->jmax)
           {
                 if (norme != 0.0) 
		     Mat(F,i,j) = Mat(F,i,j)/norme; 
		 j++;
           }
           norme = 0.0; j=0; i++;
     }
}

/*************************************************************************/
/* Fonction normant les lignes d'une matrice (||colonne|| = 1).          */
/* Le parametre est un pointeur sur la matrice. Celle-ci est modifiee.   */
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* Function that transforms a matrix such that ||column|| = 1,              */
/* for all columns.                                                         */
/* ------------------------------------------------------------------------ */
void 
MatNormCo(matrix *M)
{
     real norme=0.0;
     int i=0,j=0;

     while (j < M->jmax)
     {
           while (i < M->imax)
           {
                 norme = norme + pow(Mat(M,i,j),2); i++;
           }
           norme = sqrt(norme);
           i = 0;
           while (i < M->imax)
           {
                 Mat(M,i,j) = Mat(M,i,j)/norme; i++;
           }
           norme = 0.0; i=0; j++;
     }
}
/*************************************************************************/
/*************************************************************************/
/* ------------------------------------------------------------------------ */
/* ???????????????????????????????????????????????????????????????????????? */
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* This function returns the total squared error between two matrices. The  */
/* formula is : sum_i sum_j (m1_ij -m2_ij)^2.                               */
/* ------------------------------------------------------------------------ */
real 
MatTotalSquaredError(matrix *M1,
		     matrix *M2)
{
    int i,j;
    real totalSE = 0.0;
				/* Check the arguments */
    if ((M1 == NULL) || (M2 == NULL)) 
	Error("MatTotalSquaredError","The matrices should be defined");
    if ((M1->imax != M2->imax) || (M1->jmax != M2->jmax))
	Error("MatTotalSquaredError","The matrices should have same size");

    for(i=0; i<M1->imax; i++)
    {
	for(j=0; j<M1->jmax; j++)
	{
	    totalSE += sqr(Mat(M1,i,j) - Mat(M2,i,j));
	}
    }
    return(totalSE);
}

/* ------------------------------------------------------------------------ */
/* Function that returns the distance between a vector and a row of a       */
/* matrix.                                                                  */
/* Arguments: (1) vector (type matrix) (2) the matrix (3) the row number.   */
/* ------------------------------------------------------------------------ */
real 
MatDist(matrix *v,
	matrix *m,
	int    i)
{
      real d,f;
      int j;
      j=0; d=0.0;
      while (j < v->imax)
      {
            f = (Mat(v,j,0)) - (Mat(m,i,j));
            d = d + (f * f);
            j++;
      }
      d = (real) sqrt((double) d);
      return(d);
}

/* ------------------------------------------------------------------------ */
/* Function that returns the k nearest neighbors of a vector in a data set. */
/* Arguments: (1) the k parameter (2) the vector (type matrix) (3) the data */
/* set (each row is a pattern).                                             */
/* ------------------------------------------------------------------------ */
matrix *
KNearestNeighbor(int    k,
		 matrix *v,
		 matrix *m)
{
        matrix *ppv,*dist;
        int i,l,j;
	real max;
	int imax=-1;

        ppv = MatAlloc(k,1);	/* Niearest neighbors (indices) */
        dist = MatAlloc(m->imax,1); /* distances vector */
	max = - MAXREAL;
        i=0;
        while (i < m->imax)	/* compute distances */
        {
              Mat(dist,i,0) = MatDist(v,m,i);
	      if (Mat(dist,i,0) > max)
	      {
		  max = Mat(dist,i,0);
		  imax = i;
	      }
              i++;
        }
	MatFill(ppv,(real)imax);
        for (i=0; i < dist->imax; i++)
        {
              j=0;
              while ((j < k) && 
		     (Mat(dist,i,0) >= Mat(dist,(int)Mat(ppv,j,0),0))) 
                    j++;
              if (j < k) 
              {
                 l = ppv->imax-1;
                 while (l > j) 
                 {
                       Mat(ppv,l,0) = Mat(ppv,l-1,0); l--; 
                 }
                 Mat(ppv,j,0) = i;
	      }
        }
        MatFree(dist);
        return(ppv);
}

/* ------------------------------------------------------------------------ */
/* Function that finds the indices of the first and second nearest          */
/* neighbors of a vector in a data set.                                     */
/* Arguments: (1) the vector (2) the data set (each line is a pattern)      */
/* (3) the indice of the first nearest neighbor (on output) (4) the indice  */
/* of the second nearest neighbor.                                          */
/* ------------------------------------------------------------------------ */
void 
FirstSecondNearestNeighbors(matrix *v,		/* center */
			    matrix *m,		/* data set */
			    int    *i1,
			    int    *i2)
{
    matrix *dist;
    int i;
    real d1,d2;
    
    dist = MatAlloc(m->imax,1);
    for (i=0;i < m->imax;i++)
    {
	Mat(dist,i,0) = MatDist(v,m,i);
    }
    d1 = MAXREAL;
    d2 = MAXREAL;
    for (i=0;i<dist->imax;i++)
    {
	if (d1 > Mat(dist,i,0))
	{
	    d2 = d1; *i2 = *i1;
	    d1 = Mat(dist,i,0);
	    *i1 = i;
	}
    }
    MatFree(dist);
}


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

void 
StochastickMeans(matrix *set, 
		 matrix *centers, 
		 int    maxit)
{
    int i,n,j;
    matrix *v;
    int i1,i2;
    real alpha = 0.1;
    real total, last_total;
    real d;
    int z;
    real nn;

    v = MatAlloc(set->jmax,1);
				/* Choose initial centers, randomly in the */
				/* data set. */
    for(i=0;i<centers->imax;i++)
    {
	n = (int) RandomInterval2(0.0, (real)set->imax);
	for(j=0;j<centers->jmax;j++)
	{
	    Mat(centers,i,j) = Mat(set,n,j);
	}
    }

    z=0;
    total=0.0;
    do
    {
	last_total=total; total=0.0;

				/* For every pattern in data set: */
	for(i=0;i<set->imax;i++)
	{
				/* Select one pattern in the data set, */
				/* randomly. */
	    n = (int) RandomInterval2(0.0, (real)set->imax);
	    for(j=0;j<set->jmax;j++)
		Mat(v,j,0) = Mat(set,n,j);

				/* What is the first nearest neighbour of */
				/* this pattern in the set of centers ? */
	    FirstSecondNearestNeighbors(v,centers,&i1,&i2);

				/* Compute the distance between the pattern */
				/* and the nearest center. Move this center */
				/* toward the pattern. */
	    for(j=0;j<centers->jmax;j++)
	    {
		d = (Mat(set,n,j) - Mat(centers,i1,j));
		Mat(centers,i1,j) += alpha * d;
				/* Calculate the sum of all distances. */
		total += d;
	    }
	}
	z++;
    } while (z<maxit);
    MatFree(v);
    fprintf(stderr,"Stochastic k-means: %d centers, %d iterations\n",centers->imax,z);
}

/*
----------------------------------------------------------------------
This function executes the k-Means algorithm. The arguments are the
data set and a matrix whose size correspond to the desired number of
centers. The algorithm stops when the centers do not move.
---------------------------------------------------------------------- */
#include "/usr/include/math.h"

void 
kMeans(const matrix *set, 
       matrix *centers,
       int quiet,
       real epsilon)
{
    int i,n,j;
    matrix *v;
    matrix *newCenters;		/* Gravity center of each cluster */
    matrix *iC;			/* Nb of points in each cluster */
    int i1,i2;
    real total;
    int z;
    FILE *F;

    if (centers->imax > set->imax) Error("kMeans", "too many clusters");

    for (i=0; i < set->imax; i++) {
	for (j=0; j < set->jmax; j++) {
	    if (isinf(Mat(set,i,j)) || isnan(Mat(set,i,j))) {
		fprintf(stderr, 
			"*** Problem Nan of Inf in data set at %d %d \n", 
		       i,j);		
		exit(1);
	      }
	  }
      }
    v = MatAlloc(set->jmax,1);
    newCenters = MatAlloc(centers->imax,centers->jmax);
    iC = MatAlloc(centers->imax,1);
    /* Initialize centers to data points, */
    /* randomly. */
    /*    for(i=0;i<centers->imax;i++)
          {
          n = (int) RandomInterval2(0.0, (real)set->imax);
          for(j=0;j<centers->jmax;j++)
          {
          Mat(centers,i,j) = Mat(set,n,j);
          }
          }
    */
    RandomSubSet(set,centers);
    z=0;
    do {
	total=0.0;
				/* For every pattern in data set. */
	for (i=0;i<set->imax;i++) {
				/* Select the ith pattern. */
          for (j=0;j<set->jmax;j++)
            Mat(v,j,0) = Mat(set,i,j);
				/* What is the first nearest neighbour of */
				/* this pattern in the set of centers ? */
	    FirstSecondNearestNeighbors(v,centers,&i1,&i2);	
				/* Clusters are associated to each center. */
				/* Compute the center of gravity of the */
				/* clusters to which the current pattern */
				/* belongs. */
	    for (j=0;j<centers->jmax;j++) {			       
		Mat(newCenters,i1,j) += Mat(set,i,j);
	    }
	    Mat(iC,i1,0) = Mat(iC,i1,0) + 1;
	}
				/* Set new centers to centers of gravity of */
				/* each cluster. */
	for (i=0;i<centers->imax;i++) {		
          for (j=0;j<centers->jmax;j++) {
            if (Mat(iC,i,0) != 0.0)
              Mat(newCenters,i,j) /= Mat(iC,i,0);
            else
              Mat(newCenters,i,j) = Mat(centers,i,j);
            /* Compute the total movement of the centers */
            total += sqr(Mat(newCenters,i,j) - Mat(centers,i,j));
          }
	}
	if ((z == 0) && (total == 0.0)) { 
	    fprintf(stderr, 
		    "Please verify data format. If the format used is too long (ex: 1.2075538306700864d0), the MatLoad() function may have read incorrecly the data.\n"); 
	    exit(1); 
	  };	
	MatCopy(newCenters,centers);
	MatZeros(newCenters);
	MatZeros(iC);
	z++;
	fprintf(stderr, " total %5.2e imax %d jmax %d \n", 
		total, set->imax, centers->jmax);
	/* if (quiet == 0) */
	  fprintf(stderr, " * [%d] %5.2e", z, total/set->imax/centers->jmax);
				/* Do this until the centers do not move */
				/* or number of iteration = 10000 */
    } while (((total/set->imax/centers->jmax) > epsilon) && (z<10000));
    MatFrees(v,newCenters,iC,0);
    if (quiet == 0) 
      fprintf(stderr,"\nk-means: %d centers, %d iterations\n",centers->imax,z);
				/* If the algorithm did not converge before */
				/* 10000 iterations, try again. */
    MatPrint(centers);
    if (MatAllDiff(centers) == FALSE) kMeans(set,centers,quiet,epsilon);
}





