////////////////////////////////////////////////////////////////////////////////
//////////////////// Definition of class CalibrationData ///////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef CALIDATA_HPP
#define CALIDATA_HPP

typedef struct
{
   char  typ[5];
   char  lab[5];
   int   leff;
   int   ap;
   float a0;
   float a1;
   float a2;
   float a3;
   float j0;
   float jmax;
   float dacmax;
} REC6;

class CalibrationData : public FileBuff
{
    REC6 *rec6;
    int  nr;
  public:
    CalibrationData(char *);
    ~CalibrationData(void);
    int  GetNumberOfLines(void);
    int  CompareData(int, char *, float, float);
    VOID GetDac(float, int, long &, float &);
    VOID GetField(long, int, float &);
};

#endif
///////////////////////////////////// EOF //////////////////////////////////////
