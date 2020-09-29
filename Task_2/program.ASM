;Разработать программу, которая вводит одномерный массив A[N], формирует 
;из элементов массива A новый массив B по правилам, указанным в таблице, 
;и выводит его. Память под массивы может выделяться как статически, так 
;и динамически по выбору разработчика.
;
;Вариант 2. Массив В из элементов A, значение которых не совпадает с первым и последним элементами A
;
format          PE console 4.0
entry           start

include         'win32a.inc'
 

 
section         '.code' code readable executable
 
  start:
	
        cinvoke printf, input				; Вывод сообщения и считывание размера масиива
        cinvoke scanf, ioDigitTemp, N
	test eax, eax
	jz .readError

        cinvoke printf, cr

	stdcall readArray, [N]				; Считывание массива
	mov [arrayA_ptr], eax
	cmp eax, 0
	je .readError
        cinvoke printf, cr
	

	stdcall printArray, [arrayA_ptr], [N]           ; Вывод массива
        cinvoke printf, cr
	

	stdcall FilterArrayExceptFirstAndLast, [arrayA_ptr], [N], [arrayB_ptr], [sizeB]
	mov [arrayB_ptr], eax				; Фильтрация массива и получение нового
	mov [sizeB], ebx

	cinvoke printf, FilteredArraySize, [sizeB]     	; Вывод полученного массива
	stdcall printArray, [arrayB_ptr], [sizeB]

	jmp exit

.readError:
	cinvoke printf, readError                     	; Сообщение об ошибке, если такое появилось
		



  exit:
        cinvoke free, [arrayA_ptr]			; Освобождение памяти и выход 
	cinvoke free, [arrayB_ptr]

        invoke  ExitProcess, 0


proc FilterArrayExceptFirstAndLast uses ecx, pointerA, sizeA, pointerB, sizeB
locals
	firstElemL 	dd 	?
	lastElemL	dd	?
endl             	
	mov ebx, [pointerA]
	mov eax, [ebx]
        mov [firstElemL], eax

	mov eax, [sizeA]
	dec eax
	mul dword[typeSize]
	
	add ebx, eax
	mov eax, [ebx]
	mov [lastElemL], eax


	stdcall CalcSizeB, [pointerA], [sizeA]
	mov [sizeB], eax

	mul dword[typeSize]
	
	cinvoke malloc, eax
	mov [pointerB], eax

	mov ebx, [pointerA]
	mov edx, [pointerB]	

	mov ecx, [sizeA]

.forLoop:
	push ecx
	
	mov eax, [ebx]

	cmp eax, [firstElemL]
	je @f

	cmp eax, [lastElemL]
	je @f

	mov [edx], eax
	
	xor eax, eax
	mov al, [typeSize]
	add edx, eax
		                
@@:
	xor eax, eax
	mov al, [typeSize]
	add ebx, eax

	pop ecx
	loop .forLoop 

	mov eax, [pointerB]
	mov ebx, [sizeB]
	 
	ret
endp

proc readArray uses ecx ebx, arraySize
	xor eax, eax
	mov eax, [arraySize]
	test eax, 80000000h
	jnz .error
	test eax, eax
	jz .error

	mul DWORD [typeSize]

        cinvoke malloc, eax
        mov [arrayA_ptr], eax
	
	mov ebx, eax
	mov ecx, [arraySize]

.forLoop:
	push ecx
	mov eax, [arraySize]
	sub eax, ecx
	
 	cinvoke printf, HelloWorld, eax 	
	cinvoke scanf, ioDigitTemp, curNum
	test eax, eax
	jz .error

	mov eax, [curNum]
	mov [ebx], eax

	xor eax, eax
	mov al, [typeSize]
	add ebx, eax

	pop ecx
        loop .forLoop

	
	mov eax, [arrayA_ptr]
	jmp .endProc

.error:
	xor eax, eax

.endProc:
	ret
endp

proc printArray uses ecx ebx, pointer, arraySize
        cinvoke printf, output
	cinvoke printf, arrayStart


	mov ecx, [arraySize]
	mov ebx, [pointer]
	mov [first], 1

	cmp ecx, 0
	je .endProc

.forLoop:
	push ecx

	cmp [first], 0
	jne @f
	
	cinvoke printf, separator
@@:	
	cinvoke printf, ioDigitTemp, [ebx]
	mov [first], 0

	xor eax, eax
	mov al, [typeSize]
	add ebx, eax
	

	pop ecx
	loop .forLoop

.endProc:	
	cinvoke printf, arrayEnd

	ret;
endp

proc CalcSizeB uses ecx ebx, pointerA, SizeA
locals
    	firstElemL	dd	?
	lastElemL	dd	?
endl
	mov ebx, [pointerA]
	mov eax, [ebx]
        mov [firstElemL], eax

	mov eax, [SizeA]
	dec eax
	mul dword[typeSize]
	
	add ebx, eax
	mov eax, [ebx]
	mov [lastElemL], eax

	mov [sizeB], 0
	mov ecx, [SizeA]
	mov ebx, [pointerA]
	
.forLoop:
	push ecx
	
	mov eax, [ebx]

	cmp eax, [firstElemL]
	je @f

	cmp eax, [lastElemL]
	je @f

	inc [sizeB]
		
@@:
	xor eax, eax
	mov al, [typeSize]
	add ebx, eax

	pop ecx
	loop .forLoop 
	
	mov eax, [sizeB]

	ret
endp


section         '.data' data readable writeable
 	input 			db	'Enter size of array : ',0
	output 			db 	'Entered array : ',0
	separator		db	', ',0
        ioDigitTemp         	db      '%d',0
	outStringTemp		db 	'%s',0
	HelloWorld		db	'Enter %d number : ',0
	Entered			db 	'You enter %d',13,10,0
	FilteredArraySize	db	'Filtered array size : %d',13,10,0
        cr                      db      13,10,0
	arrayStart		db	'{ ',0
	arrayEnd		db	' }',13,10,0
	readError		db 	'Read Error ',13,10,0

        N                       dd      ?
        arrayA_ptr              dd      ?
	arrayB_ptr		dd	?
        temp                    db      ?
        curNum			dd	?
	first			db	?
	pointer			dd 	?
	arraySize		dd	?
	
	sizeB			dd	?
	
	firstElem		dd	?
	lastElem		dd	?
	
	typeSize		db	4





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
