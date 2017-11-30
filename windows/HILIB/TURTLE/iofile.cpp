///////////////////////////////////////////////////////////////////////////////
//////////////////////////// IOFile class implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"


IOFile :: IOFile(char *FileName, int &ok)
        : Object(101)
{
    int   i;
    LONG  lpar1;
    int   par1,  par2,  par3,  par4,  par5,  par6;
    float fpar1, fpar2, fpar3, fpar4, fpar5, fpar6, fpar7, fpar8, fpar9, fpar10;
    char  char5[5];

    mtables    = FALSE;
    xmoms      = FALSE;
    xmom2s     = FALSE;
    zs         = FALSE;
    istops     = FALSE;
    names      = FALSE;
    noms       = FALSE;
    nom1s      = FALSE;
    hists      = FALSE;
    losss      = FALSE;
    labs       = FALSE;
    io         = NULL;

    if ((fp = fopen(FileName,"r")) != NULL)
    {  ok = 0;
       io = new IOFILE;
       int n = fscanf(fp,"%d %d %d %ld",&par1,&par2,&par3,&lpar1);
       if (n == 4)
       {  io->nhists = par1;
          io->limno  = par2;
          io->loss   = par3;
          io->norays = lpar1;
       }
       else
       {  fclose(fp);
          ok = -1;
          return;
       }      

       io->mtable = new float[io->limno];
       mtables = TRUE;
       int nb = io->limno / 10;       // number of blocks
       int nr = io->limno - 10 * nb;  // remainder
       for (i=0; i<nb; i++)
       {  if (fscanf(fp,"%f %f %f %f %f %f %f %f %f %f",
                     &fpar1,&fpar2,&fpar3,&fpar4,&fpar5,
                     &fpar6,&fpar7,&fpar8,&fpar9,&fpar10) == 10)
          {  int j = 10 * i;
             io->mtable[j]   = fpar1;
             io->mtable[j+1] = fpar2;
             io->mtable[j+2] = fpar3;
             io->mtable[j+3] = fpar4;
             io->mtable[j+4] = fpar5;
             io->mtable[j+5] = fpar6;
             io->mtable[j+6] = fpar7;
             io->mtable[j+7] = fpar8;
             io->mtable[j+8] = fpar9;
             io->mtable[j+9] = fpar10;
          }
          else
          {  fclose(fp);
             ok = -1;
             return;
          }
       }
       if (nr > 0)  // remainder
       {  for (i=0; i<nr; i++)
          {  if (fscanf(fp,"%f",&fpar1) == 1)
                io->mtable[10*nb+i] = fpar1;
             else
             {  fclose(fp);
                ok = -1;
                return;
             }
          }
       }

       io->xmom = new float[io->nhists];
       xmoms = TRUE;
       for (i=0; i<io->nhists; i++)
       {  if (fscanf(fp,"%f",&fpar1) == 1)
              io->xmom[i] = fpar1;
          else
          {  fclose(fp);
             ok = -1;
             return;
          }
       }

       io->xmom2 = new float[io->nhists];
       xmom2s = TRUE;
       for (i=0; i<io->nhists; i++)
       {  if (fscanf(fp,"%f",&fpar1) == 1)
              io->xmom2[i] = fpar1;
          else
          {  fclose(fp);
             ok = -1;
             return;
          }
       }

       io->z = new float[io->nhists];
       zs = TRUE;
       for (i=0; i<io->nhists; i++)
       {  if (fscanf(fp,"%f",&fpar1) == 1)
              io->z[i] = fpar1;
          else
          {  fclose(fp);
             ok = -1;
             return;
          }
       }

       io->lab = (char **) new long[io->nhists];
       for (i=0; i<io->nhists; i++)
       {  if (fscanf(fp,"%s",&char5[0]) == 1)
          {  io->lab[i] = new char[5];
             strcpy(io->lab[i],char5);
          }
          else
          {  fclose(fp);
             ok = -1;
             return;
          }
          labs = TRUE;
       }

       io->istop = new float[29994];  // 29994 = 3 * 9998
       istops = TRUE;
       for (i=0; i<2999; i++)
       {  if (fscanf(fp,"%f %f %f %f %f %f %f %f %f %f",
                     &fpar1,&fpar2,&fpar3,&fpar4,&fpar5,
                     &fpar6,&fpar7,&fpar8,&fpar9,&fpar10) == 10)
          {  int j = 10 * i;
             io->istop[j]   = fpar1;
             io->istop[j+1] = fpar2;
             io->istop[j+2] = fpar3;
             io->istop[j+3] = fpar4;
             io->istop[j+4] = fpar5;
             io->istop[j+5] = fpar6;
             io->istop[j+6] = fpar7;
             io->istop[j+7] = fpar8;
             io->istop[j+8] = fpar9;
             io->istop[j+9] = fpar10;
          }
          else
          {  fclose(fp);
             ok = -1;
             return;
          }
       }
       if (fscanf(fp,"%f %f %f %f",&fpar1,&fpar2,&fpar3,&fpar4) == 4)
       {
	     int j = 29990;
             io->istop[j]   = fpar1;
             io->istop[j+1] = fpar2;
             io->istop[j+2] = fpar3;
             io->istop[j+3] = fpar4;
       }
       else
       {  fclose(fp);
          ok = -1;
          return;
       }

       for (i=0; i<12; i++)
       {  if (fscanf(fp,"%f",&fpar1) == 1)
              io->unit[i] = fpar1;
          else
          {  fclose(fp);
             ok = -1;
             return;
          }
       }

       for (i=0; i<12; i++)
       {  if (fscanf(fp,"%s",&char5[0]) == 1)
          {  io->name[i] = new char[5];
             strcpy(io->name[i],char5);
          }
          else
          {  fclose(fp);
             ok = -1;
             return;
          }
          names = TRUE;
       }

       for (i=0; i<12; i++)
       {  if (fscanf(fp,"%s",&char5[0]) == 1)
          {  io->nom[i] = new char[5];
             strcpy(io->nom[i],char5);
          }
          else
          {  fclose(fp);
             ok = -1;
             return;
          }
          noms = TRUE;
       }

       for (i=0; i<4; i++)
       {  if (fscanf(fp,"%s",&char5[0]) == 1)
          {  io->nom1[i] = new char[5];
             strcpy(io->nom1[i],char5);
          }
          else
          {  fclose(fp);
             ok = -1;
             return;
          }
          nom1s = TRUE;
       }

       io->mtype  = new int[io->nhists+1];   // +1 found with Bounds Checker
       io->nentry = new int[io->nhists+1];
       io->ncoor  = new int[io->nhists+1];
       io->intvls = new int[io->nhists+1];
       io->begin  = new float[io->nhists+1];
       io->end    = new float[io->nhists+1];
       io->step   = new float[io->nhists+1];
       io->zhist  = new float[io->nhists+1];
       hists = TRUE;
       for (i=0; i<io->nhists; i++)
       {  if (fscanf(fp,"%d %d %d %d %f %f %f %f",&par1,&par2,&par3,&par4,
                     &fpar1,&fpar2,&fpar3,&fpar4) == 8)
          {  io->mtype[i]  = par1;
             io->nentry[i] = par2;
             io->ncoor[i]  = par3;
             io->intvls[i] = par4;
             io->begin[i]  = fpar1;
             io->end[i]    = fpar2;
             io->step[i]   = fpar3;
             io->zhist[i]  = fpar4;
          }
          else
          {  fclose(fp);
             ok = -1;
             return;
          }
       }     

       io->nblk  = new int[io->loss];
       io->kflg  = new int[io->loss];
       io->xstrt = new float[io->loss];
       io->xend  = new float[io->loss];
       io->lthx  = new int[io->loss];
       io->xbin  = new float[io->loss];
       io->nunx  = new int[io->loss];
       io->ystrt = new float[io->loss];
       io->yend  = new float[io->loss];
       io->lthy  = new int[io->loss];
       io->ybin  = new float[io->loss];
       io->nuny  = new int[io->loss];
       losss = TRUE;
       for (i=0; i<io->loss; i++)
       {  if (fscanf(fp,"%d %d %f %f %d %f %d %f %f %d %f %d",
                     &par1,&par2,&fpar1,&fpar2,&par3,&fpar3,&par4,
                                 &fpar4,&fpar5,&par5,&fpar6,&par6) == 12)
          {  io->nblk[i]  = par1;
             io->kflg[i]  = par2;
             io->xstrt[i] = fpar1;
             io->xend[i]  = fpar2;
             io->lthx[i]  = par3;
             io->xbin[i]  = fpar3;
             io->nunx[i]  = par4;
             io->ystrt[i] = fpar4;
             io->yend[i]  = fpar5;
             io->lthy[i]  = par5;
             io->ybin[i]  = fpar6;
             io->nuny[i]  = par6;
          }
          else
          {  fclose(fp);
             ok = -1;
             return;
          }
       }

       fclose(fp);
    }
    else
       ok = -2;
}

IOFILE *IOFile :: GetDataPointer(VOID)
{
    return io;
}

IOFile :: ~IOFile(void)
{
    int i;

    if (mtables) delete []io->mtable;
    if (xmoms)   delete []io->xmom;
    if (xmom2s)  delete []io->xmom2;
    if (zs)      delete []io->z;
    if (labs)
    {
       for (i=0; i<io->nhists; i++)
         delete []io->lab[i];
       delete []io->lab;
    }
    if (istops)  delete []io->istop;
    if (names)
       for (i=0; i<12; i++)
         delete []io->name[i];
    if (noms)
       for (i=0; i<12; i++)
         delete []io->nom[i];
    if (nom1s)
       for (i=0; i<4; i++)
         delete []io->nom1[i];
    if (hists)
    {  delete []io->mtype;
       delete []io->nentry;
       delete []io->ncoor;
       delete []io->intvls;
       delete []io->begin;
       delete []io->end;
       delete []io->step;
       delete []io->zhist;
    }
    if (losss)
    {  delete []io->nblk;
       delete []io->kflg;
       delete []io->xstrt;
       delete []io->xend;
       delete []io->lthx;
       delete []io->xbin;
       delete []io->nunx;
       delete []io->ystrt;
       delete []io->yend;
       delete []io->lthy;
       delete []io->ybin;
       delete []io->nuny;
    }
    if (io) delete io;
}

////////////////////////////////// EOF ////////////////////////////////////////
