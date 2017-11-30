///////////////////////////////////////////////////////////////////////////////
////////////////////////// IOFile class definition ////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef IOFILE_HPP
#define IOFILE_HPP

typedef struct
{
   int nhists;
   int limno;
   int loss;
   LONG norays;
   float *mtable;
   float *xmom;
   float *xmom2;
   float *z;
   char  **lab;
   float *istop;
   float unit[12];
   char  *name[12];
   char  *nom[12];
   char  *nom1[4];
   int   *mtype;
   int   *nentry;
   int   *ncoor;
   int   *intvls;
   float *begin;
   float *end;
   float *step;
   float *zhist;
   int   *nblk;
   int   *kflg;
   float *xstrt;
   float *xend;
   int   *lthx;
   float *xbin;
   int   *nunx;
   float *ystrt;
   float *yend;
   int   *lthy;
   float *ybin;
   int   *nuny;
} IOFILE;

class IOFile : public Object
{
     FILE *fp;
     IOFILE *io;
     BOOL mtables, xmoms, xmom2s, istops;
     BOOL names, noms, nom1s, zs, labs;
     BOOL hists, losss;
   protected:
   public:
     IOFile(char *, int &);
     ~IOFile(VOID);
     IOFILE *GetDataPointer(VOID);
};

#endif

////////////////////////////////// EOF ////////////////////////////////////////
