#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#ifdef __BORLANDC__
#include <dir.h>
#else
#include <direct.h>
#endif
#include <windows.h>
#include <winsock.h>
#include <commdlg.h>
#ifdef WIN95
#include <commctrl.h>
#endif
#ifndef WIN32
#include <toolhelp.h>
#include <ntdefs.h>
#endif
#include <manager.hpp>
#include <drwtools.hpp>
#include <dispdev.hpp>
#include <window.hpp>
#include <metadev.hpp>
#include <dialog.hpp>
#include <control.hpp>
#include <simpdlg.hpp>
// #include <camac.hpp>
#include <printer.hpp>
#include <defines.h>
#include <minmax.h>
#include <colorbut.h>
#include <colorbut.hpp>

#include "tpmenu.h"
#include "tcp.hpp"
#include "filebuff.hpp"
#include "calidata.hpp"
#include "varidlg.hpp"
#include "textwin.hpp"
#ifndef WIN32
#include <listhapi.h>
#include "stextwin.hpp"
#endif
#ifdef WIN95
#include <listhapi.h>
#include "stextwin.hpp"
#endif
#include "toutfile.hpp"
#include "tranfile.hpp"
#include "tgrafile.hpp"
#include "tsecfile.hpp"
#include "dispfile.hpp"
#include "dispdlg.hpp"
#include "trinfile.hpp"
#include "tgrafwin.hpp"
#include "teliwin.hpp"
#include "teliwin2.hpp"
#include "teliwin3.hpp"
#include "flatdlg.hpp"
#include "popwin.hpp"
#include "tpmenu.hpp"
