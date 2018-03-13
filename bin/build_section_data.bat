@echo off
REM findstr doesn't interpret a range expression "[A-Z]" in the 
REM proper way (see "Regex character class ranges [x-y]" from
REM http://ss64.com/nt/findstr.html), so don't even consider to 
REM simplify the following:
findstr /V "^[ABCDEFGHIJKLMNOPQRSTUVWXYZ]" <bin\annex-f | findstr /V "^[0-9]" | findstr /V "ISO/IEC" | findstr /V "^$" >bin\index
bin\section_data <bin\index >bin\section.data

if exist bin\networking-annex-f (
findstr /V "^[ABCDEFGHIJKLMNOPQRSTUVWXYZ]" <bin\networking-annex-f | findstr /V "^[0-9]" | findstr /V "ISO/IEC" | findstr /V "^$" >bin\networking-index
bin\section_data networking.ts <bin\networking-index >bin\networking-section.data
copy bin\networking-section.data meta-data\networking-section.data
)

type meta-data\networking-section.data >>bin\section.data
type meta-data\tr1_section.data >>bin\section.data
dir bin\section.data
