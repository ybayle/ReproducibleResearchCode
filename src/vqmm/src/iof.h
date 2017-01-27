/* 	$Id: iof.h,v 1.3 1994/07/11 13:44:24 tl Exp tl $	 */
/*
   $Log: iof.h,v $
 * Revision 1.3  1994/07/11  13:44:24  tl
 * + New functions: FileNewName() and Base().
 * + Small changes to comply the ansi standard.
 *
 * Revision 1.2  1994/05/25  15:08:09  tl
 * + Now files mlp.h, matrix.h and iof.h may be included in any order.
 *
*/
/*######################################################################

     This file contains functions for file input/output (headers).
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

 /* iof.h : Fonctions d'entrees sorties pour les fichiers */
 /* Les valeurs retournees par les fonctions de base (fopen,...) */
 /* sont testees. Les macros WARNING et ERREUR sont utilisees */
 /* en cas de probleme. */
 /* Thibault LANGLOIS */
 /* Septembre 1992 */

#ifndef IOF_DEFINED
#define IOF_DEFINED
#include <stdio.h>

 /* Ouverture d'un fichier (comme fopen) */
FILE *FileOpen(char *, char *);

 /* Fermeture d'un fichier (comme fclose) */
int FileClose(FILE *);

 /* Pour changer le nom d'un fichier */
int FileRename(char * , char * );

int FileExist(char *filename);

char *FileName(char *name, 
	       int  n,
	       char *ext);

char * RemoveExtension(char *filename);
char * RemoveDirectory(char * filename);
char * FilenameName(char *filename);

char *Base(int n,
           int to);

char *FileNewName(char *prefix,
		  char *extension);

 /* Pour lire une ligne dans un fichier (longueur max = 3000 caracteres) */
char *LineRead(FILE *input);

/*********************************************************************

 Fonctions qui lisent et ecrivent un entier dans un fichier.

*********************************************************************/

void IntSave(int  i,
	     char *filename);

int IntLoad(char * filename);


/*********************************************************************

 Fonctions qui lisent et ecrivent un float dans un fichier.

*********************************************************************/

void FloatSave(float f,
	       char  *filename);

float FloatLoad(char *filename);

char ** ReadLines(char * filename, int * nLines);

#endif
