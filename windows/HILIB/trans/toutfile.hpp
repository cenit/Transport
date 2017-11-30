///////////////////////////////////////////////////////////////////////////////
////////////////////////// class TitleOfRun definition ////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef TROUTFILE_HPP
#define TROUTFILE_HPP

class TitleOfRun : public Object
{
    char caption[100];

  public:

    TitleOfRun(char *);
    ~TitleOfRun(void);
    char *GetTitleOfRun(void);
};

///////////////////////////////////////////////////////////////////////////////
////////////////////////// class TrOutFile definition /////////////////////////
///////////////////////////////////////////////////////////////////////////////

class TrOutFile : public TitleOfRun
{
    FILE *fp;
    char *titel[17];
    long rem[1000];
    int  pos;

    char line[200];
    char sigma1[100];
    char sigma2[100];
    char sigma3[100];
    char sigma4[100];
    char sigma5[100];
    char sigma6[100];
    char transform1[100];
    char transform2[100];
    char transform3[100];
    char transform4[100];
    char transform5[100];
    char transform6[100];
    char transform7[100];

  protected:

    void FindPlotSection(void);
    BOOL GetNextLine(void);
    char *SearchElement(float, char *);

  public:

    TrOutFile(char *, char *);
    ~TrOutFile(void);
    char *FindElement(float);
    char *FindElement(char *);
    char *GetNextElement(void);
    char *GetPrevElement(void);
    char *GetSigmaLine(int);
    char *GetMatrixLine(int);
    VOID Rewind(void);
};

#endif

///////////////////////////////// EOF /////////////////////////////////////////
