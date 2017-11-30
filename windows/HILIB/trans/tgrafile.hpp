///////////////////////////////////////////////////////////////////////////////
//////////////////////// class TransGrafFile definition ///////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef TGRAFILE_HPP
#define TGRAFILE_HPP

class TransGrafFile : public TransFile
{
    float hap;
    float totrot;
    float sn, cs;
    int prevtype, nexttype;
    char CMS[3];

  public:
    TransGrafFile(char *, char *, char *);
    ~TransGrafFile(void);
    float Fac2(char *, char *) ;
    float splint(float x[], float y[], float yp[], int n, float z);
    int GetEnveloppe(int, float, float, int, float x[],
                                 float y[], float p[], float py[], float dl[]);
    int GetCSTrajectories(int, float, float, int, float cx[],
                                      float sx[], float cy[], float sy[]);
    int GetRays(int, float, float, int, float x[], float y[]);
    int GetNextDrift(float &, char *);
    int GetNextSlit(float &, float &, float &, char *);
    int GetNextMagnet(int &, float &, float &, float &, float &, char *, int &);
    int GetNextFit(int &, float &, float &);
};

#endif
//////////////////////////////////// EOF //////////////////////////////////////

