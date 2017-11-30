///////////////////////////////////////////////////////////////////////////////
////////////////////////// class TransFile definition /////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef TRANFILE_HPP
#define TRANFILE_HPP

void adjust(FILE *fp);

typedef struct
{
   char  xdime[11][4];
   float amp;
} REC1;

typedef struct
{
   long  type;
   char  label[4];
   float data[4];
   float along;
   float cenun[6];
   float siun[6];
   float r12;
   float r34;
   float r56;
   float r13;
   float r24;
   float rtmx[13];
   float layout[5];
} REC2;

typedef struct
{
   long  type;
   char  label[4];
   float f1, f2, con, delta, val, z;
   float chisqr;
   long  nl;
   char  labs[20][4];
   long  failed;
   long  dummy[6];
   float layout[5];
} REC5;

VOID GetTitleOfElement(int, char *);
BOOL CheckDimensions(char *dim[11]);
VOID PutInfo(UINT);
char *String(UINT wID, UINT n=0);

class TransFile : public TitleOfRun
{
    char  *dim[11];
    FILE  *fp[11];
    int   nr, NumberOfOpenFiles;
    char  title[11];
    float totleng;
    BOOL  first, flag14;
    int   nrdev;
    char  *caption;
    float g2, k1, k2, r1inv, r2inv;
    float x0,y0,xp0,yp0,r12,r34,r16,r26,p0,dpp,epsx,epsy;
    float r56,dl;
    float r13,r24;
    int   linesout, flag, nl;
    float f1, f2, con, delta, val, z;
    char  labs[5], *labv[20];
    char  *info;
    long  prevtype;
    float oldchisqr;
    float xmax,ymax,xpmax,ypmax;
    float cx,sx,dx,cy,sy,dy,cpx,spx,dpx,cpy,spy,dpy;
    char  ilabel[10];
    int   itype;
    float fpar1, fpar2, fpar3;
    int   (TransFile::*f[13])(char *);
    char  NullStr[1], Blank[2], RB[3];
    float totrot;
    float alphax, alphay, betax, betay; // twiss parameters
    BOOL  equad, ebend;
    
  protected:
    VOID GetDimensions(void);
    VOID SaveParams(REC2 *);
    int  GetCalibrationData(char *);
    int  NextQuadData(int, CalibrationData *, char *);
    int  NextBendData(int, CalibrationData *, char *);
  public:
    TransFile(char *, char *, char *);
    ~TransFile(void);
    int InitialBeam(char *buffer);
    int InitialBeam2(char *buffer);
    int NextQuad(char *buffer);
    int NextBend(char *buffer);
    int NextBeamLine(char *buffer);
    int NextFitCondition(char *buffer);
    int NextShift(char *buffer);
    int NextLayout(char *buffer);
    int NextElement(char *buffer);
    int NextQuadDac(char *buffer);
    int NextQuadAmp(char *buffer);
    int NextBendDac(char *buffer);
    int NextBendAmp(char *buffer);
    int ErrorMess(char *buffer);
    int GetNumberOfOpenFiles(void);
    FILE *GetFilePointer(int fn=0);
    char *GetDimension(int);
    char *FindElement(float);
    char *FindElement(char *);
    char *GetNextElement(void);
    char *SearchElement(float, char *);
    char *GetPrevElement(void);
    char *GetBeamInfoData(int);
    char *GetBeamInfoElem(int);
    BOOL GetElement(float, char *, char *, char *, char *, char *, char *);
    VOID GetParameters(int &, float &, float &, float &, char *);
    VOID GetEllipsePar(float &, float &, float &, float &,
                       float &, float &, float &, float &);
    VOID GetEllipsePar2(float &, float &, float &);
    VOID GetEllipsePar3(float &, float &, float &, float &,
                       float &, float &);
    int Show(int, char *);
    int GetNumberOfElements(void);
    VOID Rewind(void);
};

#endif

///////////////////////////////// EOF /////////////////////////////////////////
