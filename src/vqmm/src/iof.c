/* 	$Id: iof.c,v 1.7 1995/03/02 14:47:51 lmn Exp tl $	 */

#ifndef lint
static char vcid[] = "$Id: iof.c,v 1.7 1995/03/02 14:47:51 lmn Exp tl $";
#endif /* lint */
/*
   $Log: iof.c,v $
 * Revision 1.7  1995/03/02  14:47:51  lmn
 * *** empty log message ***
 *
 * Revision 1.6  1995/03/02  14:24:04  lmn
 *  C++ adaptation.
 *  Variables called new and old where changed to newName oldName.
 *
 *  Thu Mar  2 15:22:35 MET 1995
 *  Luis.Nunes@inesc.pt
 *
 * Revision 1.5  1995/02/02  16:50:58  tl
 * + Small changes due to the creation of the "error.c" module.
 *
 * Revision 1.4  1994/07/11  13:40:13  tl
 * + Small changes to comply ansi standard.
 *
 * Revision 1.3  1994/06/20  14:04:53  tl
 * + New functions: Base() and FileNewName().
 *
*/
/*######################################################################

     This file contains functions for file input/output.
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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>

#include "define.h"
#include "error.h"
#include "iof.h"


/*texi(um)---------------------------------------------------------------

@node  iof, matrix, Top, Top
@comment  node-name,  next,  previous,  up
@chapter The @code{iof} module
This module contains functions for doing input/output to files. This
section documents version @value{iof-version} of the @code{iof}
module. We can distinguish three kinds of functions:
 
@itemize @bullet
@item
Functions that constitute a layer on the top of traditional functions
of the input/output C Library (@code{stdio}). These functions are
convenient because they check the status values returned by the
@code{stdio} library functions. When an error occurs, the @code{Error}
or @code{Warning} function is called. 

@item
Functions that facilitate the creation of file names. 
@item
Functions to load and save numerical values in files.
@end itemize


@menu
* O/C/Rename::                  Opening/Closing/Renaming files
* File names::                  Create File Names
* Load/Save::                   Load and Save lines and numbers
@end menu

texi(end)---------------------------------------------------------------- */

/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */

/*texi(um)---------------------------------------------------------------
@node  O/C/Rename, File names, iof, iof
@comment  node-name,  next,  previous,  up
@section Opening/Closing/Renaming files

Two functions are defined on the top of @code{fopen} and @code{fclose}:

@deftypefun {FILE *} FileOpen (char * @var{file-name}, char *  @var{mode})
This function calls @code{fopen} in order to open the file
@var{file-name} with mode defined by @var{mode}. Both arguments are
strings. If @code{fopen} fails, the function @code{Warning} is called
printing the @var{file-name} argument on @var{stderr}. When
@code{fopen} succeeds, the function returns a stream (type @code{FILE
*}).  
@end deftypefun

texi(end)---------------------------------------------------------------- */

FILE *FileOpen(char *filename, 
	       char *mode)
{
     FILE *stream;
     
     stream = fopen(filename,mode);
     if (stream == NULL)
     {              
        Warning("FileOpen",filename); 
     }
     return(stream);
}


/*texi(um)---------------------------------------------------------------

@deftypefun int FileRename (char * @var{old-name}, char * @var{new-name})
Function that renames a file. First @var{new-name} is removed using
@code{remove}, then @var{old-name} is renamed to @var{new-name} with the
function @code{rename}. If @var{new-name} does not exist, it is created and
the function @code{Warning} is called. @var{old-name} and @var{new-name}
are of type @code{char *}.
@end deftypefun

texi(end)---------------------------------------------------------------- */

/*
extern int      remove( const char *__filename );
extern int      rename( const char *__filename1, const char *__filename2);
*/

int FileRename(char *oldName,
	       char *newName)
{
     FILE *F;
     int stat;

     stat = remove(newName);
     stat = rename(oldName,newName);
#ifdef DEBUG
printf("FileRename %s %s\n",oldName,newName);
#endif
     if (stat != 0)
     {
	F = FileOpen(newName,"w"); FileClose(F);
	Warning("FileRename",oldName);
	printf("%s created\n",newName);
     }
     return(0);
}

/*texi(um)---------------------------------------------------------------

@deftypefun int FileClose (FILE * @var{stream})
This function calls the @code{fclose} function in order to close
@var{stream}. If @code{fclose} fails to close the @var{stream}, the
function @code{Error} is called. 
@end deftypefun

texi(end)---------------------------------------------------------------- */

int FileClose(FILE *stream)
{     
     int stat;
     stat = fclose(stream);
     if (stat != 0)
     {
        Error("FileClose","fclose");
     }
     return(stat);
}

/*texi(um)---------------------------------------------------------------
@node  File names, Load/Save, O/C/Rename, iof
@comment  node-name,  next,  previous,  up
@section Create File Names

@deftypefun {char *} FileName (char * @var{name}, int @var{n}, char *  @var{extension})
This functions returns a file name (type @code{char *}) formed by the
concatenation of 
@var{name}, an integer @var{n} and an @var{extension}. For example:
@example
FileName("fileNo", 3, ".dat")
@result{} "fileNo3.dat"
@end example
@end deftypefun
texi(end)---------------------------------------------------------------- */

char *FileName(char *name, 
	       int  n,
	       char *ext)
{
    char *filename;
    char num[5];

    filename = (char *)malloc(50 * sizeof(char));
    strcpy(filename,name);
    sprintf(num,"%d",n);
    strcat(filename,num);
    strcat(filename,ext);
    return(filename);
}

/* ------------------------------------------------------------------------
This function converts an integer from base 10 to any base in [2 36]. 
returns a string that contains the representation of the number (first 
argument) in base indicated by the second argument.
------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
texi(end)---------------------------------------------------------------- */

char *Base(int n,
           int to)
{
    int k,size,i;
    char *result;
    char digit;

    if (to > 36) Error("Base","to > 36");
    if (to <= 1) Error("Base","think about it");

    size = (int) ((float) log((double) n) / log((double) to))+1;
    result = (char *) malloc(size+1);

    result[size] = 0;
    k=size-1;
    while (k>=0)
    {
	i = n % to;
	n = n / to;
	if (i<10) digit = i + '0';
	else digit = i -10 + 'a';
	result[k] = digit;
	k--;
    }
    return(result);
}

/* ------------------------------------------------------------------------
Function that creates a new file name. The name is based on a
conversion of the number of seconds since 1/01/70 into base 36. Two
calls will return different names is they are separated with more than
one second. The arguments are a prefix and an extension for the file
name.
------------------------------------------------------------------------ */
/*texi(um)---------------------------------------------------------------
@deftypefun {char *} FileNewName (char * @var{prefix}, char * @var{extension})
Function that creates a new file name. The name is based on a
conversion of the number of seconds since 1/01/70 into base 36. Two
calls will return different names is they are separated with more than
one second. The arguments are a prefix and an extension for the file
name. The value returned @code{char *} is the concatenation of the
@var{prefix}, the generated string and the @var{extension}. The
@var{prefix} and @var{extension} arguments are @code{char *}.
For example, a call to this function could return:
@example 
FileNewName("result", ".txt")
@result{} "resultd3hc1u.txt".
@end example
@end deftypefun
texi(end)---------------------------------------------------------------- */

char *FileNewName(char *prefix,
		  char *extension)
{
    int i;
    char *name,*filename;

    i = time(NULL);
    name = Base(i,36);
    filename = (char *) malloc(strlen(prefix) + strlen(extension) +
			       strlen(name) +1);
    strcpy(filename,prefix);
    strcat(filename,name);
    strcat(filename,extension);
    free(name);
    return(filename);
}
    

/*texi(um)---------------------------------------------------------------
@deftypefun int FileExist (char * @var{file-name})
Function that checks if a file exists. Returns @code{TRUE} if
@var{file-name} exists else returns @code{FALSE}. @var{file-name} is
@code{char *}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

int FileExist(char *filename)
{
    FILE *F;
    
    F = FileOpen(filename,"r");
    if (F == NULL)
    {  return(FALSE); }
    else 
    {
       FileClose(F);
       return(TRUE);
    }
}

#define NBCHAR 4000 

/*********************************************************************/
/*                                                                   */
/* Fonction qui lit une ligne dans un fichier jusqu'au \n et         */
/* renvoie un pointeur sur cette ligne.                              */
/* Le nombre maximum de caracteres de la ligne est NBCHAR            */
/*                                                                   */
/*********************************************************************/

/*texi(um)---------------------------------------------------------------
@node  Load/Save,  , File names, iof
@comment  node-name,  next,  previous,  up
@section Load and Save lines and numbers

@deftypefun {char *} LineRead (FILE * @var{stream})
This function reads a line from @var{stream}. Reads characters until it
founds @code{\n} or lines exceeds 4000 characters. This limitation should
@emph{not} exist. The function returns a character string, @var{stream} is
an opened stream (@code{FILE *}. 
@end deftypefun
texi(end)---------------------------------------------------------------- */

char *LineRead(FILE *input)
{
     int i=0;
     char *line;
     char c;

     line = (char *) malloc(NBCHAR);
     do 
     {
       c = fgetc(input);
       line[i] = c;
       i++;
     } while ((c != '\n')  &&
              (i < NBCHAR));
     line[i-1] = 0;
     return(line);
}

/*********************************************************************

 Fonctions qui lisent et ecrivent un entier dans un fichier.

*********************************************************************/

/*texi(um)---------------------------------------------------------------
@deftypefun void IntSave (int @var{n}, char *  @var{file-name})
Function that saves integer @var{n} in file @var{file-name}.
@end deftypefun
texi(end)---------------------------------------------------------------- */

void IntSave(int  i,
	     char *filename)
{
     FILE *F;
     
     F = FileOpen(filename,"w");
     if (F == NULL) exit(0);
     fprintf(F,"%d\n",i);
     FileClose(F);
}

/*texi(um)---------------------------------------------------------------
@deftypefun int IntLoad (char * @var{file-name})
Function that returns an interger value read in @var{file-name}. If it
fails opening @var{file-name} the function returns 0 after calling the
function @code{Warning}
@end deftypefun
texi(end)---------------------------------------------------------------- */

int IntLoad(char *filename)
{
     FILE *F;
     int i;

     F = FileOpen(filename,"r");
     if (F == NULL) return(0);
     fscanf(F,"%d",&i);
     FileClose(F);
     return(i);
}


/*********************************************************************

 Fonctions qui lisent et ecrivent un float dans un fichier.

*********************************************************************/

/*texi(um)---------------------------------------------------------------
@deftypefun void FloatSave (real @var{n}, char *  @var{file-name})
Function that saves a floating point number @var{n} in the file
@var{file-name}. 
@end deftypefun
texi(end)---------------------------------------------------------------- */

void FloatSave(float f,
	       char  *filename)
{
     FILE *F;
     
     F = FileOpen(filename,"w");
     if (F == NULL) exit(0);
     fprintf(F,"%e\n",f);
     FileClose(F);
}

/*texi(um)---------------------------------------------------------------
@deftypefun real FloatLoad (char * @var{file-name})
Function that returns a floatingpoint number read in file
@var{file-name}. It it is not possible to open @var{file-name}, the
function calls @code{Warning} and returns 0.0.
@end deftypefun
texi(end)---------------------------------------------------------------- */

float FloatLoad(char *filename)
{
     FILE *F;
     float f;
     int stat;

     F = FileOpen(filename,"r");
     if (F == NULL) return(0.0);
     stat = fscanf(F,"%e",&f);
     if (stat == 0) Error("fscanf","FloatLoad");
     FileClose(F);
     return(f);
}


char * ReplaceExtension(char * filename, char * newExt) {
  char * newFilename = malloc(sizeof(char) * /* ok that's a little
						more than necessary */
			      (strlen(filename) + strlen(newExt)));
  int pos = 0;
  strcpy(newFilename, filename);
  for (pos = strlen(filename) - 1; (pos >= 0) && (filename[pos] != '.'); pos--);
  if (filename[pos] == '.') {
    pos++;
    strcpy(&(newFilename[pos]), newExt);
    return newFilename;
  }
  return filename;
}  

char * RemoveExtension(char *filename) {
  // printf("FilenameName: [%s]\n ", filename);
  int n = strlen(filename);
  int k = n-1;
  int i;
  char * name;
  while (k >= 0 && filename[k] != '.') k--;
  if (k == -1) {
    // printf("****************************************************************************************\n");
    return strdup(filename);
  } else {
    name = (char *) malloc(sizeof(char) * (n - (n - k) + 1));
    for (i = 0; i < k; i++) name[i] = filename[i];
    name[i] = 0;
    return name;
  }
}

char * RemoveDirectory(char * filename) {
  int n = strlen(filename);
  int k = n-1;
  int i;
  char * name;
  // FIXME: não funciona em WINDOWS !
  while (k >= 0 && filename[k] != '/') k--;
  k++;
  // printf("[%s]\nn %d k %d n-k+1 %d \n",filename, n, k, n-k+1);
  name = (char *) malloc(sizeof(char) * (n-k+1));
  if (name != NULL) {
    // name = strdup(filename + k + 1);
    int j = 0;
    for (i = k; i < n; i++) name[j++] = filename[i];
    name[j] = 0;
    // printf("i %d name: [%s]\n", i, name);
    return name;
  } else {
    Error("RemoveDirectory", "Memory allocation error");
  }
}

char * FilenameName(char *pathname) {
  char * withoutDirectory = RemoveDirectory(pathname);
  char * fn = RemoveExtension(withoutDirectory);
  free(withoutDirectory);
  return fn;
}

int CountLines(char * filename) {
  FILE * input = FileOpen(filename, "r");
  int nlines = 0;
  int c;
  do {
    do 
      {
        c = fgetc(input);
      } while ((c != '\n') && !feof(input));
    nlines++;
  } while (!feof(input));
  FileClose(input);
  // printf("nlines : %d\n", nlines);
  return nlines;
}

char * linebuffer = NULL;
int buffersize = 0;

void ReallocBuffer() {
  int i = 0;
  buffersize += NBCHAR;
  linebuffer = realloc(linebuffer,buffersize);
  if (linebuffer != NULL) {
    for (i = buffersize - NBCHAR; i < buffersize; i++)
      linebuffer[i] = 0;
  } else {
    Error("ReallocBuffer", "Allocation error.");
  }
  if (buffersize > NBCHAR) fprintf(stderr, "Line buffer reallocated : %d\n", buffersize);
}

char * ReadOneLine(FILE * input) {
  int i = 0;
  int c = 0;
  int cont = TRUE;
  char * line;
  if (linebuffer == NULL) ReallocBuffer();
  do {
    do {
      c = fgetc(input);
      if ((c != '\n') && !feof(input)) linebuffer[i++] = c;
    } while (c != '\n' && !feof(input) && i < buffersize);
    // printf("feof: %d i %d c %d\n", feof(input), i, c);
    if (i == buffersize && c != '\n' && !feof(input)) {
      ReallocBuffer();
      cont = TRUE;
    } else cont = FALSE;
  } while (cont);
  linebuffer[i] = 0;
  // printf("read line : [%s]\n", linebuffer);
  /* 
  line = calloc((i+1), sizeof(char));
  strcpy(line, linebuffer);
  return line;
  */
  return linebuffer;
}

char ** ReadLines(char * filename, int * nLines) {
  int nlines = CountLines(filename);
  char ** result = (char **) calloc(nlines, sizeof(char *));
  int i = 0;
  char * line;
  char * newline;
  FILE * input = FileOpen(filename, "r");
  do {
    line = ReadOneLine(input);
    newline = (char *) malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(newline, line);
    // printf("i %d line [%s]\n", i, line);
    result[i++] = newline;
  } while (!feof(input));
  if (strlen(newline) == 0) nlines--;
  *nLines = nlines;
  FileClose(input);
  return result;
}


