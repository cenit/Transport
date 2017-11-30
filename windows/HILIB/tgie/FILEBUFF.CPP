///////////////////////////////////////////////////////////////////////////////
//////////////////////// Implementation of class FileBuff /////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#define MAXB 255

FileBuff :: FileBuff(char *filename)          // Constructor
          : Object(100)
{
    if ((fp = fopen(filename,"r")) != NULL)
    {   Length = FileSize(fp);
        Buff = new char[Length+1];
        long l = fread(Buff, 1, Length, fp);
        *(Buff+l) ='\0';            // terminate with zero
        fclose(fp);
    }
    else
        Buff = NULL;                          // file error

    WorkBuff = Buff;                          
}

FileBuff :: ~FileBuff(void)
{
    if (Buff) delete []Buff; // [] found with Bounds Checker
}

long FileBuff :: FileSize(FILE *fp)

//  Get length of diskfile in Bytes

{
    long curpos, len;

    curpos = ftell(fp);   /* save current position */

    fseek(fp, 0L, 2);     /* put file pointer to EOF */
    len = ftell(fp);      /* = length of file */
    fseek(fp, curpos, 0); /* put back at saved position */
    return(len);
}

int FileBuff :: ReadLn(char **p, char *string)

//  Read line from memory buffer
//  *p is the current pointer in memory, which is updated.

{
    int letter;
    int count = 0;  /* check for overflow */

    while ( ((letter = **p) != '\n') && (letter != '\0') && (count < MAXB) )
    {
	count++;
        *string++ = **p;
        (*p)++;
    }

    *string = '\0';      /* terminate string */
    if (count < MAXB)
       (*p)++;           /* jump over CR/LF */

    if (letter == '\0')  /* signal EOF */
        return(0);
    else
        return(1);
}

int FileBuff :: TellNumberOfLines(void)
{
    if (Buff == NULL)
       return 0;          // file doesn't exist

    char *line = new char[MAXB+1];
    char *p    = Buff;

    int nl = 0;
    while (ReadLn(&p,line) > 0)
    {
       strcpy(line,"");
       nl++;
    }
    delete []line;  // [] found with Bounds Checker
    return(nl);
}

int FileBuff :: GetNextLine(char *line)
{
    int retval = ReadLn(&WorkBuff,line);    
    return (retval);    
}

////////////////////////////////// EOF ////////////////////////////////////////
