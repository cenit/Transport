LIBRARY     ANAMTR
DESCRIPTION 'Dialog Control Library /AnalogMeter/'
EXETYPE     NT
VERSION     1.0
CODE        PRELOAD MOVEABLE DISCARDABLE
DATA        PRELOAD MOVEABLE SINGLE
HEAPSIZE    1024
EXPORTS     AnaMtrInfo    @2
            AnaMtrStyle   @3
            AnaMtrFlags   @4
            AnaMtrWndFn   @5
            AnaMtrDlgFn   @6
            AnaMtrInfoRW  @7
            AnaMtrFlagsRW @8
            ListClasses   @9 RESIDENTNAME
