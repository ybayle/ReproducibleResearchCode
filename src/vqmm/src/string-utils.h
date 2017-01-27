
int findPosition(char * s, char ** table, int size);

int findStringPosition(char * token, char * s);

char ** splitAtChar(char c, char * s); 

int charFirstPosition(char c, char * s); 

int charLastPosition(char c, char * s); 

int charCount(char c, char *s); 

char * copyBeforeChar(char c, char * s); 

char * copyAfterChar(char c, char * s); 

int addIfNew(char ** stringArray, int size, char * s);

void equalizeLengths(char ** s, int size);

void SortStrings(char ** table, int size);
