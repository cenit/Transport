///////////////////////////////////////////////////////////////////////////////
//////////////////////// definition of class DisplayFile //////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef DISPFILE_HPP
#define DISPFILE_HPP

typedef struct
{
   float zmin;
   float zmax;
   float xampl;
   float yampl;
   float apfac;
   float spez;
   float spez2;
   float aplab;
   float steer;
   float disp;
   float ydisp;
   float rax;
   float ray;
   float measx;
   float measy;
   float measd;
   float csx;
   float csy;
   float bunl;
} DISPAR;

class DisplayFile : public FileBuff
{
    union { DISPAR dispar; float p[19]; };
  public:
    DisplayFile(char *);
    ~DisplayFile(void);
    DISPAR *GetParameters(void);
    float GetValue(int);
    void SetValue(float, int);
    BOOL WriteFile(char *);
};

#endif

/////////////////////////////////////// EOF ///////////////////////////////////
