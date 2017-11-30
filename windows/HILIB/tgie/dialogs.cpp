
#include "allincs.h"

///////////////////////////////////////////////////////////////////////////////

BOOL MoreLeft(HWND hPar)
{
// get size and position of parent window

   RECT rcp;
   GetWindowRect(hPar,&rcp);
   if (rcp.left + (rcp.right - rcp.left) / 2 > GetScreenWidth() / 2)
      return TRUE;   // more space on the left
   else
      return FALSE;  // more space on the right
}

void GetWindowCoordnates(HWND hPar, int &x, int &y, int &w, int &h)
{
	RECT rcp;
	GetWindowRect(hPar,&rcp);

	h = rcp.bottom - rcp.top; // make child equally high as parent

	if (MoreLeft(hPar))
	{
   	    y = rcp.top + ((rcp.bottom - rcp.top) - h) / 2;
	    if (y <= 0)
	      y = 0;
	    x = rcp.left - w;
	    if (x < 0)
	      x = 0;
	}
	else
	{
   	    y = rcp.top + ((rcp.bottom - rcp.top) - h) / 2;
	    if (y <= 0)
	      y = 0;
	    x = rcp.right;
	    if (x + w > GetScreenWidth())
	      x = GetScreenWidth() - w;
	}
}

int GetNumberOfVaryCodes(int typcod, BOOL Turtle)
{
    static unsigned char nv[] = {6,2,1,3,3, 0,6,6,0,0, 0,15,0,6,0, 2,0,2,2,1, 2,0,1,0,0,
				 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,
				 1,1,1,0,0, 0,0,0,0,0};

    if (Turtle)
	nv[50-1] = 1;

    int typecode = abs(typcod);

    if (typecode < 60)
	return (int)nv[typecode-1]; 
    else
	return 0;
}

int GetNumberOfDataItems(int typcod, BOOL update, BOOL Turtle)
{
    static unsigned char nv[] = {8,1,1,3,3,  4,6,7,1,4, 2,15,1,7,3, 3,1,3,2,1, 2,4,3,3,4,
			         3,3,8,7,11, 0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,4,
				 4,4,0,0,0,  0,0,0,0,4};

    int typecode = abs(typcod);

    if (typecode <= 60)
    {
	int di = (int)nv[typecode-1]; 
	return di;
    }
    else
	return 0;
}

char *GetNameOfParam(int typcod, int nr, BOOL update, BOOL angle, BOOL Turtle, BOOL equad, BOOL ebend, char *titel)
{
    char tit[30];
    static char *nam[] = {"x","x'","y","y'","dl","dp/p","p0"," ",
			  "Angle",
			  "Length",
			  "Leff","B-Field","n-value",
			  "Leff","B-Field","Half-Apperture",
			  "1 or 3","half width x/y","[ 3 ]","[ half width y ]",
			  "x","x'","y","y'","dl","dp/p",
			  "x-Displacem.","x-Axis-Rot.","y-Displacem.","y-Axis-Rot.","z-Displacem.","z-Axis-Rot.","3-Dig. Code",
			  "Code",
			  "Code 1","Code 2","Value","Accuracy",
			  "Length","Voltage",
			  "r21 <-","r31","r32","r41","r42","r43 <-","r51","r52","r53","r54","r61","r62","r63","r64","r65",
			  "Code digit  *",
			  "Ri1","Ri2","Ri2","Ri4","Ri5","Ri6","Line i",
			  "Coord. Code *","Unit Name","Scale Factor",
			  "Code digit  *","Value"," ",
			  "  ",
			  "Leff","Field","Half-Apperture",
			  "Leff","Field",
			  "Angle",
			  "Leff","Magnet. Field",
			  "Length","Current","Bunch Frequ.","Print Frequ.",
			  "Voltage","Phase","Frequency",
			  "Code i","Code j","Register #",
			  "Inp. Register 1","Inp. Register 2","Operation #","Output Reg.",
			  "Leff","Field","Half-Apperture",
			  "Leff","E-Field","B-Field",
			  "Dz","Bz0","Bz1 [r**2]","Bz2 [r**4]","Br1 [r]","Br2","Br3 [r]","Ap-radius",
			  "Dz","Dx-Entr","Dy-Entr","Dx-Exit","Dy-Exit","Dinst","Dslits",
			  "E0","dE/dx(E0)","E1","dE/dx(E1)","E2","dE/dx(E2)","E3","dE/dx(E3)","E4","dE/dx(E4)","id",
			  "Coord. Code","Min. Value","Max. Value"," ",
			  "Coord. Code","Min. Value","Max. Value","Delta Value",
			  "Coord. Code","Min. Value","Max. Value","Delta Value",
			  "m.parent","m.daughter.c","m.daughter.n","par.lifetime"
			  };

    static char *nam2[] = {"Code 1","Code 2"," "," "};
    static char *nam3[] = {"Leff","Angle","n-val."};
    static char *nam4[] = {"Coord. Code","Min. Value","Max. Value","Delta Value"};
    static char *nam5[] = {"3 for 3rd order"};
    static char *nam6[] = {"Coord.Code+id","half width","[ Coord.Code ]","[ half width ]"};
    static char *nam7[] = {"Code digit  *","Value","Value"};
    static char *nam8[] = {"1/e scat.angle","B Moliere distr","Intera.length","Absorb.prob","Nuc.el.scat.ang","Width Land.dis","Aver.Ener.loss","id"};
    static char *nam9[] = {"delta-x","delta-x'","delta-y","delta-y'","delta-dl","delta-dp/p","delta-p0","id"};
    static char *nam10[]= {"Leff","E-Field","Half-Apperture"};
    static char *nam11[]= {"Leff","Angle","parameter"};

    int typecode = abs(typcod);

    strcpy(tit,titel);
    tit[4] = '\0';

    if ((typecode == 6) && update)	// update or slit ?
	return (nam2[nr-1]);
    else if ((typecode == 4) && (nr == 2) && angle)  // angle or field ?
	return (nam3[nr-1]);
    else if ((typecode == 50) && Turtle)
	return (nam4[nr-1]);
    else if ((typecode == 17) && Turtle)
	return (nam5[nr-1]);
    else if ((typecode == 6) && Turtle)
	return (nam6[nr-1]);
    else if ((typecode == 16) && Turtle)
	return (nam7[nr-1]);
    else if (Turtle && (typecode == 1) && (strcmp(tit,"Mult") == 0))
	return (nam8[nr-1]);
    else if (equad && (typecode == 5))
        return (nam10[nr-1]);
    else if (ebend && (typecode == 4))
        return (nam11[nr-1]);
    else if ((typecode == 1) && (strcmp(tit,"Add ") == 0))
	return (nam9[nr-1]);
    else
    {
	int ind = 0;
	for (int i=1; i<typecode; i++)
	   ind += GetNumberOfDataItems(i,FALSE,Turtle);
	return (nam[ind+nr-1]);
    }
}

char *GetTypecode13Info(int n, BOOL Turtle)
{
    int shift, indmax;
    static char *info[] = {
			" 1:  Print Current Beam Matrix",
			" 2:  Automatic Printing of beam matrix suppressed",
			" 3:  Print Beam Matrix after every Element",
			" 4:  Print Current Transform Matrix R1",
			" 5:  Print Transform Matrix R1 after every Element",
			" 6:  Automatic Printing of Transform R1 suppressed",
			" 7:  Automatic Printing of Element Data suppressed",
			" 8:  Automatic Printing of Element Data resumed",
			" 9:  Print Misalignment Summary Table",
			"10: -",
			"11: -",
			"12: Print Beam Line Coordinate Layout",
			"13: .... 23: -",
			"24: Print Current Transform Matrix R2",
			"25: .... 39: -",
			"40: Bending Magnet: Restore normal options",
			"41: Bending Magnet: Beta1 = Beta2 = Bend-Angle/2",
			"42: Bending Magnet: Beta1 = 0, Beta2 = Bend-Angle",
			"43: Bending Magnet: Beta1 = Bend-Angle, Beta2 = 0",
			"44: -",
			"45: -",
			"46: -",
			"47: Bending Magnet: Second Parameter means Field",
			"48: Bending Magnet: Second Parameter means Angle",

			"10+id (true) or 11+id (false) : for aperture constraint observation",
			"  modifiable along beam line with :",
			" id =   0 : apertures for parent particles",
			" id = 100 : apertures for charged daughter particles",
			" id = 200 : apertures for neutral daughter particles",
			" "
			   };

	if (Turtle)
	{
	   indmax = 5;
	   shift  = 24;
	}
	else	
	{
	   indmax = 24;
	   shift  = 0;
	}

	if ((n>0) && (n<=indmax))
	{
	   return (info[n-1+shift]);
	}
	else
	   return NULL;
}

char *GetTypecode15Info(int n, BOOL Turtle)
{
    int shift, indmax;
    static char *info[] = {
			" 1:  Horizontal and Vertical Transverse Dimensions",
			" 2:  Horizontal and Vertical Divergences",
			" 3:  Vertical Beam Extent only",
			" 4:  Vertical Beam Divergence only",
			" 5:  Pulsed Beam Length",
			" 6:  Momentum Spread",
			" 7:  Bend, Pole Face Rotation and Layout Angles",
			" 8:  Longitudinal Length",
			" 9:  Magnetic Fields",
			"10:  Particle Mass",
			"11:  Beam Momentum",

			" 1:  Horizontal and Vertical Transverse Dimensions",
			" 2:  Horizontal and Vertical Divergences",
			" 3:  -",
			" 4:  -",
			" 5:  -",
			" 6:  Momentum Spread",
			" 7:  Angle",
			" 8:  Longitudinal Length",
			" 9:  Magnetic Fields",
			"10:  Particle Mass",
			"11:  Beam Momentum",
			"12:  Particle Energy"
			  };

	if (Turtle)
	{
	   indmax = 12;
	   shift  = 11;
	}
	else	
	{
	   indmax = 11;
	   shift  = 0;
	}

	if ((n>0) && (n<=indmax))
	{
	   return (info[n-1+shift]);
	}
	else
	   return NULL;
}

char *GetTypecode16Info(int n, BOOL Turtle)
{
    int shift, indmax;
    static char *info[] = {
			" 1:  Bending Magnet Field 2nd Derivative",
			" 2:  -",
			" 3:  Particle Mass (in units of electron mass)",
			" 4:  Bending Magnet horizontal Half-Aperture",
			" 5:  Bending Magnet vertical Half-Aperture",
			" 6:  Cumulative Length of System",
			" 7:  Bending Magnet Fringing Field Integral K1",
			" 8:  Bending Magnet Fringing Field Integral K2",
			" 9:  Periodic Solution Horizontal Emittance",
			"10: Periodic Solution Verticalal Emittance",
			"11: Periodic Solution Period-Length End-Mark",
			"12: Bending Magnet Entrance Face 1/Curvature Radius",
			"13: Bending Magnet Exit Face 1/Curvature Radius",
			"14: Slits instead of Update",
			"15: Tilt of Focal Plane",
			"16: Initial Beam Line Coordinate x0",
			"17: Initial Beam Line Coordinate y0",
			"18: Initial Beam Line Coordinate z0",
			"19: Initial Beam Line Direction theta0",
			"20: Initial Beam Line Direction phi0",
			"21: Limits to 1/Curvature Radius while Fitting",
			"22: Maximum Number of Fit Iterations",
			"23: -",
			"24: -",
			"25: Quadrupole Fringing Field Integral I1",
			"26: Quadrupole Fringing Field Integral I2",
			"27: Quadrupole Fringing Field Integral I3",
			"28: -",
			"29: Particle charge (in electron units, default=1)",
			"30: -",
			"31: Turn on Random Optimizer",
			"32: Turn off Random Optimizer, take Standard one",
			"...",
			"40: Accelerator extended plates feature",
			"...",
			"93: Bend is magnetic (reversal of 94)",
			"94: Bend is an electrostatic Prism",
			"95: Quadrupole is normal (reversal of 96)",
			"96: Quadrupole is an electrostatic Quadrupole",
			"97: Quadrupole is normal (reversal of 98)",
			"98: Quadrupole is a Lithium (Plasma) lens",

			" -N a b: multipole aberration in quadrupole",
			" 1     : Bending Magnet Field 2nd Derivative",
			" 2     : -",
			" 3     : Particle Mass (in Units of Electron mass)",
			" 4     : Bending Magnet horizontal Half-Aperture",
			" 5     : Bending Magnet vertical Half-Aperture",
			" 6  z  : Redefine z for histogramming.",
			" 7     : Bending Magnet Fringing Field Integral K1",
			" 8     : Bending Magnet Fringing Field Integral K2",
			" 9     : Zmax for Decay.",
			"10     : Take entries on beam card as energy instead of momentum.",
			"11     : p0 - design Momentum.",
			"12     : Bending Magnet Entrance Face 1/Curvature Radius",
			"13     : Bending Magnet Exit Face 1/Curvature Radius",
			"14 .... 19: -",
			"20     : Take homogeneous distribution for beam instead of gaussian.",
			"21 xm  : Binomial distribution for horizontal beam parameters.",
			"22 ym  : Binomial distribution for vertical beam parameters.",
			"23 zm  : Binomial distribution for longitudinal beam parameters.",
			"24     : Take 4Vol distribution for beam.",
			"25 I1  : 1st fringing field integral for quadrupoles",
			"26 I2  : 2nd fringing field integral for quadrupoles",
			"27 I3  : 3rd fringing field integral for quadrupoles",
			"28 I4  : 4th fringing field integral for quadrupoles",
			"29 Chg : Particle charge (in electron units, default=1)",
			"30 RAN : Seed for random generator.",
			"31     : Use Halton quasi-random generator for emittance",
			"35 dBQ : Random-add amplitude to Quad pole tip field",
			"36 dp0 : Random spread amplitude of p0 inside Bends",
		        "50 sf1 : Field scaling factor for SOLO 1 (Runge-Kutta)",
		        "51 sf2 : Field scaling factor for SOLO 2 (Runge-Kutta)",
			"93     : Bend is magnetic (reversal of 94)",
			"94     : Bend is an electrostatic Prism",
			"95     : Quadrupole is normal (reversal of 96)",
			"96     : Quadrupole is a electrostatic Quadrupole",
			"97     : Quadrupole is normal (reversal of 98)",
			"98     : Quadrupole is a Lithium (Plasma) lens",
			"100 a b: Elliptical quadrupole aperture.",
			"101 a  : Hyperbolic quadrupole aperture.",
			"160    : Slits are taken inverse.",
			"161    : Slits are taken normal (default).",
			"162    : Slit contours are taken rectangular.",
			"163    : Slit contours are taken elliptical (default).",
			"165    : Enable multiple scattering.",
			"166    : Disable multiple scattering. Restore RMS-scattering (default).",
			"170    : Sign of x'(y')-shift depends on sign of x(y) of rays.",
			"171    : Restore normal meaning of shifts.",
			"172    : x'(y')-shift applies only for positive x(y)-coordinates of rays.",
			"177 dx dy   : Increments for slit dimensions",
			"180 unit id : Output of particle beam vector on diskfile (append mode).",
			"181 unit id : same like above, but instead of Pz, Nr (number of ray) is taken.",
			"190 case unit : Store histogram information on metafile.",
			"191 unit cnt: 3dr order output for first cnt particles.",
			"195 /LABL/  : suppress entry in loss histogram.",
			"196    : Engage logical inversion of histogram flagging.",
			"197    : Restore normal meaning of histogram flagging (default).",
			"198 n  : Concatination factor for list of stopped particles by slits.",
			" "
			  };

	if (Turtle)
	{
	   indmax = 55;
	   shift  = 41;
	}
	else	
	{
	   indmax = 41;
	   shift  = 0;
	}

	if ((n>0) && (n<=indmax))
	{
	   return (info[n-1+shift]);
	}
	else
	   return NULL;
}

VOID CloseHelp(int typecode)
{
  char title[100];

//  titles must be equal to those in the calls of the constructors
//  of the class HelpWindow !!!

  if (typecode == 13)
     strcpy(title,"Help for Typecode 13:");
  else if (typecode == 15)
     strcpy(title,"Help for Typecode 15:");
  else if (typecode == 16)
     strcpy(title,"Help for Typecode 16:");
  else
     strcpy(title,"Help");

  HWND hwnd = FindWindow(NULL,title);
  if (hwnd != NULL)
     SendMessage(hwnd,WM_CLOSE,(WPARAM)0,(LPARAM)0L);
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////// ParDialog class implementation ///////////////////////
///////////////////////////////////////////////////////////////////////////////

ParDialog :: ParDialog(int ResID, Dialog *opDlg, char *ln, char *tt, BOOL upd,
		       BOOL ang, BOOL turt, BOOL eq, BOOL eb)
           : ModalDialog(ResID,opDlg)
{
    hPar = opDlg->GetHandle();
    titel = tt;
    strcpy(line,ln);
    update = upd;
    angle  = ang;
    Turtle = turt;
    equad = eq;
    ebend = eb;
    opText = NULL;

    typecode = 0;
    nr       = 0;
    strcpy(delim,"/");
    strcpy(delim2,"/");

    int   i;
    char  *ptr, temp[40], str1[10];

    t = (char **) new long[20];
    for (i=0; i<20; i++)
    {
	t[i] = new char[21];
	strcpy(t[i],"");
    }

    if (strlen(line) == 0)  // not yet implemented
    {
	Start();
	return;
    }

    for (i=strlen(line)-1; i>=0; i--)
	if (line[i] == ';')
	{
	   line[i] = '\0';  // get rid of trailing ";"
	   break;
	}

    nrblanks = 0;
    for (i=0; i<(int)strlen(line); i++)
	if (line[i] == ' ')  // count leading blanks
	   nrblanks++;
	else
	   break;

    nr = sscanf(line,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
	 t[0],t[1],t[2],t[3],t[4],t[5],t[6],t[7],t[8],t[9],t[10],
	 t[11],t[12],t[13],t[14],t[15],t[16],t[17],t[18],t[19]);

    strcpy(temp,t[0]);
    ptr = strstr(temp,".");
    if (!ptr)			// no point
    {
        strcpy(str1,temp);	// typecode without point
	strcpy(varycodes,"");	// no varycode
    }
    else
    {
	*ptr = '\0';
	strcpy(str1,temp);	 // before point = typecode
        strcpy(varycodes,++ptr); // after point  = varycodes
    }

    sscanf(str1,"%d",&typecode);	  
    sign = 1;
    if (typecode < 0)		 // minus sign in front of typecode 
	sign = -1;
    typecode = abs(typecode);

    strcpy(label,"");
    i = nr-1;
    if ((t[i][0] == '/') || (t[i][0] == '\'') || (t[i][0] == '\"') )   
    {
	delim[0] = t[i][0];
	strcpy(label,&t[i][1]);
	strcpy(t[i],"");
	nr--;
    }

    for (i=0; i<20; i++)
	if ((label[i] == '/') || (label[i] == '\'') || (label[i] == '\"') )
	{
		label[i] = '\0';
		break;
	}

    if (typecode == 15)
    {
	if ((t[2][0] == '/') || (t[2][0] == '\'') || (t[2][0] == '\"') )
	{
	   delim2[0] = t[2][0];
	   int l = strlen(t[2]);
	   t[2][l-1] = '\0';
	   char temp[10];
	   strcpy(temp,&t[2][1]);
	   strcpy(t[2],temp);
	}
    }

    Start();
}

ParDialog :: ~ParDialog(VOID)
{
    int i;

    for (i=0; i<20; i++)
       delete []t[i];
    delete []t;

    for (i=1; i<=GetNumberOfDataItems(typecode,update,Turtle); i++)
    {
	   delete opPar[i];
	   delete opNam[i];
    }

    if (typecode > 0)
    {
	delete opDummy;
	delete opSign;
	delete opType;
	delete opVary;
	delete opLabel;
    }
    if (opText != NULL)
	delete opText;
}

BOOL ParDialog :: Init(VOID)
{
   char temp[20];
   int  i;

// Title and Errors

   CenterDialog(hPar,this->GetHandle());
   SendMessage(this->GetHandle(),WM_SETTEXT,(WPARAM)0,(LPARAM)titel);
   opDummy  = new StaticText(IDC_TEXT,this);
   opDummy->SetText("Not yet implemented !");

   if (typecode == 0)
      return FALSE;

// SignumTypecode.Varycodes

   opSign  = new StringEdit(IDC_SIGN,this);
   if (sign < 0)
	strcpy(temp,"-");
   else
	strcpy(temp,"");
   opSign->SetText(temp);
   opSign->SetCharLimit(1);
   opType  = new StaticText(IDC_TYPE,this);
   sprintf(temp,"%d.",typecode);
   opType->SetText(temp);
   opVary  = new StringEdit(IDC_VARY,this);
   opVary->SetText(varycodes);
   int nrv = GetNumberOfVaryCodes(typecode,Turtle);
   if (nrv > 0)
      opVary->SetCharLimit(nrv);
   else
      EnableWindow(opVary->GetHandle(),FALSE); // special treatment for nrv = 0

// Data fields

   for (i=1; i<=GetNumberOfDataItems(typecode,update,Turtle); i++)
   {
      opPar[i] = new StringEdit(IDC_PAR1+i-1,this);
      if (opPar[i])
	 opPar[i]->SetText(t[i]);
      opNam[i] = new StaticText(IDC_NAM1+i-1,this);
      if (opNam[i])
	 opNam[i]->SetText(GetNameOfParam(typecode,i,update,angle,Turtle,equad,ebend,titel));
   }

   if ( (typecode == 13) || (typecode == 15) || (typecode == 16) )
	;
   else    // hide special marker box with ID = IDC_HIDE
   {
	StaticText *opFrame = new StaticText(IDC_HIDE,this);
	HWND hwnd = opFrame->GetHandle();
	if (hwnd != NULL)
	   ShowWindow(hwnd,SW_HIDE);
	delete opFrame;
   }

   if (typecode == 15)
      opPar[2]->SetCharLimit(4);
   
// Label

   opLabel = new StringEdit(IDC_LABEL,this);
   opLabel->SetText(label);
   opLabel->SetCharLimit(4);

// Focus

   if (opPar[1] != NULL)
   {
      opPar[1]->SetCrntSelection(strlen(t[1]),strlen(t[1]));
      opPar[1]->SetFocus();
   }
   return FALSE;
}

#define HMIN 3

long ParDialog :: WmPaint(HWND hDlg, PAINTSTRUCT &ps)
{
//  This paint routine draws a series of shades in dialogboxes
//  Controls have to have IDs of 1000, 1001, 1002 ....

    HWND hShade;     // handle of Control with ID>=1000:
    UINT ID = 1000;  // Control "",1000,"static",SS_SIMPLE |...,x,y,w,h
    HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    HPEN hPen1 = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
    HPEN hPen2 = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNHIGHLIGHT));

    if (GetDlgItem(hDlg,1000) == 0)  // not there --> nothing to do here!!
       goto BitMap;

    while ( (hShade = GetDlgItem(hDlg,ID)) != 0 )
    {
          RECT   rcDlg, rcShade, cliprgn;
          HBRUSH hOldBrush;
          HPEN   hOldPen;
#ifdef WIN32
          POINT  point;
#endif

          GetWindowRect(hDlg,&rcDlg);  // Dialog, relative to screen origin
          int OrigX = rcDlg.left + GetSystemMetrics(SM_CXFRAME);
          int OrigY = rcDlg.top  + GetSystemMetrics(SM_CYFRAME)
                                 + GetSystemMetrics(SM_CYCAPTION);

          GetWindowRect(hShade,&rcShade); // Control: relative to screen origin
          rcShade.left   -= OrigX;        // make relative to Dialog
          rcShade.top    -= OrigY;
          rcShade.right  -= OrigX;
          rcShade.bottom -= OrigY;

          IntersectRect(&cliprgn,&ps.rcPaint,&rcShade);
          HRGN hClipRgn = CreateRectRgnIndirect(&cliprgn);
          SelectClipRgn(ps.hdc,hClipRgn);
                    
          hOldBrush = (HBRUSH)SelectObject( ps.hdc, hBrush );
          hOldPen   = (HPEN)SelectObject( ps.hdc, hPen1 );

          int height = rcShade.bottom - rcShade.top;

          if ( height > HMIN )  // h in dialog resource > 1
             Rectangle(ps.hdc, rcShade.left,  rcShade.top, 
                               rcShade.right, rcShade.bottom);
          else
          {  
#ifndef WIN32
             MoveTo(ps.hdc, rcShade.left,    rcShade.bottom-2);
#else
             MoveToEx(ps.hdc, rcShade.left,  rcShade.bottom-2, &point);
#endif
             LineTo(ps.hdc, rcShade.right,   rcShade.bottom-2);
          }

          SelectObject( ps.hdc, hPen2 );

          if ( height > HMIN )
          {  
#ifndef WIN32
             MoveTo(ps.hdc, rcShade.right-1,   rcShade.top);
#else
             MoveToEx(ps.hdc, rcShade.right-1, rcShade.top, &point);
#endif
             LineTo(ps.hdc, rcShade.right-1,   rcShade.bottom);
          }
#ifndef WIN32
          MoveTo(ps.hdc, rcShade.left,    rcShade.bottom-1);
#else
          MoveToEx(ps.hdc, rcShade.left,  rcShade.bottom-1, &point);
#endif
          LineTo(ps.hdc, rcShade.right,   rcShade.bottom-1);

          SelectObject( ps.hdc, hOldBrush );
          SelectObject( ps.hdc, hOldPen );
          DeleteObject( hClipRgn );

          ID++;  // loop until sequence 1000, 1001, 1002 ... is finished
    }
    DeleteObject( hBrush );
    DeleteObject( hPen1 );
    DeleteObject( hPen2 );

//  Draw Bitmap in static frame

BitMap:

    {
	HBITMAP hBitmap;
	if (isLarge(hPar))
	   hBitmap = LoadBitmap(Application::GetInstance(),"MyIcons2");
	else
	   hBitmap = LoadBitmap(Application::GetInstance(),"MyIcons");
	HDC hdcMem = CreateCompatibleDC(ps.hdc);
	HBITMAP hbmpOld = (HBITMAP) SelectObject(hdcMem,hBitmap);

	int ipos1;
	if ((typecode == 6) && update)
	   ipos1 = typecode + 34;
	else if ((typecode == 6) && !update)
	   ipos1 = typecode + 33;   // 2-dim slit
	else if (typecode >= 50)
	{
	    if (!Turtle)
	        ipos1 = typecode - 18;
	    else
	        ipos1 = typecode - 15;
	}
	else
	    ipos1 = typecode + 4;    

	RECT rc, rcDlg;
	GetWindowRect(this->GetHandle(),&rcDlg);  // Dialog, relative to screen origin
	GetWindowRect(GetDlgItem(this->GetHandle(),IDC_FRAM),&rc);
	int OrigX = rc.left - rcDlg.left - GetSystemMetrics(SM_CXFRAME);
	int OrigY = rc.top - rcDlg.top  - GetSystemMetrics(SM_CYFRAME)
					- GetSystemMetrics(SM_CYCAPTION);
	int isize;
	int ishift;
	if (!isLarge(hPar))
	{
	   isize  = 15;
	   ishift = 3;
	}
	else
	{
	   isize  = 19;
	   ishift = 4;
	}
	BitBlt(ps.hdc,
	       OrigX+ishift,OrigY+ishift,
	       isize,isize,
	       hdcMem,isize*ipos1,0,SRCCOPY);

	SelectObject(hdcMem,hbmpOld);
	DeleteDC(hdcMem);
        DeleteObject(hBitmap);
    }

    return FALSE;
}

long ParDialog :: WmRightMouseDown(POINT &pos)
{
   return WmLeftMouseDown(pos);
}

void ParDialog :: Fillin(int typecode, BOOL Turtle)
{
   char *ptr;
   int i = 1;
   switch(typecode)
   {
      case 13:
	{
	   for (;;)
	   {
		ptr = GetTypecode13Info(i,Turtle);
		if (ptr)		
		{
		   opText->WriteLn(ptr);
		   i++;
		}
		else
		   break;
	   }
	}
	break;
      case 15:
	{
	   for (;;)
	   {
		ptr = GetTypecode15Info(i,Turtle);
		if (ptr)		
		{
		   opText->WriteLn(ptr);
		   i++;
		}
		else
		   break;
	   }
	}
	break;
      case 16:
	{
	   for (;;)
	   {
		ptr = GetTypecode16Info(i,Turtle);
		if (ptr)		
		{
		   opText->WriteLn(ptr);
		   i++;
		}
		else
		   break;
	   }
	}
	break;
      default:
	break;
   }
}

long ParDialog :: WmLeftMouseDown(POINT &pos)
{
    RECT rc, rcDlg;
    GetWindowRect(this->GetHandle(),&rcDlg);  // Dialog, relative to screen origin
    GetWindowRect(GetDlgItem(this->GetHandle(),IDC_HIDE),&rc);
    int OrigX = rc.left - rcDlg.left - GetSystemMetrics(SM_CXFRAME);
    int OrigY = rc.top - rcDlg.top  - GetSystemMetrics(SM_CYFRAME)
				    - GetSystemMetrics(SM_CYCAPTION);
    int EndX = OrigX + rc.right - rc.left;
    int EndY = OrigY + rc.bottom - rc.top;
    if ((pos.x>OrigX) && (pos.x<EndX) && (pos.y>OrigY) && (pos.y<EndY))  // Par[1] field
    {
	   int x=0, y=0, w=500, h=350;
	   if (isLarge(hPar))
	   {
		w = 625;
		h = 438;	
	   }
	   GetWindowCoordnates(this->GetHandle(), x, y, w, h);
	   RECT rc = {x,y,w,h};
	   switch (typecode)
	   {
		case 13:
		  {
			if (opText)
			   delete opText;
			opText = new TextWindow(SW_SHOWNORMAL,"Help for Typecode 13:",rc,hPar);
			Fillin(13, Turtle);
			opText->Home();
		  }
		  break;
		case 15:
		  {
			if (opText)
			   delete opText;
			opText = new TextWindow(SW_SHOWNORMAL,"Help for Typecode 15:",rc,hPar);
			Fillin(15, Turtle);
			opText->Home();
		  }
		  break;
		case 16:
		  {
			if (opText)
			   delete opText;
			opText = new TextWindow(SW_SHOWNORMAL,"Help for Typecode 16:",rc,hPar);
			Fillin(16, Turtle);
			opText->Home();
		  }
		  break;
		default:
		  break;
	   }
    }
    return FALSE;
}

long ParDialog :: Control( UINT nControlId, UINT )
{ 
    switch (nControlId)
    {
	case IDOK:
        case IDYES:
	  {
	    if (typecode > 0)
	    {
		opSign->GetText(sigstr,5);
		opVary->GetText(varycodes,40);

		for (int i=1; i<=GetNumberOfDataItems(typecode,update,Turtle); i++)
		   opPar[i]->GetText(t[i],20);

		opLabel->GetText(label,20);
	    }
	    CloseHelp(typecode);
            TerminateDialog();
            result = TRUE;
	  }
          break;
        case IDCANCEL:
        case IDNO:
	case IDC_OKAY:
	  
	  CloseHelp(typecode);
          TerminateDialog();
          result = FALSE;
          break;
    }
    return FALSE;
}

BOOL ParDialog :: GetResult(VOID)
{
    return(result);
}

char *ParDialog :: GetString( void )
{
   char temp[30];
   int  i;

   strcpy(line,"");
   for (i=0; i<nrblanks; i++)
	strcat(line," ");

   if (strcmp(sigstr,"-") == 0)
	typecode *= -1;
   sprintf(temp,"%d.%s ",typecode,varycodes);
   strcat(line,temp);

   for (i=1; i<=GetNumberOfDataItems(typecode,update,Turtle); i++)
   {
	if (strlen(t[i]) > 0)
	{
	   if ((typecode == 15) && (i == 2) ) strcat(line,delim2);
	   strcat(line,t[i]);
	   if ((typecode == 15) && (i == 2) ) strcat(line,delim2);
	   strcat(line," ");
	}
   }
   if (strlen(label) > 0)   // label
   {
	char temp[10];
	sprintf(temp,"%c%s%c ",delim[0],label,delim[0]);
	strcat(line,temp);
   }
   strcat(line,";");
   return line;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////// TEditDialog class implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

TEditDialog :: TEditDialog(int ResID, Dialog *opDlg, LPCSTR text, UINT wl)
             : ModalDialog(ResID,opDlg)
{
   wLen = wl;
   hPar = opDlg->GetHandle();
   fixedfont = FALSE;
   SetString(text);
   Start();
}

TEditDialog :: TEditDialog(int ResID, Dialog *opDlg, LPCSTR text, UINT wl,
			     BOOL ff)
             : ModalDialog(ResID,opDlg)
{
   wLen = wl;
   hPar = opDlg->GetHandle();
   fixedfont = ff;
   SetString(text);
   Start();
}

TEditDialog :: ~TEditDialog(VOID)
{
    delete opEdit;
}

BOOL TEditDialog :: Init(VOID)
{
   CenterDialog(hPar,this->GetHandle());

   if (fixedfont)
       SendDlgItemMessage(this->GetHandle(),IDC_EDIT,WM_SETFONT,
			(WPARAM)GetStockObject(SYSTEM_FIXED_FONT),FALSE);

// transfer text to stringedit

   opEdit = new StringEdit(IDC_EDIT,this);
   opEdit->SetText(caString);
   opEdit->SetCharLimit(wLen);
   opEdit->SetCrntSelection(strlen(caString),strlen(caString));
   opEdit->SetFocus();

   return FALSE;
}

long TEditDialog :: Control( UINT nControlId, UINT )
{ 
    switch (nControlId)
    {
        case IDC_EXIT:
          opEdit->GetText(caString,1001);
          caString[wLen] = '\0';
          TerminateDialog();
          result = TRUE;
          break;
        case IDCANCEL:
          TerminateDialog();
          result = FALSE;
          break;
    }
    return FALSE;
}

BOOL TEditDialog :: GetResult(VOID)
{
    return(result);
}

VOID TEditDialog :: SetString(LPCSTR cpString)
{
    strcpy(caString,cpString);
}

char *TEditDialog :: GetString( void )
{
    return caString;
}

//////////////////////////////////// EOF //////////////////////////////////////

