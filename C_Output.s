%include "io.mac"
section .bss
N1 resd 1
section .text
global _start
_start:
mov byte [N1],0x2D
push N1
call EscreverChar
mov eax,1
mov ebx,0
int 80h
EscreverChar:  push EBP
		   mov EBP,ESP
		   push eax
		   push ebx
		   push ecx
		   push edx
		   mov eax,4
		   mov ebx,1
		   mov ecx,[EBP+8]
		   mov edx,1
		   int 80h
           pop edx
           pop ecx
           pop ebx
           pop eax
           mov ESP,EBP
           pop EBP
           ret 4         