@echo off
cl /c /MT /EHs- /EHa- /wd4530  /DTARGET_WINDOWS /DBIGARRAY_MULTIPLIER=1 /DUSING_XED /D_CRT_SECURE_NO_DEPRECATE /D_SECURE_SCL=0 /nologo /Gy /O2 /DTARGET_IA32 /DHOST_IA32  /I..\..\..\source\tools\..\..\source\include /I..\..\..\source\tools\..\..\source\include\gen  /I..\InstLib /I..\..\..\source\tools\..\..\extras\xed2-ia32\include /I..\..\..\source\tools\..\..\extras\components\include /Foobj-ia32\detect.o detect.cpp

cl /c /MT /EHs- /EHa- /wd4530  /DTARGET_WINDOWS /DBIGARRAY_MULTIPLIER=1 /DUSING_XED /D_CRT_SECURE_NO_DEPRECATE /D_SECURE_SCL=0 /nologo /Gy /O2 /DTARGET_IA32 /DHOST_IA32  /I..\..\..\source\tools\..\..\source\include /I..\..\..\source\tools\..\..\source\include\gen  /I..\InstLib /I..\..\..\source\tools\..\..\extras\xed2-ia32\include /I..\..\..\source\tools\..\..\extras\components\include /Foobj-ia32\set.o set.cpp


cl /c /MT /EHs- /EHa- /wd4530  /DTARGET_WINDOWS /DBIGARRAY_MULTIPLIER=1 /DUSING_XED /D_CRT_SECURE_NO_DEPRECATE /D_SECURE_SCL=0 /nologo /Gy /O2 /DTARGET_IA32 /DHOST_IA32  /I..\..\..\source\tools\..\..\source\include /I..\..\..\source\tools\..\..\source\include\gen  /I..\InstLib /I..\..\..\source\tools\..\..\extras\xed2-ia32\include /I..\..\..\source\tools\..\..\extras\components\include /Foobj-ia32\stack.o stack.cpp


link /DLL /EXPORT:main /NODEFAULTLIB  /NOLOGO /INCREMENTAL:NO  /OPT:REF  /MACHINE:x86 /ENTRY:Ptrace_DllMainCRTStartup@12 /BASE:0x55000000    /LIBPATH:..\..\..\source\tools\..\..\ia32\lib /LIBPATH:..\..\..\source\tools\..\..\ia32\lib-ext  /LIBPATH:..\..\..\source\tools\..\..\extras\xed2-ia32\lib /IMPLIB:obj-ia32\detect.lib /PDB:obj-ia32\detect.pdb /OUT:obj-ia32\detect.dll obj-ia32\detect.o obj-ia32\set.o obj-ia32\stack.o pin.lib libxed.lib libcpmt.lib libcmt.lib pinvm.lib kernel32.lib ntdll-32.lib