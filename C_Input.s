%include "io.mac"
section .bss
N1 resd 1
section .text
global _start
_start:
push N1
call C_INPUT
PutCh [N1]
mov eax,1
mov ebx,0
int 80h
C_INPUT: push EBP ;push variavel antes (talvez mais um registrador). Funcao ainda nao esta pronta
		   mov EBP,ESP
		   push eax
		   push ebx
		   push ecx
		   push edx
		   mov eax,3
		   mov ebx,0
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

