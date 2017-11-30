///////////////////////////////////////////////////////////////////////////////
//////////////////////// Definition of class TransInFile //////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef TRINFILE_HPP
#define TRINFILE_HPP

class TransInFile : public FileBuff
{
    char FileName[50];
    char **line;
    char **title;
    char **t;
    int nlines;
    BOOL update, angle, equad1, ebend1;
    char dline[133];
    BOOL Turtle, multscat;
    int  index;

  public:
    TransInFile(char *);
    ~TransInFile(VOID);
    char *GetLine(int, int);
    VOID SetLine(char *, int);
    char *GetTitle(int);
    char *GetTitleSpec(int, BOOL);
    char *GetText13(int, BOOL);
    char *GetText15(int, BOOL);
    char *GetText16(int, BOOL);
    VOID SetTitle(char *, int);
    int  GetLabel(char *, char *);
    int  Title2Typecode(char *);
    int  MakeTitle(char *, char *, BOOL, BOOL);
    int  GetNew(void);
    char *GetDefaultData(int);
    BOOL GetAngleInfo(void);
    BOOL GetUpdateInfo(void);
    VOID OverWrite(char *, int);
    int  TellNumberOfLinesEx(void);
    BOOL IsForTurtle(void);
    BOOL IsEquad(int);
    BOOL IsEbend(int);
};

#endif
///////////////////////////////////// EOF /////////////////////////////////////
