 format pe; console
 entry start
 include 'win32ax.inc'
 section '.code' code readable executable

start:
   cinvoke scanf, fl, varq
   cinvoke printf, fl, dword[varq], dword[varq+4]
   cinvoke printf, cr
   cinvoke scanf, f4, vard
   cinvoke printf, f4, dword[vard] ; not work
exit:
   invoke ExitProcess, 0;

section '.data' data readable writeable
 cr db 13,10,0
 f4 db '%f', 0
 fl db '%lf', 0
 varq dq ?
 vard dd ?
section '.idata' import data readable writeable
library msvcrt, 'msvcrt.dll', kernel32, 'kernel32.dll'
import msvcrt, printf, 'printf', getchar,'getchar', scanf,'scanf'
import kernel32, ExitProcess,'ExitProcess'