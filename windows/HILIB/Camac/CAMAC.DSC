; CAMAC.DEF -- Definition file for CAMAC.DLL

LIBRARY     CAMAC
DESCRIPTION 'DLL for CAMAC access'
EXETYPE     NT
VERSION     1.0
STUB        'WINSTUB.EXE'
CODE        PRELOAD MOVEABLE DISCARDABLE
DATA        PRELOAD MOVEABLE SINGLE
HEAPSIZE    16384
EXPORTS     IniDevList
            SetDac
            SetDacWait
            GetDac
            GetAdc
            GetDevName
            GetDevParam
            DelDevList
            IniSerialComm
            CloseSerialComm
            IniKSI
            WriteCamac
            ReadCamac
            ControlCamac
	    WriteCamacDac
	    ReadCamacDac
            getq
            getx
            WriteDAC
            ReadDAC
            ReadADC
            ReserveCrate
            FreeCrate
            ReadScaler
            ClearScaler
            SwitchOnCombis
            SwitchOnCombi
            GetNumberOfPages
            GetPageIndex
            YieldQQ
            wait
	    ReadAll
