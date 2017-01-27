/* -*- C -*- */

#ifdef __APPLE__ // mac os x specific
	#include "sys/malloc.h"
#else
	#include <malloc.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "define.h"
#include "matrix.h" 
#include "iof.h" 
#include "string-utils.h"
#include "error.h"
#include "random.h"
#include "matrand.h"
#include "numerica2.h"

static char *dataFilename = NULL;
static char *quiet;
static int codebookSize;
static real epsilon;
static real smoothing;
static int randomGeneratorInitializationMethod;
static char *codebookFilename;
static int encode = FALSE;
static int makecodebook = FALSE;
static int makeclassmodels = FALSE;
static int maketagmodels = FALSE;
static int classify = FALSE;
static int tagify = FALSE;
static char * listoffilesfile = NULL;
static FILE * output;
static char * outputFile;
static char * outputDir;
static FILE * input;
static char * inputFile;
static char * models;
// static int dimlim = 0;

#define DEFAULT -1
#define NEWSEED -2

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
void PrintHelp() {
    fprintf(stderr,
         "Usage: vqmm -quiet y|n -data <data-filename> -codebook-size <n>\n"
" -epsilon <small-value> -smoothing <value in [0.0, 1.0]> -random new-seed > <my-filename>\n");
    fprintf(stderr, 
            "The program has 4 different modes:\n"
"1. Codebook creation.\n"
"2. Feature vector sequence encoding (according to a codebook).\n"
"3. Markov Model estimation.\n"
"4. Classification.\n");
    fprintf(stderr,
"1. Codebook creation. The input parameters are : \n"
"a. the codebook size,\n"
"b. the data set to be used. (a single matrix nxd with d = dimensionality of data)\n"
);
    // fprintf(stderr, "The -random option is optional. When \"new-seed\" is used, a new seed \n");
    // fprintf(stderr, "is generated before starting the algorithm. If the option is not used\n");
    // fprintf(stderr, "the default random state is used.\n");
    // fprintf(stderr, "Version 0.2\n");
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
int getCommand(int argc, 
	       char *argv[],
	       char *command,
	       char **value) {
    int i;
    int ok = FALSE;
    if (value == NULL) {
      for(i=1;(ok==FALSE) && (i < argc);i+=1) {
        if (strcmp(command,argv[i]) == 0) {
          ok = TRUE;
	}
      } 
    } else {
      for(i=1;(ok==FALSE) && (i < argc);i+=2) {
        if (strcmp(command,argv[i]) == 0) {
          *value = (char *) malloc(strlen(argv[i+1])+1);
          if (*value == 0) {
            puts("Allocation error.");
            exit(1);
          }
          strcpy(*value, argv[i+1]);
          ok = TRUE;
        }
      }
      if (ok == FALSE) {
        for(i=2;(ok==FALSE) && (i < argc);i+=2) {
          if (strcmp(command,argv[i]) == 0) {
            *value = (char *) malloc(strlen(argv[i+1])+1);
            if (*value == 0) {
              puts("Allocation error.");
              exit(1);
            }
            strcpy(*value, argv[i+1]);
            ok = TRUE;
          }
        }
      }
    }
    return(ok);
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
void CommandLine(int argc, char **argv) {
  char *codebooksizestr;
  char *epsilonstr;
  char *smoothingstr;
  char *randomstr;
  char * dimlimstr;
  int stat;

  input =  NULL;
  output =  NULL;
  codebooksizestr = (char *) malloc(10);
  epsilonstr = (char *) malloc(20);
  smoothingstr = (char *) malloc(20);
  /*
  if (argc < 5) {
    PrintHelp();
    exit(-1);
  }
  */
  if (getCommand(argc, argv, "-help", NULL)) {
    PrintHelp();
    exit(0);
  }
  /* if the codebook-size option appear on the command line, entre "make codebook" mode. */
  if (!getCommand(argc, argv, "-codebook-size", &codebooksizestr)) {
    codebookSize = 0;
  } else {
    sscanf(codebooksizestr,"%d",&codebookSize);
    makecodebook = TRUE;
  }
  if (makecodebook) {
    if (!getCommand(argc, argv, "-data", &dataFilename)) {
      dataFilename = NULL;
      if (!getCommand(argc, argv, "-list-of-files", &listoffilesfile)) {
        fprintf(stderr,"you must specify the data set to be used to build the codebook.\n");
        PrintHelp();
        exit(1);
      }
    }
    if (!getCommand(argc, argv, "-random", &randomstr)) {
      randomGeneratorInitializationMethod = DEFAULT; 
    } else if (strcmp(randomstr, "new-seed") == 0) {
      randomGeneratorInitializationMethod = NEWSEED;
    } else {	
      sscanf(randomstr,"%d",&randomGeneratorInitializationMethod);
    }
  }
  /* if the option encode appears on the command line the program will produce a 
     sequence of codes from a file that contains feature vectors (one by line). */
  if (getCommand(argc, argv, "-encode", NULL) &&
      getCommand(argc, argv, "-codebook", NULL)) {
    encode = TRUE;
  }
  if (getCommand(argc, argv, "-encode", NULL) &&
      !getCommand(argc, argv, "-codebook", NULL)) {
    fprintf(stderr,"you must specify the codebook file to be used for encoding.\n");
    PrintHelp();
    exit(1);
  }
  if (encode) {
    if (!getCommand(argc, argv, "-output", &outputFile))
      output = stdout;
    else
      output = FileOpen(outputFile, "w");
    if (!getCommand(argc, argv, "-input", &inputFile)) {
      input = stdin;
    } else {
      input = FileOpen(inputFile, "r");
    }
  }
  /* if the option make-models appears on the command line the program will
     produce models given a file that contains tab separated filename and class 
     and a codebook. */
  if (getCommand(argc, argv, "-codebook", NULL) &&
      getCommand(argc, argv, "-make-class-models", NULL) &&
      getCommand(argc, argv, "-list-of-files", &listoffilesfile)) {
    // getCommand(argc, argv, "-list-of-files", &listoffilesfile)
    makeclassmodels = TRUE;
  }
  if (getCommand(argc, argv, "-codebook", NULL) &&
      getCommand(argc, argv, "-make-tag-models", NULL) &&
      getCommand(argc, argv, "-list-of-files", &listoffilesfile)) {
    // getCommand(argc, argv, "-list-of-files", &listoffilesfile)
    maketagmodels = TRUE;
  }
  if (!getCommand(argc, argv, "-quiet", &quiet)) {
    quiet = (char *) malloc(2);
    strcpy(quiet, "n");
  }
  /* */
  if (getCommand(argc, argv, "-codebook", NULL) &&
      getCommand(argc, argv, "-classify", NULL) &&
      getCommand(argc, argv, "-list-of-files", &listoffilesfile) &&
      getCommand(argc, argv, "-models", &models)) {
    classify = TRUE;
  }
  if (getCommand(argc, argv, "-codebook", NULL) &&
      getCommand(argc, argv, "-tagify", NULL) &&
      getCommand(argc, argv, "-list-of-files", &listoffilesfile) &&
      getCommand(argc, argv, "-models", &models)) {
    tagify = TRUE;
  }
  if (!getCommand(argc, argv, "-epsilon", &epsilonstr)) {
    epsilon = 0.0;
  } else {
    stat = sscanf(epsilonstr,"%lf",&epsilon);
    // printf("epsilon = %lf\n", epsilon); exit(1);
  }
  if (!getCommand(argc, argv, "-smoothing", &smoothingstr)) {
    // no smoothing, use the "+epsilon method"
    smoothing = -1;
  } else {
    stat = sscanf(smoothingstr,"%lf",&smoothing);
    // printf("smoothing = %lf\n", smoothing); exit(1);
  }
  if (!getCommand(argc, argv, "-codebook", &codebookFilename)) {
    codebookFilename = 0;
  } 
  if (!getCommand(argc, argv, "-output-dir", &outputDir)) {
    outputDir = 0;
  } 
  // This was probably a bad idea. Limiting the dimensionality of data
  // using a command line parameter would complicate a lot the scheme
  // used to avoid collisions between file names.   
  /*
  if (!getCommand(argc, argv, "-limit-dim", &dimlimstr))
    dimlim = 0;
  else
    stat = sscanf(dimlimstr,"%d",&dimlim);
  */
}

/* TODO: these function should be moved to an other file. */


char * copyUntilTab(char * s) {
  return copyBeforeChar('\t',s);
}

char * copyAfterTab(char * s) {
  return copyAfterChar('\t', s);
}


/**
 * PRE: 
 * POS:
 * ALLOC:
 */
matrix * MakeCodebookFromMatrix(matrix * data, 
                                int size, 
                                real epsilon, 
                                char *codebookFilename) {
  matrix * centers;
  centers = MatAlloc(size, data->jmax);
  if (randomGeneratorInitializationMethod == NEWSEED)
    RandomInitGenerator();
  else RandomInitGenerator1(randomGeneratorInitializationMethod);
  // MatPrint(data);
  if (strcmp(quiet, "n") == 0) {
    fprintf(stderr, "data is %dx%d\n", data->imax, data->jmax);
    fprintf(stderr, "codebook size: %d\n", size);
    fprintf(stderr, "epsilon: %6.4f\n", epsilon);
    kMeans(data, centers, 0, epsilon);
  }
  else
    kMeans(data, centers, 1, epsilon);
  if (codebookFilename != NULL) {
    MatSave(centers, codebookFilename);
  } else {
    MatPut(stdout,centers);
  }
  return centers;
}

matrix * CopyXColumns(matrix * m, int n) {
  matrix * m2 = MatAlloc(m->imax, n);
  int i,j;
  for (i = 0; i < m->imax; i++) 
    for (j = 0; j < n; j++)
      Mat(m2,i,j) = Mat(m,i,j);
  return m2;
}

// This was probably a bad idea. Limiting the dimensionality of data
// using a command line parameter would complicate a lot the scheme
// used to avoid collisions between file names.   
matrix * MatLoadLimitingDim(char * filename) {
  matrix * data;
  matrix *reducedData;
  data = NULL;
  data = MatLoad(data, filename);
  /* 
     if (dimlim != data->jmax && dimlim > 0) {
     reducedData = CopyXColumns(data,dimlim);
     MatFree(data);
     return reducedData;
  } else
  */ 
    return data;  
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
matrix * MakeCodebookFromFile(char * filename, 
                              int size, /* aka k2 */
                              real epsilon,
                              char *codebookFilename) {
    matrix *data;
    matrix *reducedData;
    matrix *centers;
    data = MatLoadLimitingDim(filename);
    return MakeCodebookFromMatrix(data, size, epsilon, codebookFilename);
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
matrix * MakeCodebookFromListOfFiles(char * listOfFilesFilename,
                                     int levelOneSampling, /* aka k1 */
                                     int size, /* aka k2 */
                                     real epsilon,
                                     char *codebookFilename) {
  int nfiles;
  char ** listOfFiles = ReadLines(listOfFilesFilename, &nfiles);
  int i;
  matrix * oneFileContent;
  matrix * oneFileSample;
  matrix * data = NULL;
  matrix * tmp;
  char * filename;
  for (i = 0; i < nfiles ; i++) {
    filename = copyUntilTab(listOfFiles[i]);
    fprintf(stderr, "Selecting samples in file [%s]\r", filename);
    oneFileContent = MatLoadLimitingDim(filename);
    free(filename);
    if (oneFileContent -> imax > levelOneSampling) {
      oneFileSample = MatAlloc(levelOneSampling, 
                               oneFileContent->jmax);
      RandomSubSet(oneFileContent, oneFileSample);
    } else {
      oneFileSample = oneFileContent;
    }
    if (i == 0) {
      data = oneFileSample;
      // MatPrint(data);
    } else {
      // printf("data\n"); MatPrint(data);
      // printf("oneFileSample\n"); MatPrint(oneFileSample);
      tmp = MatAppendRows(data, oneFileSample);
      // printf("tmp\n"); MatPrint(tmp);
      if (oneFileSample != oneFileContent) { MatFree(oneFileSample);}
      MatFree(oneFileContent);
      MatFree(data);
      data = tmp;
    }
  } 
  printf("\nDone\n");
  return MakeCodebookFromMatrix(data, size, epsilon, codebookFilename);
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
int encodesinglerow(int row, matrix * data, matrix * codebook) {
  int datadim = codebook->jmax;
  int codebookSize = codebook->imax;
  double n, mindist, d, stop = 0;
  int minindice,i,j;
  /* data should be nxm with m == codebook->jmax */
  mindist = MAXDOUBLE; 
  minindice = 0;
  // printf("cbs %d row %d dim %d\n", codebookSize, row, datadim); 
  // MatPrint(codebook);
  // exit(0);
  for (i = 0; i < codebookSize; i++) {
    d = 0.00;
    for (j = 0; j < datadim && d < mindist; j++) {
      d += (Mat(codebook,i,j) - Mat(data,row,j)) * (Mat(codebook,i,j) - Mat(data,row,j));
    }
    if (d < mindist) {
      mindist = d;
      minindice = i;
    }
  }
  return minindice;
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
int encodesingleframe(matrix * data, matrix * codebook) {
  return encodesinglerow(0,data,codebook);
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
void encodeStream(FILE * inputstream, matrix * codebook, FILE *outputstream) {
  int datadim = codebook->jmax;
  matrix * v = MatAlloc(1,datadim);
  int i;
  double value;
  int status;
  while (!feof(inputstream)) {
    i = 0;
    do {
      status = fscanf(inputstream, "%lf", &value);
      if (status == 1) {
        Mat(v,0,i) = value;
        // printf("[%d] %lf ", i, value);
      }
      i++;
    } while (i < datadim && status == 1 && !feof(inputstream));
    // printf("\n");
    // MatPrint(v);
    if (i != datadim || feof(inputstream)) return;
    fprintf(outputstream, "%d ", encodesingleframe(v,codebook));
    fflush(outputstream);
  }
  MatFree(v);
}

/**
 * PRE: 
 * POS:
 * ALLOC: 
 *     Creates (and returns) an array of int of size input->imax.
 */
int * encodematrix(matrix * input, matrix * codebook) {
  int * codes = (int *) calloc(input->imax, sizeof(int));
  int i = 0;
  if (codes != NULL) {
    for (i = 0; i < input -> imax; i++) {
      codes[i] = encodesinglerow(i,input,codebook);
    }
  } else {
    Error("encodeMatrix", "Allocation error.");
  }
  return codes;
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
matrix * MakeMarkovModelFromListOfFiles(char ** filenames, int nfiles, matrix * codebook) {
  matrix * count = MatAlloc(codebook->imax, codebook->imax);
  matrix * filecontent;
  int i = 0;
  int * codes;
  int k = 0;
  int j = 0;
  real sum = 0.0;
  if (epsilon == 0.0) {
    Error("MakeMarkovModelFromListOfFiles","You must set the epsilon parameter to a small value for smoothing the model.");
    exit(1);
  }
  printf("Computing model using %d files\n", nfiles);
  for (i = 0; i < nfiles; i++) {
    printf("%d/%d\r", i+1, nfiles); fflush(stdout);
    filecontent = MatLoadLimitingDim(filenames[i]);
    codes = encodematrix(filecontent, codebook);
    for (k = 1; k < filecontent->imax; k++) {
      Mat(count, codes[k-1], codes[k]) = Mat(count, codes[k-1], codes[k]) + 1;
    }
    MatFree(filecontent);
    free(codes);
  }
  printf("\n");
  for (i = 0; i < count->imax; i++) {
    sum = 0.0;
    for (j = 0; j < count->jmax; j++) sum += Mat(count, i, j);
    if (sum != 0) {
      for (j = 0; j < count->jmax; j++) {
        Mat(count,i,j) = Mat(count,i,j) / sum;
      }
    }
  }
  for (i = 0; i < count->imax; i++) {
    for (j = 0; j < count->jmax; j++) {
      if (Mat(count,i,j) == 0.0) Mat(count,i,j) = epsilon;
    } 
  }
  return count;
}


matrix * MakeMarkovModelFromListOfFiles_AlternateSmoothingMethod(char ** filenames, int nfiles, matrix * codebook) {
  matrix * count = MatAlloc(codebook->imax, codebook->imax);
  matrix * filecontent;
  int i = 0;
  int * codes;
  int k = 0;
  int j = 0;
  real sum = 0.0;
  if (smoothing == 0.0) {
    Error("MakeMarkovModelFromListOfFiles_AlternateSmoothingMethod","You must set the smoothing parameter to a small value for smoothing the model.");
    exit(1);
  }
  printf("Computing model using %d files\n", nfiles);
  for (i = 0; i < nfiles; i++) {
    printf("%d/%d\r", i+1, nfiles); fflush(stdout);
    filecontent = MatLoadLimitingDim(filenames[i]);
    codes = encodematrix(filecontent, codebook);
    for (k = 1; k < filecontent->imax; k++) {
      Mat(count, codes[k-1], codes[k]) = Mat(count, codes[k-1], codes[k]) + 1;
    }
    MatFree(filecontent);
    free(codes);
  }
  printf("\n");
  int * zeroCount = (int *) malloc(sizeof(int) * count->imax);
  for (i = 0; i < count->imax; i++) {
    sum = 0.0;
    zeroCount[i] = 0;
    for (j = 0; j < count->jmax; j++) {
      sum += Mat(count, i, j);
      if (Mat(count,i,j) == 0)
        zeroCount[i]++;
    }
    if (sum != 0) {
      for (j = 0; j < count->jmax; j++) {
        Mat(count,i,j) = Mat(count,i,j) / sum;
      }
    }
  }
  // MatPrint(count);
  for (i = 0; i < count->imax; i++) {
    for (j = 0; j < count->jmax; j++) {
      Mat(count,i,j) = Mat(count,i,j) * (1 - smoothing);
      if (Mat(count,i,j) == 0.0)
        Mat(count,i,j) = smoothing / zeroCount[i];
    } 
  }
  // printf("----------------------------------------\n");
  // MatPrint(count);
  free(zeroCount);
  return count;
}


/**
 * PRE: 
 * POS:
 * ALLOC:
 */
int classMatch(char * fileClassPair, char * class) {
  // printf("fcp [%s] c [%s] ", fileClassPair, class);
  // FIXME: memory leak here ?
  char * p = copyAfterTab(fileClassPair);
  // printf(" -> %d \n ",strcmp(class, p) == 0);
  return (strcmp(class, p) == 0);
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
void getClasses(int nfiles,             // the number of files / audio excerpts
                char ** fileClassPairs, // an array of strings like : <filename>TAB<tag>
                char *** classesTmp,    // returns an array of string
                                        // that correspond to the
                                        // tags/classes of the datat set.
                char **** filesPerClass,// returns an array of arrays
                                        // of strings that contains
                                        // for aech tag, the files
                                        // that have this tag.
                int ** nbFilesPerClass, // returns an array of int
                                        // that constains the number
                                        // of files for each tag.
                int * nClasses          // returns the number of classes/tags
                ) {
  char ** classesTmp2  = (char **) calloc(nfiles,sizeof(char *));
  int i,j,k=0,m=0;
  char * p;
  char *** filesPerClass2;
  int * nbFilesPerClass2;
  for (i = 0; i < nfiles; i++) {
    // printf("%d [%s] ", i, fileClassPairs[i]);
    // extract the tag name
    p = copyAfterTab(fileClassPairs[i]);
    // printf("p [class: %s]\n",  p);
    // collect all tags names
    k = addIfNew(classesTmp2,k,p);
    free(p);
    // for (j = 0; j < k; j++) printf("%X >class[%d] = [%s]\n", classesTmp2, j, classesTmp2[j]);
  }
  SortStrings(classesTmp2,k);
  // classesTemp2 contains the tags found in the data set.
  // for (i = 0; i < k; i++) printf("class[%d] = [%s]\n", i, classesTmp2[i]);
  fprintf(stderr, "#Classes: %d\n", k);
  *nClasses = k;
  filesPerClass2 = (char ***) malloc(sizeof(char **) * k);
  nbFilesPerClass2 = (int *) calloc(k,sizeof(int));
  // Compute the number of files in each class/tag
  for (i = 0; i < k; i++) { // for each tag
    for (j = 0; j < nfiles; j++) { // for each file
      if (classMatch(fileClassPairs[j], classesTmp2[i]))
        nbFilesPerClass2[i] ++;
    } 
  }
  // compute the array of files for each tag/class
  for (i = 0; i < k; i++) {
    m=0;
    // printf(">>nFiles in class %d [%s] = %d\n", i, classesTmp2[i], nbFilesPerClass2[i]);
    filesPerClass2[i] = (char **) malloc(sizeof(char *) * nbFilesPerClass2[i]);
    for (j = 0; j < nfiles; j++) {
      // for each tag collect the files that have this tag.
      if (classMatch(fileClassPairs[j], classesTmp2[i]))
        filesPerClass2[i][m++] = copyUntilTab(fileClassPairs[j]);
    }
  }
  *classesTmp = classesTmp2;
  *filesPerClass = filesPerClass2;
  *nbFilesPerClass = nbFilesPerClass2;
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
void MakeModels(char * listOfFilesFilename, matrix * codebook) {
  int nfiles;
  // an array of lines. Each line is composed by a filename followed
  // by a TAB followed by a string that corresponds to a tag. nfiles
  // is the number of lines in the file (and in the array).
  char ** fileClassPairs = ReadLines(listOfFilesFilename, &nfiles);
  char *** FilesPerClass;
  int * nFilesPerClass;
  char ** classes;
  int nClasses;
  int i, k;
  // the name of the dataset:
  char * dataFiles = FilenameName(listOfFilesFilename);
  // printf("start fcp[0] %s\n", fileClassPairs[0]);
  getClasses(nfiles, fileClassPairs, &classes, &FilesPerClass, &nFilesPerClass, &nClasses);
  char *codebookName = FilenameName(codebookFilename);
  for (k = 0; k < nClasses; k++) {
    matrix * model = (smoothing == -1 
                      ? MakeMarkovModelFromListOfFiles(FilesPerClass[k], nFilesPerClass[k], codebook)
                      : MakeMarkovModelFromListOfFiles_AlternateSmoothingMethod(FilesPerClass[k], nFilesPerClass[k], codebook)); 
    char * modelName = (char *) malloc(sizeof(char) * ((outputDir == 0 ? 0 : strlen(outputDir)) + 
                                                       strlen(classes[k]) + 1 +
                                                       strlen(codebookName) +
                                                       strlen(dataFiles) + 1 +
                                                       4 + 1));
    if (outputDir == 0)
      strcpy(modelName, classes[k]);
    else {
      strcpy(modelName, outputDir);
      strcat(modelName, classes[k]);
    }
    strcat(modelName, "$");
    //strcat(modelName, codebookName);
    //strcat(modelName, ".");
    strcat(modelName, dataFiles);
    strcat(modelName, ".mm");
    printf("Saving model [%s]\n", modelName);
    MatSave(model,modelName);
  }
  for (i = 0; i < nfiles; i++) {
    free(fileClassPairs[i]);
  }
  free(fileClassPairs);
}


char ** uniqueFiles(char ** fileClassPairs, int nfiles, int * nUniqueFiles) {
  int k = 0;
  int i;
  char * filename;
  char ** uniqueFilenames = (char **) calloc(nfiles,sizeof(char *));
  for (i = 0; i < nfiles; i++) {
    filename = copyUntilTab(fileClassPairs[i]);
    k = addIfNew(uniqueFilenames,k,filename);
    free(filename);
  }
  *nUniqueFiles = k;
  // for (i = 0; i < k; i++) printf("%d [%s]\n", i, uniqueFilenames[i]);
  return uniqueFilenames;
}

// Idem MakeModels but for autotagging. The difference is that a pair
// of models is build for each tag, one (positive) with the files that
// have the tag an one (negative) with the files that do not have the
// tag. 

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
void MakeModelsForTagging(char * listOfFilesFilename, matrix * codebook) {
  int nfiles;
  // an array of lines. Each line is composed by a filename followed
  // by a TAB followed by a string that corresponds to a tag. nfiles
  // is the number of lines in the file (and in the array).
  char ** fileClassPairs = ReadLines(listOfFilesFilename, &nfiles);
  char *** FilesPerClass;
  int * nFilesPerClass;
  char ** classes;
  int nClasses;
  int i, j, k, l;
  int found;
  // the name of the dataset:
  char * dataFiles = FilenameName(listOfFilesFilename);
  char * filename;
  // printf("start fcp[0] %s\n", fileClassPairs[0]);
  getClasses(nfiles, fileClassPairs, &classes, &FilesPerClass, &nFilesPerClass, &nClasses);
  int nUniqueFilenames;
  char ** uniqueFilenames = uniqueFiles(fileClassPairs,nfiles,&nUniqueFilenames);
  char *codebookName = FilenameName(codebookFilename);
  for (k = 0; k < nClasses; k++) {
    printf("Training model for tag [%s] (%d/%d)\n", classes[k], k+1, nClasses);
    matrix * model = (smoothing == -1 
                      ? MakeMarkovModelFromListOfFiles(FilesPerClass[k], nFilesPerClass[k], codebook)
                      : MakeMarkovModelFromListOfFiles_AlternateSmoothingMethod(FilesPerClass[k], nFilesPerClass[k], codebook));
    char * modelName = (char *) malloc(sizeof(char) * ((outputDir == 0 ? 0 : strlen(outputDir)) + 
                                                       strlen(classes[k]) + 1 +
                                                       strlen(codebookName) +
                                                       strlen(dataFiles) + 1 +
                                                       4 + 1));
    if (outputDir == 0)
      strcpy(modelName, classes[k]);
    else {
      strcpy(modelName, outputDir);
      strcat(modelName, classes[k]);
    }
    strcat(modelName, "$");
    // strcat(modelName, codebookName);
    // strcat(modelName, ".");
    strcat(modelName, dataFiles);
    strcat(modelName, ".mm");
    printf("Saving model [%s]\n", modelName);
    MatSave(model,modelName);
    // train negative model
    char ** negativeSamples = (char **) malloc(sizeof(char*) * (nUniqueFilenames - nFilesPerClass[k]));
    printf("nUniquefiles %d\nnFilesinclass %d\nNegative model should be trained with %d files\n", 
           nUniqueFilenames, nFilesPerClass[k],  (nUniqueFilenames - nFilesPerClass[k]));
    l = 0;
    for (i = 0; i < nUniqueFilenames; i++) {
      j = findPosition(uniqueFilenames[i], FilesPerClass[k], nFilesPerClass[k]);
      if (j == -1) {
        negativeSamples[l] = uniqueFilenames[i];
        l++;
      }
    }
    printf("Files collected for training negative model: %d\n", l);
    if (l != (nUniqueFilenames - nFilesPerClass[k])) exit(1);
    matrix * negModel = (smoothing == -1 
                         ? MakeMarkovModelFromListOfFiles(negativeSamples, l, codebook)
                         : MakeMarkovModelFromListOfFiles_AlternateSmoothingMethod(FilesPerClass[k], nFilesPerClass[k], codebook));
    free(modelName);
    free(negativeSamples);
    modelName = (char *) malloc(sizeof(char) * ((outputDir == 0 ? 0 : strlen(outputDir)) + 
                                                strlen(classes[k]) + 1 +
                                                strlen(codebookName) +
                                                strlen(dataFiles) + 1 +
                                                4 + 1 + 4));
    if (outputDir == 0) {
      strcpy(modelName, "NOT_");
      strcat(modelName, classes[k]);
    }
    else {
      strcpy(modelName, outputDir);
      strcat(modelName, "NOT_");      
      strcat(modelName, classes[k]);
    }    
    strcat(modelName, "$");
    // strcat(modelName, codebookName);
    // strcat(modelName, ".");
    strcat(modelName, dataFiles);
    strcat(modelName, ".mm");
    printf("Saving model [%s]\n", modelName);
    MatSave(negModel,modelName);
    MatFree(model);
    MatFree(negModel);
    free(modelName);
  }
  for (i = 0; i < nUniqueFilenames; i++) free(uniqueFilenames[i]); free(uniqueFilenames);
  for (i = 0; i < nfiles; i++) {
    free(fileClassPairs[i]);
  }
  free(fileClassPairs);
}


/**
 * PRE: 
 * POS:
 * ALLOC: 
 *     none.
 */
real scoreFromFile(char * filename, matrix *model, matrix * codebook) {
  matrix * data = MatLoadLimitingDim(filename);
  int * codes = encodematrix(data,codebook);
  int i;
  real result = 0.0;
  int lastS, s;
  lastS = codes[0];  
  for (i = 1; i < data->imax; i++) {
    s = codes[i];
    result += log(Mat(model,lastS,s));
    // printf("%d %d %d prob %8.4f log(prob) %8.4f result %8.4f\n", i, lastS, s, Mat(model,lastS,s), log(Mat(model,lastS,s)), result);
    lastS = s;
  }
  free(codes);
  MatFree(data);
  // printf("%8.4f File [%s]\n", result , filename);
  // exit(1);
  return result;
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
int * ClassifyListOfFiles(char * listOfFilesFilename, 
                          char * listOfModelsFilename, 
                          matrix * codebook,
                          int * nFiles) {
  int nfiles;
  char ** fileClassPairs = ReadLines(listOfFilesFilename, &nfiles);
  // int nModels = charCount(':', models) + 1;
  // char ** smodels = splitAtChar(':', models);
  int nModels;
  char ** smodels = ReadLines(listOfModelsFilename, &nModels);
  int i,j,k;
  matrix ** mmodels = (matrix **) malloc(sizeof(matrix *) * nModels);
  int * result = (int *) calloc(nfiles, sizeof(int));
  for (i = 0; i < nModels; i++) {
    printf("Load model [%s]\n", smodels[i]);
    mmodels[i] = MatLoad(NULL,smodels[i]);
    // MatPrint(mmodels[i]);
  }  
  // exit(1);
  matrix * scores = MatAlloc(nfiles,nModels);
  char * filename;
  printf("%d files to classify.\n", nfiles);
  for (j = 0; j < nfiles; j++) {
    filename = copyUntilTab(fileClassPairs[j]);
    // printf("# %s\t", filename);
    for (i = 0; i < nModels; i++) {
      Mat(scores,j,i) = scoreFromFile(filename, mmodels[i], codebook);
      // printf("%f\t", Mat(scores,j,i));
    }
    free(filename);
  }
  for (i = 0; i < nfiles; i++) {
    MatMaxInRow(scores,i,&k);
    // printf("class: %d ", k);
    result[i] = k;
  }
  for (i = 0; i < nModels; i++) {
    free(smodels[i]);
    MatFree(mmodels[i]);
  }
  free(smodels);
  free(mmodels);
  for (i = 0; i < nfiles; i++) {
    free(fileClassPairs[i]);
  }
  free(fileClassPairs);
  MatFree(scores);
  *nFiles = nfiles;
  return result;
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 *     Creates and returns a new string.
 */
char * getNegativeModelName(char * positiveModelName) {
  // FIXME: does not work with windows:
  int pos = charLastPosition('/', positiveModelName);
  char * negName = (char *) malloc(sizeof(char) * (strlen(positiveModelName) + 5));
  strncpy(negName,positiveModelName,pos+1);
  negName[pos+1] = 0;
  // printf("nn1 [%s]\n", negName);
  strcat(negName,"NOT_");
  // printf("nn2 [%s]\n", negName);
  strcat(negName, positiveModelName + pos + 1);
  // printf("negName: [%s]\n", negName);
  return negName;
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
matrix * ClassifyListOfFilesForTagging(char * listOfFilesFilename, 
                                       char * listOfModelsFilename, 
                                       matrix * codebook,
                                       char *** uniqueFilenames,
                                       int * nFiles
                                       ) {
  int nfiles;
  char ** fileClassPairs = ReadLines(listOfFilesFilename, &nfiles);
  // int nModels = charCount(':', models) + 1;
  // char ** smodels = splitAtChar(':', models);
  int nModels;
  char ** smodels = ReadLines(listOfModelsFilename, &nModels);
  int i,j,k;
  matrix ** mPmodels = (matrix **) malloc(sizeof(matrix *) * nModels);
  matrix ** mNmodels = (matrix **) malloc(sizeof(matrix *) * nModels);
  // matrix ** result = MatAlloc(nfiles, nModels);
  char * negModelName;
  SortStrings(smodels, nModels);
  // Load models in memory
  for (i = 0; i < nModels; i++) {
    printf("Load model [%s]\n", smodels[i]);
    mPmodels[i] = MatLoad(NULL,smodels[i]);
    negModelName = getNegativeModelName(smodels[i]);
    printf("Load model [%s]\n", negModelName);
    mNmodels[i] = MatLoad(NULL,negModelName);
    free(negModelName);
    // MatPrint(mmodels[i]);
  }  
  // exit(1);
  char * filename;
  real scoreP, scoreN;
  char ** filesToClassify = (char **) calloc(nfiles,sizeof(char *));
  // let's remove duplicates from list of files. If the list of file
  // is a sequence of pairs (file,tag) a file name can appear several
  // times if it has more than one tag.
  k = 0;
  for (j = 0; j < nfiles; j++) {
    // printf("p [%s]\n", fileClassPairs[j]);
    filename = copyUntilTab(fileClassPairs[j]);
    // printf("[%s] k %d \n", filename, k);
    k = addIfNew(filesToClassify,k,filename);
    free(filename);
  }
  for (i = 0; i < nfiles; i++) {
    free(fileClassPairs[i]);
  }
  // printf("bbb\n");
  free(fileClassPairs);
  nfiles = k;
  printf("%d files to classify\n", k); // exit(1);
  matrix * scores = MatAlloc(nfiles,nModels);
  for (j = 0; j < nfiles; j++) {
    // filename = copyUntilTab(fileClassPairs[j]);
    // printf("# %s\t", filename);
    printf("[%d/%d]\r",j+1,nfiles); fflush(stdout);
    for (i = 0; i < nModels; i++) {
      scoreP = scoreFromFile(filesToClassify[j], mPmodels[i], codebook);
      scoreN = scoreFromFile(filesToClassify[j], mNmodels[i], codebook);
      Mat(scores,j,i) = scoreP - scoreN;
      // printf("%d %d %f\n", j, i, Mat(scores,j,i));
    }
    // MatMaxInRow(scores,j,&k);
    // printf("%d\n", k);    
    // free(filesToClassify[j]);
  }
  printf("\n");
  // free(filesToClassify);
  for (i = 0; i < nModels; i++) {
    free(smodels[i]);
    MatFree(mPmodels[i]);
    MatFree(mNmodels[i]);
  }
  free(smodels);
  free(mPmodels);
  free(mNmodels);
  // MatFree(scores);
  *nFiles = nfiles;
  *uniqueFilenames = filesToClassify;
  // for (i = 0; i < nfiles; i++) printf("%d[%s]\n", i, (*uniqueFilenames)[i]);
  // printf("ccc %x \n", *uniqueFilenames);
  return scores;
}


/**
 * PRE: 
 * POS:
 * ALLOC:
 */
void PrecisionRecallFscoreForOneClassBefore(char ** classes, 
                                      int nClass, 
                                      int ** classification, 
                                      int ** groundTruth, 
                                      int size, 
                                      real * precision, 
                                      real * recall, 
                                      real * fscore) {
  int tp = 0, tn = 0, fp = 0, fn = 0;
  real p, r, fs;
  int i,j = nClass;
  printf("# --------\n");
  printf("# Tag: %s\n", classes[nClass]);
  // printf("\nConfusion matrix:\n");
  // MatPrint(confusion);
  for (i = 0; i < size; i++) {
    if ((classification[i][j] == 1) && (groundTruth[i][j] == 1)) tp++;
    if ((classification[i][j] == 1) && (groundTruth[i][j] == 0)) fp++;
    if ((classification[i][j] == 0) && (groundTruth[i][j] == 1)) fn++;
    if ((classification[i][j] == 0) && (groundTruth[i][j] == 0)) tn++;
  }
  if (tp + fn == 0) {
    printf("# There are no positive examples in gt (TP+FN==0).\nTP: %d, TN: %d, FP: %d, FN: %d\n",
           tp, tn, fp, fn);
    *precision = -1.0; *recall = -1.0; *fscore = -1.0;
  } else {
    if (tp + fp == 0) {
      printf("# No samples were classified positively, cannot compute precision.\nTP: %d, TN: %d, FP: %d, FN: %d\n",
           tp, tn, fp, fn);             
      *precision = -1.0; *recall = -1.0; *fscore = -1.0;
    } else {
      printf("# TP: %d, TN: %d, FP: %d, FN: %d\n", tp, tn, fp, fn);
      p = (tp * 1.0) / (tp + fp);
      r = (tp * 1.0) / (tp + fn);
      printf("# Precision: %f\n# Recall: %f\n", p, r);
      if (p+r == 0) {
        printf("# Precision + Recall == 0, cannot compute F-Score.\n");
        *precision = p; *recall = r;
      } else {
        fs = (2.0 * p * r) / (p + r);
        printf("# F-Score: %f\n", fs);
        *precision = p; *recall = r; *fscore = fs;
      }
    }
  }  
}

/**
 * PRE: 
 * POS:
 * ALLOC:
 */
void PrecisionRecallFscoreForOneClass(char ** classes, 
                                      int nClass, 
                                      int ** classification, 
                                      int ** groundTruth, 
                                      int size, 
                                      real * precision, 
                                      real * recall, 
                                      real * fscore,
                                      FILE * outputFile,
                                      FILE * outputCSVFile) {
  int tp = 0, tn = 0, fp = 0, fn = 0;
  real p, r, fs;
  int i,j = nClass;
  fprintf(outputFile, "# --------\n");
  fprintf(outputFile, "# Tag: %s\n", classes[nClass]);
  fprintf(outputCSVFile, "%s,", classes[nClass]);
  // printf("\nConfusion matrix:\n");
  // MatPrint(confusion);
  for (i = 0; i < size; i++) {
    if ((classification[i][j] == 1) && (groundTruth[i][j] == 1)) tp++;
    if ((classification[i][j] == 1) && (groundTruth[i][j] == 0)) fp++;
    if ((classification[i][j] == 0) && (groundTruth[i][j] == 1)) fn++;
    if ((classification[i][j] == 0) && (groundTruth[i][j] == 0)) tn++;
  }
  if (tp + fn == 0) {
    fprintf(outputFile, "# There are no positive examples in gt (TP+FN==0).\nTP: %d, TN: %d, FP: %d, FN: %d\n",
           tp, tn, fp, fn);
    *precision = -1.0; *recall = -1.0; *fscore = -1.0;
  } else {
    if (tp + fp == 0) {
      fprintf(outputFile, "# No samples were classified positively, cannot compute precision.\nTP: %d, TN: %d, FP: %d, FN: %d\n",
           tp, tn, fp, fn);             
      *precision = -1.0; *recall = -1.0; *fscore = -1.0;
    } else {
      fprintf(outputFile, "# TP: %d, TN: %d, FP: %d, FN: %d\n", tp, tn, fp, fn);
      p = (tp * 1.0) / (tp + fp);
      r = (tp * 1.0) / (tp + fn);
      fprintf(outputFile, "# Precision: %f\n# Recall: %f\n", p, r);
      if (p+r == 0) {
        fprintf(outputFile, "# Precision + Recall == 0, cannot compute F-Score.\n");
        *precision = p; *recall = r;
      } else {
        fs = (2.0 * p * r) / (p + r);
        fprintf(outputFile, "# F-Score: %f\n", fs);
        *precision = p; *recall = r; *fscore = fs;
      }
    }
  }
  fprintf(outputCSVFile, "%d,%d,%d,%d,%f,%f,%f\n", tp, tn, fp, fn, *precision, *recall, *fscore);
}


/**
 * PRE: 
 * POS:
 * ALLOC:
 */
void GlobalPrecisionRecallFscore(char ** classes, 
                                 int nClasses, 
                                 int ** classification, 
                                 int ** groundTruth, 
                                 int size, 
                                 real * precision, 
                                 real * recall, 
                                 real * fscore,
                                 FILE * outputFile) {
  int tp = 0, tn = 0, fp = 0, fn = 0;
  real p, r, fs;
  int i,j;
  for (i = 0; i < size; i++) {
     for (j = 0; j < nClasses; j++) {
       if ((classification[i][j] == 1) && (groundTruth[i][j] == 1)) tp++;
       if ((classification[i][j] == 1) && (groundTruth[i][j] == 0)) fp++;
       if ((classification[i][j] == 0) && (groundTruth[i][j] == 1)) fn++;
       if ((classification[i][j] == 0) && (groundTruth[i][j] == 0)) tn++;
     }
  }
  if (tp + fn == 0) {
    fprintf(outputFile, "# There are no positive examples in gt (TP+FN==0).\nTP: %d, TN: %d, FP: %d, FN: %d\n",
           tp, tn, fp, fn);
    *precision = -1.0; *recall = -1.0; *fscore = -1.0;
  } else {
    if (tp + fp == 0) {
      fprintf(outputFile,"# No samples were classified positively, cannot compute precision.\nTP: %d, TN: %d, FP: %d, FN: %d\n",
           tp, tn, fp, fn);             
      *precision = -1.0; *recall = -1.0; *fscore = -1.0;
    } else {
      fprintf(outputFile, "# TP: %d, TN: %d, FP: %d, FN: %d\n", tp, tn, fp, fn);
      p = (tp * 1.0) / (tp + fp);
      r = (tp * 1.0) / (tp + fn);
      fprintf(outputFile,"# Precision: %f\n# Recall: %f\n", p, r);
      if (p+r == 0) {
        fprintf(outputFile,"# Precision + Recall == 0, cannot compute F-Score.\n");
        *precision = p; *recall = r;
      } else {
        fs = (2.0 * p * r) / (p + r);
        fprintf(outputFile,"# F-Score: %f\n", fs);
        *precision = p; *recall = r; *fscore = fs;
      }
    }
  }
  fprintf(outputFile,"# --------\n");
  fprintf(outputFile,"# Tags:\n");
  for (i = 0; i < nClasses; i++) fprintf(outputFile,"%d. %s\n", i, classes[i]);
  fprintf(outputFile,"\n");
}


FILE * OpenOutputFileForResults(char * baseFilename, char * suffix) {
  char * filename;
  // fprintf(stderr, "OpenOutputFileForResults. baseFilename:%s suffix: %s outputDir: %s \n",
  //         baseFilename, suffix, outputDir);
  filename = (char *) malloc(sizeof(char) * ((outputDir == 0 ? 0 : strlen(outputDir)) +
                                             strlen(baseFilename) + 
                                             strlen(suffix) + 1));
  if (outputDir == 0) strcpy(filename, baseFilename);
  else {
    strcpy(filename, outputDir);
    strcat(filename, baseFilename);
  }
  strcat(filename, suffix);
  // fprintf(stderr, "filename: %s\n", filename);
  FILE * file = FileOpen(filename, "w");
  free(filename);
  return file;
}


/**
 * PRE: 
 * POS:
 * ALLOC:
 */
void PresentResultsEvaluation(char * listOfFilesFilename, 
                              char * listOfModelsFilename, 
                              int * results,
                              char * codebookFilename) {
  int nfiles;
  char ** fileClassPairs = ReadLines(listOfFilesFilename, &nfiles);
  int i,j;
  char * filename, * class;
  // int nModels = charCount(':', models) + 1;
  // smodels correspond to files that contain transition probability matrix (one per file). 
  // there is one model per class do we use this filename as the class name.
  // char ** smodels = splitAtChar(':', models);
  int nModels;
  char ** smodels = ReadLines(listOfModelsFilename, &nModels);  
  char ** classes = (char **) malloc(sizeof(char *) * nModels);
  char * shortname, *cfs;
  char *gts;
  matrix * confusion;
  int iGt, iFound;
  int ** groundTruth;
  int ** classification;
  real p,r,fs;
  real totalP = 0.0, totalR = 0.0, totalFS = 0.0;
  char * tmp1, * tmp3;
  char ** tmp2;
  int n,k;
  real * allP;
  real * allR;
  real * allFS;
  char * outputBaseFilename;

  SortStrings(smodels, nModels);
  tmp1 = FilenameName(listOfFilesFilename);
  i = findStringPosition(".cbk", tmp1);
  if (i != -1) tmp1[i] = 0;
  tmp3 = FilenameName(codebookFilename);
  outputBaseFilename = (char *) malloc(sizeof(char) * (strlen(tmp1) + strlen(tmp3) + 4 + 1));
  strcpy(outputBaseFilename,tmp1);
  free(tmp1);
  strcat(outputBaseFilename,".cbk");
  strcat(outputBaseFilename,tmp3);
  free(tmp3);
  FILE * perClassDetailsFile = 
    OpenOutputFileForResults(outputBaseFilename, ".perClass.txt");
  FILE * perClassCSVFile = 
    OpenOutputFileForResults(outputBaseFilename, ".perClass.csv");
  FILE * perItemDetailsFile = 
    OpenOutputFileForResults(outputBaseFilename, ".perItem.txt");
  FILE * resultsSummaryFile = 
    OpenOutputFileForResults(outputBaseFilename, ".summary.txt");
  FILE * resultsSummary1File = 
    OpenOutputFileForResults(outputBaseFilename, ".summary1.csv");
  //
  groundTruth = (int **) malloc(sizeof(int*) * nfiles);
  classification = (int **) malloc(sizeof(int*) * nfiles);
  for (i = 0; i < nfiles; i++) {
    groundTruth[i] = (int *) calloc(nModels,sizeof(int));
    classification[i] = (int *) calloc(nModels,sizeof(int));
  }
  // printf("models [%s] nModels %d\n", models, nModels);
  for (i = 0; i < nModels; i++) {
    // printf("sm[%d] = [%s]\n", i, smodels[i]);
    tmp1 = FilenameName(smodels[i]);
    n = charCount('$',tmp1)+1;
    if (n == 0) {
      Error("PresentResultsEvaluationForTagging", 
            "The models filenames should contain the class/tag. The syntax is: <tag-name>$<rest-of-filename>. The filename used lacks the $ separating the class/tag name and the rest");
    }
    tmp2 = splitAtChar('$',tmp1);
    classes[i] = strdup(tmp2[0]);
    free(tmp1);
    for (k = 0; k < n; k++) free(tmp2[k]);
    free(tmp2);
  }
  // exit(0);
  confusion = MatAlloc(nModels, nModels);
  printf("=================================\n");
  printf("This is a classification problem.\n");
  printf("=================================\n");
  printf("%d files to classify\n", nfiles);
  for (j = 0; j < nfiles; j++) {
    // printf("%d loop start\nfcp: [%s]\n", j, fileClassPairs[j]);
    // a file to classify:
    filename = copyUntilTab(fileClassPairs[j]);
    // printf("filename: %s\n", filename);
    // the ground truth assiciated to this file:
    gts = copyAfterTab(fileClassPairs[j]);
    // printf("gt: %s result %d sm [%s]\n", groundTruth, results[j], smodels[results[j]]);
    // The classification obtained:
    cfs = classes[results[j]];
    // printf("cf: %s\n", classFound);
    // Simplified name of the classified file
    shortname = RemoveDirectory(filename);
    // printf("sn: %s\n", shortname);
    fprintf(perItemDetailsFile,"%s\t%s\t%s\n", shortname, cfs, gts);
    // The number of GT class:
    iGt = findPosition(gts, classes, nModels);
    // The number of class found:
    iFound = results[j];
    // Update confusion matrix:
    Mat(confusion, iGt, iFound) = Mat(confusion, iGt, iFound) + 1;
    // Update the GT and classification matrices
    groundTruth[j][iGt] = 1;
    classification[j][iFound] = 1;
    // printf("free classFound\n");
    // free(classFound); 
    // printf("free filename\n");
    free(filename); 
    // printf("free sn\n");
    free(shortname);
    // printf("free gt\n");
    free(gts);
  }
  // GlobalPrecisionRecallFscore(classes,nModels,classification,groundTruth,nfiles,&p,&r,&fs);
  fprintf(perClassDetailsFile, "# Per-class performance:\n");
  allP = (real *) malloc(sizeof(real) * nModels);
  allR = (real *) malloc(sizeof(real) * nModels);
  allFS = (real *) malloc(sizeof(real) * nModels);
  for (i = 0; i < nModels; i++) {
    PrecisionRecallFscoreForOneClass(classes,i,classification,groundTruth,nfiles,&p,&r,&fs,perClassDetailsFile,perClassCSVFile);
    totalP += p; totalR += r; totalFS += fs;
    allP[i] = p;
    allR[i] = r;
    allFS[i] = fs;
  }
  // fprintf(resultsSummaryFile,"# --------\n");
  // The following lines print a final summary of results on one
  // line. This may be usefull when running a lot of tests. All other
  // output may be ignored by piping through grep "##".  
  tmp1 = RemoveDirectory(codebookFilename);
  tmp3 = RemoveDirectory(listoffilesfile);
  i = findStringPosition(".cbk", tmp3);
  if (i != -1) tmp3[i] = 0;
  fprintf(resultsSummaryFile,"Codebook: %s\nData set: %s\n", tmp1, tmp3);
  fprintf(resultsSummary1File,"%s,%s,", tmp1, tmp3);
  // for (i = 0; i < nModels; i++) {
  //   printf("P(%s)\tR(%s)\tFS(%s)\t", classes[i], classes[i], classes[i]);
  // }
  // printf("average pertag P\taverage pertag R\taverage pertag FS\t\n");
  // printf("##\t%s\t%s\t", tmp1, tmp3);
  // for (i = 0; i < nModels; i++) {
  //   printf("%f\t%f\t%f\t", allP[i], allR[i], allFS[i]);
  // }
  // printf("%f\t%f\t%f\n",totalP/nModels, totalR/nModels, totalFS/nModels);
  equalizeLengths(classes, nModels);
  fprintf(resultsSummaryFile,"# Confusion matrix:\n");
  fprintf(resultsSummaryFile,"#\t");
  for (j = 0; j < nModels; j++) { fprintf(resultsSummaryFile,"%s\t", classes[j]); }
  fprintf(resultsSummaryFile,"P\tR\tFS\t");
  fprintf(resultsSummaryFile,"\n");
  int ok = 0; 
  int ko = 0;
  char sep = '\t';
  for (i = 0; i < nModels; i++) {
    fprintf(resultsSummaryFile,"# %s\t", classes[i]);
    for (j = 0; j < nModels; j++) {
      fprintf(resultsSummaryFile,"%d\t", (int) round(Mat(confusion,i,j)));
      if (i == j) ok += (int) round(Mat(confusion,i,j));
      else ko += (int) round(Mat(confusion,i,j));
    }
    fprintf(resultsSummaryFile,"%5.3f\t%5.3f\t%5.3f\t", allP[i], allR[i], allFS[i]);
    fprintf(resultsSummaryFile,"\n");
  }
  fprintf(resultsSummaryFile,"# --------\n");
  fprintf(resultsSummaryFile,"# Average per-class performance:\n");
  fprintf(resultsSummaryFile,"# Precision: %f\n# Recall: %f\n# F-Score: %f\n", totalP/nModels, 
          totalR/nModels, totalFS/nModels);
  fprintf(resultsSummary1File,"%f,%f,%f,", totalP/nModels, 
          totalR/nModels, totalFS/nModels);
  fprintf(resultsSummaryFile,"# --------\n");
  fprintf(resultsSummaryFile,"# Global Accuracy: %f\n", (1.0*ok)/(ok+ko));
  fprintf(resultsSummary1File,"%f\n", (1.0*ok)/(ok+ko));
  free(tmp1); free(tmp3); free(allP); free(allR); free(allFS);
  FileClose(perClassDetailsFile);
  FileClose(perClassCSVFile);
  FileClose(perItemDetailsFile);
  FileClose(resultsSummaryFile);
  FileClose(resultsSummary1File);
}


/**
 * PRE: 
 * POS:
 * ALLOC:
 */
void PresentResultsEvaluationForTagging(char * listOfFilesFilename,
                                        char ** evalFiles, int nEvalFiles, 
                                        char * listOfModelsFilename, 
                                        matrix * results,
                                        char * codebookFilename) {
  int nfiles;
  char ** fileClassPairs = ReadLines(listOfFilesFilename, &nfiles);
  int i,j;
  char * filename, * class;
  int nModels;
  char ** smodels = ReadLines(listOfModelsFilename, &nModels);  
  // int nModels = charCount(':', models) + 1;
  // smodels correspond to files that contain transition probability matrix (one per file). 
  // there is one model per class do we use this filename as the class name.
  // char ** smodels = splitAtChar(':', models);
  char ** classes = (char **) malloc(sizeof(char *) * nModels);
  char * shortname;
  char **cfs;
  char ** gts;
  matrix * confusion;
  int iGt, iFound;
  int ** groundTruth;
  int ** classification;
  real p,r,fs;
  real totalP = 0.0, totalR = 0.0, totalFS = 0.0;
  char * tmp1, * tmp3;
  char ** tmp2;
  int n,k;
  real * allP;
  real * allR;
  real * allFS;
  char * file;
  char * outputBaseFilename;
  tmp1 = FilenameName(listOfFilesFilename);
  i = findStringPosition(".cbk", tmp1);
  if (i != -1) tmp1[i] = 0;
  tmp3 = FilenameName(codebookFilename);
  outputBaseFilename = (char *) malloc(sizeof(char) * (strlen(tmp1) + strlen(tmp3) + 4 + 1));
  strcpy(outputBaseFilename,tmp1);
  free(tmp1);
  strcat(outputBaseFilename,".cbk");
  strcat(outputBaseFilename,tmp3);
  free(tmp3);
  FILE * perItemGTDetailsFile = 
    OpenOutputFileForResults(outputBaseFilename, ".perItemGT.txt");
  FILE * perTagDetailsFile = 
    OpenOutputFileForResults(outputBaseFilename, ".perTag.txt");
  FILE * perTagCSVFile = 
    OpenOutputFileForResults(outputBaseFilename, ".perTag.csv");
  FILE * perItemCLDetailsFile = 
    OpenOutputFileForResults(outputBaseFilename, ".perItemCL.txt");
  FILE * resultsSummaryFile = 
    OpenOutputFileForResults(outputBaseFilename, ".summary.txt");
  FILE * perItemBitCLDetailsFile = 
    OpenOutputFileForResults(outputBaseFilename, ".perItem01CL.csv");
  FILE * perItemBitGTDetailsFile = 
    OpenOutputFileForResults(outputBaseFilename, ".perItem01GT.csv");
  FILE * resultsSummary1File = 
    OpenOutputFileForResults(outputBaseFilename, ".summary1.csv");
  printf("PresentResultsEvaluationForTagging nEvalFiles %d nfiles %d\n", nEvalFiles, nfiles);
  printf("=================================\n");
  printf("This is an autotagging problem.\n");
  printf("=================================\n");
  groundTruth = (int **) malloc(sizeof(int*) * nEvalFiles);
  classification = (int **) malloc(sizeof(int*) * nEvalFiles);
  for (i = 0; i < nEvalFiles; i++) {
    groundTruth[i] = (int *) calloc(nModels,sizeof(int));
    classification[i] = (int *) calloc(nModels,sizeof(int));
  }
  for (i = 0; i < nModels; i++) {
    printf("sm[%d] = [%s]\n", i, smodels[i]);
  }
  SortStrings(smodels, nModels);
  printf("== Sorted\n");
  for (i = 0; i < nModels; i++) {
    printf("sm[%d] = [%s]\n", i, smodels[i]);
  }
  // printf("2PresentResultsEvaluationForTagging nModels %d\n", nModels);
  // printf("models [%s] nModels %d\n", models, nModels);
  // put tag names in classes[].
  for (i = 0; i < nModels; i++) {
    // printf("sm[%d] = [%s]\n", i, smodels[i]);
    tmp1 = FilenameName(smodels[i]);
    n = charCount('$',tmp1)+1; // number of substrings in model name
    if (n == 0) {
      Error("PresentResultsEvaluationForTagging", 
            "The models filenames should contain the class/tag. The syntax is: <tag-name>$<rest-of-filename>. The filename used lacks the $ separating the class/tag name and the rest");
    }
    tmp2 = splitAtChar('$',tmp1); // split substrings
    classes[i] = strdup(tmp2[0]); // the class correspond to the first substring
    free(tmp1);
    for (k = 0; k < n; k++) free(tmp2[k]);
    free(tmp2);
  }
  for (i = 0; i < nModels; i++) {
    printf("%d/%d [%s]\n", i, nModels, classes[i]);
  }  
  // exit(0);
  // confusion = MatAlloc(nModels, nModels);
  // printf("3PresentResultsEvaluationForTagging \n");
  for (j = 0; j < nEvalFiles; j++) {
    // printf("%d loop start\nfcp: [%s]\n", j, fileClassPairs[j]);
    // filename = copyUntilTab(fileClassPairs[j]);
    filename = evalFiles[j];
    // printf("filename = [%s]\n", filename);
    shortname = RemoveDirectory(filename);
    // printf("shortname = [%s]\n", shortname);
    // printf("filename: %s\n", filename);
    // gts = copyAfterTab(fileClassPairs[j]);
    /* ------ Ground truth: */
    fprintf(perItemGTDetailsFile,"GT: %s\t", shortname);
    for (i = 0; i < nfiles; i++) {
      file = copyUntilTab(fileClassPairs[i]);
      class = copyAfterTab(fileClassPairs[i]);
      if (strcmp(file, filename) == 0) {
        fprintf(perItemGTDetailsFile,"%s\t", class);
        iGt = findPosition(class, classes, nModels);
        k = findPosition(filename, evalFiles, nEvalFiles);
        if (iGt != -1) { 
          // printf("class: [%s]\n", class); Error("PresentResultsEvaluationForTagging","class in not member of classes"); 
          groundTruth[k][iGt] = 1;
        }
      }
      // printf("free class [%s]\n", class);
      free(class); // printf("%d/%d free file [%s]\n", i, nfiles, file); 
      free(file);
      // printf("after free file\n");
    }
    fprintf(perItemGTDetailsFile,"\n");
    /* ------ Models output */
    fprintf(perItemCLDetailsFile,"CL: %s\t", shortname);
    //for (i = 0; i < nEvalFiles; i++) {
    for (i = 0; i < nModels; i++) { 
      if (Mat(results,j,i) > 0) { 
        classification[j][i] = 1;
        fprintf(perItemCLDetailsFile,"%s\t", classes[i]);
      } else {
        classification[j][i] = 0;
      }
      fprintf(perItemBitCLDetailsFile, "%d,", classification[j][i]);
      fprintf(perItemBitGTDetailsFile, "%d,", groundTruth[j][i]);
    }
    fprintf(perItemCLDetailsFile,"\n");
    fprintf(perItemBitCLDetailsFile,"\n");
    fprintf(perItemBitGTDetailsFile,"\n");
    // printf("free shortname [%s]\n", shortname);
    free(shortname);
  }
  tmp1 = RemoveDirectory(codebookFilename);
  tmp3 = RemoveDirectory(listoffilesfile);
  fprintf(resultsSummaryFile,"Codebook: %s\nData set: %s\n", tmp1, tmp3);
  fprintf(resultsSummary1File, "%s,%s,",tmp1, tmp3);
  // fprintf(resultsSummaryFile,"# Per-class performance:\n");
  allP = (real *) malloc(sizeof(real) * nModels);
  allR = (real *) malloc(sizeof(real) * nModels);
  allFS = (real *) malloc(sizeof(real) * nModels);
  for (i = 0; i < nModels; i++) {
    PrecisionRecallFscoreForOneClass(classes,i,classification,groundTruth,nEvalFiles,
                                     &p,&r,&fs,perTagDetailsFile,perTagCSVFile);
    totalP += p; totalR += r; totalFS += fs;
    allP[i] = p;
    allR[i] = r;
    allFS[i] = fs;
  }
  fprintf(resultsSummaryFile,"# --------\n");
  fprintf(resultsSummaryFile,"# Average per-class performance:\n");
  fprintf(resultsSummaryFile,"# Precision: %f\n# Recall %f\n# F-Score: %f\n", totalP/nModels, 
         totalR/nModels, totalFS/nModels);
  fprintf(resultsSummary1File,"%f,%f,%f\n", totalP/nModels, 
          totalR/nModels, totalFS/nModels);
  fprintf(resultsSummaryFile,"# --------\n");
  fprintf(resultsSummaryFile,"# Global performance:\n");
  GlobalPrecisionRecallFscore(classes,nModels,classification,groundTruth,nEvalFiles,
                              &p,&r,&fs,resultsSummaryFile);
  free(tmp1); 
  free(tmp3);
  FileClose(perTagDetailsFile);
  FileClose(perTagCSVFile);
  FileClose(perItemGTDetailsFile);
  FileClose(perItemCLDetailsFile);
  FileClose(resultsSummaryFile);
  FileClose(resultsSummary1File);
  FileClose(perItemBitCLDetailsFile);
  FileClose(perItemBitGTDetailsFile);
}


int main (int argc, char **argv) {
  matrix * codebook;
  int * classification;
  matrix * tagification;
  int i,n,k;
  char **files;

  CommandLine(argc, argv);
  if (makecodebook) {
    if (dataFilename != NULL) {
      MakeCodebookFromFile(dataFilename, codebookSize, epsilon, codebookFilename);
    } else if (listoffilesfile != NULL) {
      // printf("listoffilesfile: [%s]\n", listoffilesfile);
      // TODO: 50 -> command line parameter
      MakeCodebookFromListOfFiles(listoffilesfile, 50, codebookSize, epsilon, codebookFilename);
    }
    // 
  } else if (encode) {
    codebook = MatLoad(NULL, codebookFilename);
    // printf("codebookFilename %s\n", codebookFilename ); exit(1);
    // MatPrint(codebook);
    encodeStream(input,codebook,output);
  } else if (makeclassmodels) {
    codebook = MatLoad(NULL, codebookFilename);
    MakeModels(listoffilesfile,codebook);
  } else if (maketagmodels) {
    codebook = MatLoad(NULL, codebookFilename);
    MakeModelsForTagging(listoffilesfile,codebook);
  } else if (classify) {
    codebook = MatLoad(NULL, codebookFilename);
    classification = ClassifyListOfFiles(listoffilesfile,models,codebook, &n);
    PresentResultsEvaluation(listoffilesfile, models, classification, codebookFilename);
    // printf("end2\n");
  } else if (tagify) {
    codebook = MatLoad(NULL, codebookFilename);
    tagification = ClassifyListOfFilesForTagging(listoffilesfile,models,codebook, &files, &n);
    // printf("n %d add %x %x\n",n, files, *files);
    // for (i = 0; i < n; i++) printf("++ %d[%s]\n", i, (files)[i]);
    PresentResultsEvaluationForTagging(listoffilesfile, files, n, models, tagification, codebookFilename);
    // printf("end2\n");
  }
  return(0);  
}
