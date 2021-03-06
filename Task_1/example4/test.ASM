format   PE Console 4.0
include  'win32a.inc'
include  'api\kernel32.inc'
entry    start

section '.data' data readable writeable
;------
fName     db  'temp.txt',0        ; ??? ?????
buff      db  'start'                  ; ????? ??? ??????
len       =   $ - buff                 ; ????? ??????
handle    dd  0                        ; ?????????? ?????

section '.text' code readable executable
;------
start:  invoke  _lopen, fName, 1               ; ??????? ????. 1 = R\W
        mov     [handle],eax                   ;   ..???????? ??? ??????????
        invoke  _lwrite, [handle], buff, len   ; ?????? ? ????
        invoke  _lclose, [handle]              ; ??????? ????

        invoke  ExitProcess, 0                 ; ?? ?????!

;????????????????????????????????????
section '.idata' import data readable

  library kernel, 'KERNEL32.DLL'
  import  kernel,\                     ; API ??? ??????? ?? ??????32.
             _lopen,'_lopen',\
             _lwrite,'_lwrite',\
             _lclose,'_lclose',\
              ExitProcess, 'ExitProcess'