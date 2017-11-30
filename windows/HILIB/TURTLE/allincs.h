#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef __BORLANDC__
#include <dir.h>
#else
#include <direct.h>
#endif
#include <windows.h>
#include <commdlg.h>
#ifdef WIN95
#include <commctrl.h>
#endif
#ifndef WIN32
#include <ntdefs.h>
#endif
#include <anamtr.h>
#include <rotknob.h>
#include <anamtr.hpp>
#include <rotknob.hpp>
#include <manager.hpp>
#include <drwtools.hpp>
#include <dispdev.hpp>
#include <window.hpp>
#include <metadev.hpp>
#include <dialog.hpp>
#include <control.hpp>
#include <simpdlg.hpp>
#include <printer.hpp>
#include <defines.h>
#include <colorbut.hpp>
#include <minmax.h>
#include "ttmenu.h"

#ifdef WIN32
#include <ctl3d.h>
#endif
#include "axis.hpp"
#include "iofile.hpp"
#ifdef WIN32
#ifdef WIN95
#include <listhapi.h>
#include "stextwin.hpp"
#else
#include "textwin.hpp"
#endif
#else
#include <listhapi.h>
#include "stextwin.hpp"
#endif
#include "meterdlg.hpp"
#include "graf3d.hpp"
#include "tugdlg.hpp"
#include "muscdlg.hpp"
#include "hplotdlg.hpp"
#include "sgrafwin.hpp"
#include "popwin.hpp"
#include "ttmenu.hpp"
