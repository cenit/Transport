///////////////////////////////////////////////////////////////////////////////
//////////////////////// Definition of class TransInFile //////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef TRINFILE_HPP
#define TRINFILE_HPP

class TransInFile : public FileBuff
{
    char FileName[50];
    char **line;
    int nlines;
    TransFile *opOFile;
    int index, linesout;
    int (TransInFile::*f[6])(char *);
    float x,xp,r12,y,yp,r34,R16,R26,pp,dp;
    BOOL first, anin, equad;
    CalibrationData *opCalQ, *opCalB;
    int nrQ, nrB;
    float g2, nindex;
    float OldP, NewP, fac, pscal;

  public:
    TransInFile(char *);
    ~TransInFile(VOID);
    VOID GetTransFilePtr(TransFile *);
    int TransferQuads(char *);
    int TransferBeam(char *);
    int TransferLength(char *);
    int TransferBero(char *);
    int ErrorMess(char *);
    int TransferData(int, char *);
    int AnalyseInput(int, int &, float &, float &, float &, char *);
    int InsertField(int, float, long, float, char *, float &);
    int ModifyInput(int, float, float, float, char *);
    int FxyDac(int [], int [], int [], char *, char *);
    int FxyGauss(float [], float [], float [], char *, char *);
    void SetNewP(float);
    float GetOldP(void);
    float GetPScal(void);
    VOID OverWrite(VOID);
};

#endif
///////////////////////////////////// EOF /////////////////////////////////////
