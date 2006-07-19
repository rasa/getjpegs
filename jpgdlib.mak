# Microsoft Developer Studio Generated NMAKE File, Based on jpgdlib.dsp
!IF "$(CFG)" == ""
CFG=jpgdlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to jpgdlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "jpgdlib - Win32 Release" && "$(CFG)" != "jpgdlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpgdlib.mak" CFG="jpgdlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpgdlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jpgdlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jpgdlib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\jpgdlib.lib"


CLEAN :
	-@erase "$(INTDIR)\h2v2.obj"
	-@erase "$(INTDIR)\idct.obj"
	-@erase "$(INTDIR)\jidctfst.obj"
	-@erase "$(INTDIR)\jpegdecoder.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\jpgdlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/ML /W3 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USE_NON_INTEL_COMPILER" /Fp"$(INTDIR)\jpgdlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\jpgdlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\jpgdlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\h2v2.obj" \
	"$(INTDIR)\idct.obj" \
	"$(INTDIR)\jidctfst.obj" \
	"$(INTDIR)\jpegdecoder.obj"

"$(OUTDIR)\jpgdlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "jpgdlib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\jpgdlib_d.lib"


CLEAN :
	-@erase "$(INTDIR)\h2v2.obj"
	-@erase "$(INTDIR)\idct.obj"
	-@erase "$(INTDIR)\jidctfst.obj"
	-@erase "$(INTDIR)\jpegdecoder.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\jpgdlib_d.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/MLd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USE_NON_INTEL_COMPILER" /Fp"$(INTDIR)\jpgdlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\jpgdlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\jpgdlib_d.lib" 
LIB32_OBJS= \
	"$(INTDIR)\h2v2.obj" \
	"$(INTDIR)\idct.obj" \
	"$(INTDIR)\jidctfst.obj" \
	"$(INTDIR)\jpegdecoder.obj"

"$(OUTDIR)\jpgdlib_d.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("jpgdlib.dep")
!INCLUDE "jpgdlib.dep"
!ELSE 
!MESSAGE Warning: cannot find "jpgdlib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "jpgdlib - Win32 Release" || "$(CFG)" == "jpgdlib - Win32 Debug"
SOURCE=.\h2v2.cpp

!IF  "$(CFG)" == "jpgdlib - Win32 Release"

CPP_SWITCHES=/ML /W3 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\jpgdlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\h2v2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "jpgdlib - Win32 Debug"

CPP_SWITCHES=/MLd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\jpgdlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\h2v2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\idct.cpp

"$(INTDIR)\idct.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\jidctfst.cpp

"$(INTDIR)\jidctfst.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\jpegdecoder.cpp

"$(INTDIR)\jpegdecoder.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

