///////////////////////////////////////////////////////////////////////////////
//////////////////////// class TransSecondFile definition /////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef TSECFILE_HPP
#define TSECFILE_HPP

typedef struct
{
   char  xdime[11][4];
} REC3;

typedef struct
{
   long  type;
   float along;
   char  label[4];
   float totrot;
   float srmtx[5][21];
} REC4;

typedef struct
{
   char indexline[61];
   char valueline[61];
} INDVAL;

class TransSecondFile : public TitleOfRun
{
    char *dim[11];
    FILE *fp[11];
    int  nr, NumberOfOpenFiles;
    REC4 *line;
    char *info;
    char *title;
    char *caption;
    INDVAL *indval;
    int ncurves, i[4], j[4], k[4], jkind[4];
    char tinf[4][10];
    char *jk[21];
  protected:
    VOID GetDimensions(void);
  public:
    TransSecondFile(char *, char *, char *);
    ~TransSecondFile(void);
    char *FindElement(float);
    char *FindElement(char *);
    char *GetNextElement(void);
    char *SearchElement(float, char *);
    char *GetPrevElement(void);
    int GetNumberOfElements(void);
    int GetNumberOfOpenFiles(void);
    FILE *GetFilePointer(int fn=0);
    VOID Rewind(void);
    INDVAL *GetSortedData(int);
    VOID GetParLine(char *);
    char *GetTijkLabel(int);
    int GetMatrixElements(int, int&, float&, float[], float[],
                                        float[], float[], float[]);
};

#endif

///////////////////////////////// EOF /////////////////////////////////////////
