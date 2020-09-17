format PE CONSOLE

include 'win32ax.inc'

entry start

section '.text' data readable executable

start:

        invoke GetHandle,0 ; Sorry i always use this
        invoke GetDesktop
        mov [hDesktop],eax
        invoke GetRect,[hDesktop],lpRect
        invoke printf,msg,[lpRect.right], [lpRect.bottom],[lpRect.top] ,[lpRect.left]
        invoke system,'pause>null'
        invoke Return,0

section '.data' data writable readable

        lpRect RECT ?

        msg DB 'Right %d, Bottom %d, Top %d, Left %d',0

        hDesktop DD ?

section '.idata' import data readable



library kernel,\
          'KERNEL32.DLL',\
          user,'USER32.DLL',\
          msvcrt,'msvcrt.dll'

import user,\
          GetRect,'GetWindowRect',\
          GetDesktop,'GetDesktopWindow'
          

import msvcrt,\
         system,'system',\
         printf,'printf'
         
         
import kernel,\
          GetHandle,'GetModuleHandleA',\
          Return,'ExitProcess' 


