LIBRARY         LISTHSCR
DESCRIPTION     'Windows Horizontal Listbox Scrolling Methods, Kraig Brockschmidt'
EXETYPE         NT
VERSION         1.0
CODE            PRELOAD MOVEABLE DISCARDABLE
DATA            PRELOAD MOVEABLE SINGLE
HEAPSIZE        2048
EXPORTS         FInitListboxExtents @2
                WAddExtentEntry     @3  RESIDENTNAME
                WRemoveExtentEntry  @4
                FFreeListboxExtents @5
                ResetListboxExtents @6
