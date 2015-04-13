# Microsoft Developer Studio Generated NMAKE File, Based on getjpegs.dsp
!IF "$(CFG)" == ""
CFG=getjpegs - Win32 Debug
!MESSAGE No configuration specified. Defaulting to getjpegs - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "getjpegs - Win32 Release" && "$(CFG)" != "getjpegs - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "getjpegs.mak" CFG="getjpegs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "getjpegs - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "getjpegs - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "getjpegs - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\getjpegs.exe"

!ELSE 

ALL : "jpgdlib - Win32 Release" "$(OUTDIR)\getjpegs.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"jpgdlib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\getjpegs.obj"
	-@erase "$(INTDIR)\getjpegs.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\getjpegs.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\getjpegs.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\getjpegs.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\getjpegs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\getjpegs.pdb" /machine:I386 /out:"$(OUTDIR)\getjpegs.exe" 
LINK32_OBJS= \
	"$(INTDIR)\getjpegs.obj" \
	"$(INTDIR)\getjpegs.res" \
	"$(OUTDIR)\jpgdlib.lib"

"$(OUTDIR)\getjpegs.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "getjpegs - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\getjpegs.exe" "$(OUTDIR)\getjpegs.bsc"

!ELSE 

ALL : "jpgdlib - Win32 Debug" "$(OUTDIR)\getjpegs.exe" "$(OUTDIR)\getjpegs.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"jpgdlib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\getjpegs.obj"
	-@erase "$(INTDIR)\getjpegs.res"
	-@erase "$(INTDIR)\getjpegs.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\getjpegs.bsc"
	-@erase "$(OUTDIR)\getjpegs.exe"
	-@erase "$(OUTDIR)\getjpegs.ilk"
	-@erase "$(OUTDIR)\getjpegs.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\getjpegs.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\getjpegs.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\getjpegs.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\getjpegs.sbr"

"$(OUTDIR)\getjpegs.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\getjpegs.pdb" /debug /machine:I386 /out:"$(OUTDIR)\getjpegs.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\getjpegs.obj" \
	"$(INTDIR)\getjpegs.res" \
	"$(OUTDIR)\jpgdlib_d.lib"

"$(OUTDIR)\getjpegs.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("getjpegs.dep")
!INCLUDE "getjpegs.dep"
!ELSE 
!MESSAGE Warning: cannot find "getjpegs.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "getjpegs - Win32 Release" || "$(CFG)" == "getjpegs - Win32 Debug"
SOURCE=.\getjpegs.cpp

!IF  "$(CFG)" == "getjpegs - Win32 Release"


"$(INTDIR)\getjpegs.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "getjpegs - Win32 Debug"


"$(INTDIR)\getjpegs.obj"	"$(INTDIR)\getjpegs.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\getjpegs.rc

"$(INTDIR)\getjpegs.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "getjpegs - Win32 Release"

"jpgdlib - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\jpgdlib.mak CFG="jpgdlib - Win32 Release" 
   cd "."

"jpgdlib - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\jpgdlib.mak CFG="jpgdlib - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "getjpegs - Win32 Debug"

"jpgdlib - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\jpgdlib.mak CFG="jpgdlib - Win32 Debug" 
   cd "."

"jpgdlib - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\jpgdlib.mak CFG="jpgdlib - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

