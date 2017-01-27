
#ifdef __APPLE__ // mac os x specific
	#include "sys/malloc.h"
#else
	#include <malloc.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "error.h"

/**
 * PRE: 
 *      s is a null-terminated char array.
 * POS:
 *      returns the number of occurences of char c found in s.
 * ALLOC:
 *      none.
 */
int charCount(char c, char *s) {
  int i = 0;
  int count = 0;
  while (s[i] != 0) {
    if (s[i] == c) count++;
    i++;
  }
  return count;
}


/**
 * PRE: 
 *      s is a null-terminated char array. table is a array of size 
 *      null-terminated strings.
 * POS:
 *      returns the posistion of string s in table. if s is not found -1 is 
 *      returned.
 * ALLOC:
 *      none.
 */
int findPosition(char * s, char ** table, int size) {
  int i = 0;
  do {
    if (strcmp(s, table[i]) == 0) return i;
    i++;
  } while (i < size);
  return -1;
}

int findStringPosition(char * token, char * s) {
  int i,j;
  for (i = 0; i < strlen(s) - strlen(token) + 1; i++) {
    j = 0;
    while (s[i+j] == token[j] && j < strlen(token)) {
      j++;
    }
    if (j == strlen(token)) {
      return i;
    }
  }
  return -1;
}

/**
 * PRE: 
 *      s is a null-terminated char array.
 * POS:
 *      returns an array of strings filled with copies of substrings of s, split
 *      a positions of character c. The character c is not part of the 
 *      substrings.
 * ALLOC:
 *      an array of char * is created. The size of this array is not returned 
 *      but it corresponds to charCount(c,s) + 1. Each position of this array 
 *      is allocated in order to keep a copy of each substring.
 */
char ** splitAtChar(char c, char * s) {
  int k = charCount(c,s);
  // printf("k: %d\n" , k);
  char ** result = (char **) malloc(sizeof(char *) * k+1);
  if (result == NULL) {
    Error("splitAtChar", "Memory allocation error.");
  }
  char * tmp = strdup(s);
  char * tmp2 = tmp;
  int i;
  for (i = 0; i < strlen(tmp); i++)
    if (tmp[i] == c) tmp[i] = 0;
  i = 0;
  do {
    int l = strlen(tmp)+1;
    result[i] = (char *) malloc(sizeof(char) * l);
    strcpy(result[i], tmp);
    tmp += l;
    i++;
  } while (i < k+1);
  free(tmp2);
  return result;  
}

/**
 * PRE: 
 *      s is a null-terminated char array.
 * POS:
 *      returns the position of the first occurence of c in s.
 *      if c is not found, returns -1
 * ALLOC:
 *      none.
 */
int charFirstPosition(char c, char * s) {
  int i;
  for (i = 0; i < strlen(s); i++) {
    if (s[i] == c) return i; 
  }
  return -1;
}

/**
 * PRE: 
 *      s is a null-terminated char array.
 * POS:
 *      returns the position of the last occurence of c in s.
 *      if c is not found, returns -1
 * ALLOC:
 *      none.
 */
int charLastPosition(char c, char * s) {
  int k;
  int i = strlen(s) -1;
  for (k = i; k>= 0; k--) 
    if (s[k] == c) return k;
  return -1;
}

/**
 * PRE: 
 *      s is a null-terminated char array.
 * POS:
 *      returns a new string that contains the characters of s before the 
 *      first occurence of c. If c is not found in s a copy of s is returned.
 * ALLOC:
 *      a new string is allocated and returned.
 */
char * copyBeforeChar(char c, char * s) {
  int k = 0;
  int l = strlen(s);
  char * result;
  while (k < l && s[k] != c) k++;
  // printf("k %d\n", k);
  result = (char *) malloc(sizeof(char) * (k+1));
  if (result != NULL) {
    strncpy(result,s,k);
    result[k] = 0;
    // printf("cut s [%s] k %d : [%s]\n", s, k, result);
    return result;
  } else {
    Error("copyBeforeChar", "Memory allocation error"); 
  }
}

/**
 * PRE: 
 *      s is a null-terminated char array.
 * POS:
 *      returns a new string that contains the characters of s after the 
 *      first occurence of c. If c is not found in s a zero length string is 
 *      returned.
 * ALLOC:
 *      a new string is allocated and returned.
 */
char * copyAfterChar(char c, char * s) {
  int k = 0;
  int l = strlen(s);
  char * result;
  while (k < l && s[k] != c) { k++; }
  k++;
  if ((l - k + 1) != 0) {
    result = (char *) malloc(sizeof(char) * (l - k + 1));
    if (result != NULL) {    
      // printf("k %d s [%s] sk [%s] size %d\n", k, s, s+k,(l - k + 1));
      strcpy(result,s+k);
    } else {
      Error("copyAfterChar", "Memory allocation error"); 
    }
  } else {
    result = (char *) malloc(sizeof(char));
    result[0] = 0;
  }
  return result;
}

/**
 * PRE: 
 *  stringArray is an array of char * of size >= size. stringArray
 *  must have been initialized with NULL (each position).
 *  size is the number of strings in stringArray. Strings are stored
 *  in the array begining with position 0.
 *  s is a string. 
 * POS:
 *  if the string s is found in stringArray, stringArray is unchanged
 *  and size is returned. if the string s is not found in stringArray,
 *  a copy of s is stored in the position size of the array and size +
 *  1 is returned.
 * ALLOC:
 *  the function allocates space to store the string s.
 */
int addIfNew(char ** stringArray, int size, char * s) {
  int i = 0;
  while (i < size && strcmp(s,stringArray[i]) != 0) { 
    // printf("sa %X sa[%d] [%s]\n",stringArray ,i,stringArray[i]); 
    i++; 
  }
  // printf("i: %d\n", i);
  if (stringArray[i] != NULL && strcmp(s,stringArray[i]) == 0) {
    // printf("already exists\n");
    return size;
  } else {
    // printf("add new element [%s] \n", s);
    stringArray[i] = (char *) malloc(sizeof(char) * (strlen(s)+1));
    strcpy(stringArray[i], s);
    return size + 1;
  }
}


void equalizeLengths(char ** s, int size) {
  int i,k,n;
  int max = 0;
  char * aux;
  for (i = 0; i < size; i++) {
    k = strlen(s[i]);
    if (k > max) max = k;
  }
  max++;
  // printf("max=%d\n", max);
  for (i = 0; i < size; i++) {
    k = strlen(s[i]);    
    aux = (char *) malloc(sizeof(char) * max);
    strcpy(aux, s[i]);
    for (n = 0; n < max - k-1; n++) strcat(aux, " ");
    free(s[i]);
    s[i] = aux;
    // printf("s[i]: [%s]\n", s[i]);
  }
  // printf("done\n");  
}

/* Stolen from the man page example of qsort. */
static int cmpstringp(const void *p1, const void *p2) {
           /* The actual arguments to this function are "pointers to
              pointers to char", but strcmp(3) arguments are "pointers
              to char", hence the following cast plus dereference */
  return strcmp(* (char * const *) p1, * (char * const *) p2);
}

void SortStrings(char ** table, int size) {
  qsort(&table[0], size, sizeof(char *), cmpstringp);
  printf("SORTED!\n");
  int i;
  for (i = 0; i < size; i++) printf("%d: [%s]\n", i, table[i]);
}


/*
int main (int argc, char **argv) {
  char * s1 = malloc(sizeof(char) * 10);
  strcpy(s1,"abcdefgh ijkl");
  char * s2 = copyAfterChar('h', s1);
  printf("s2 = [%s]\n", s2);
  char * s3 = copyAfterChar('z', s1);
  printf("s3 = [%s]\n", s3);
}
*/
