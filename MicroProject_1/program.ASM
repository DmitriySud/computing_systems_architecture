;Разработать программу, заменяющую все
;гласные буквы в заданной ASCII-строке
;заглавными

format          PE console 4.0
entry           start

include         'win32a.inc'
include 	'encoding/win1251.inc'
 

 
section         '.code' code readable executable
 
  start:
	cinvoke printf, outEnterMessage
        cinvoke scanf, inStringTemp, strBefore
	cinvoke printf, endl
	
	cinvoke printf, outStringBefore, strBefore
		



  exit:
	cinvoke _getch

        invoke  ExitProcess, 0


section         '.data' data readable writeable
	outEnterMessage		db 	'Enter Your string (no more than 255 symbols): ', 0
        inStringTemp		db	'%255[^',0Ah,']', 0
	endl			db	10,13,0
	outStringBefore		db	'String before : %s', 10, 13, 0
	outStringAfter		db	'String after : %s', 10, 13, 0
	strBefore		db	256 dup(0)
	
	fromString 		db 	
	

section         '.idata' import data readable writeable
 
library         kernel32,'KERNEL32.DLL',\
                msvcrt, 'msvcrt.dll'
 
import          kernel32,\ 
                GetStdHandle,'GetStdHandle',\ 
                WriteConsole,'WriteConsoleA',\
                ReadConsole,'ReadConsoleA',\
                ExitProcess,'ExitProcess'
import msvcrt,\
       __getmainargs,'__getmainargs',\
       fopen,'fopen',\
       fseek,'fseek',\
       ftell,'ftell',\
       malloc,'malloc',\
       free,'free',\
       fread,'fread',\
       fclose,'fclose',\
       printf,'printf',\
       scanf,'scanf',\
       _getch,'_getch',\
       puts,'puts'
