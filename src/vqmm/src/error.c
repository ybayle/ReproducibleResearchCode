#include <stdio.h>
#include <stdlib.h>

 /* --------------- */
 /* --------------- */
 /* Error functions */
 /* --------------- */
 /* --------------- */


void Error(char *FUNCTION, char *CAUSE)
{
     fprintf(stderr,"*** Error in function %s. Cause: %s.\n",FUNCTION,CAUSE);
     exit(1);
}


void Warning(char *FUNCTION, char *CAUSE)
{
     fprintf(stderr,"*** Warning in function %s. Cause: %s.\n",FUNCTION,CAUSE);
}

 /* --------------------- */
 /* --------------------- */
 /* Error functions (END) */
 /* --------------------- */
 /* --------------------- */


