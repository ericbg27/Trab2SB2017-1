%include "io.mac"
section .data
msg1 db "Por favor, digite novamente.",0ah
section .bss
N1 resd 1
section .text
global _start
_start:
push N1
call LeerChar
mov eax,1
mov ebx,0
int 80h
LeerChar: enter 2,0
		   push eax
		   push ebx
		   push ecx
		   push edx
		   InputC: mov eax,3
		           mov ebx,0
		           mov ecx,EBP
                 sub ecx,2
		           mov edx,2
		           int 80h
         cmp byte [EBP-2],0ah
         je Espaco
         mov CL,[EBP-2]
         mov edx,[EBP+8]
         mov [edx],CL
         jmp FIM_C
         Espaco: mov eax,4
                 mov ebx,1
                 mov ecx,msg1
                 mov edx,29
                 int 80h
                 jmp InputC
         FIM_C: pop edx
                pop ecx
                pop ebx
                pop eax
                leave
                ret 4      