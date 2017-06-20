%include "io.mac"
section .data
msg0 db "Numero Invalido!",0ah
section .bss
N1 resd 1

section .text
global _start
_start:
push N1
call Leer_Inteiro
PutInt [N1]
PutCh 0ah
mov eax,1
mov ebx,0
int 80h
Leer_Inteiro: enter 14,0
              push eax
              push ebx
              push ecx
              push edx
              push esi
              push edi
              mov esi,0 ;contador
              mov word [EBP-14],0 ;Zerando flag
              mov eax,3
              mov ebx,0
              mov ecx,EBP
              sub ecx,12
              mov edx,6
              int 80h
              mov ecx,eax
              sub ecx,1 ;ecx = numero de caracteres - 1
              mov eax,0 ;Zerando acumulador
              mov edi,10
              mov ebx,0
              cmp byte [EBP-12],0x2D
              je Negativo
              Convert_Int: cmp byte [EBP-12+esi],0x30
                           jl Invalido
                           cmp byte [EBP-12+esi],0x39
                           jg Invalido
                           cmp esi,0
                           mov bl,[EBP-12+esi]
                           mul edi
                           add eax,ebx
                           sub eax,0x30
                           inc esi
                           loop Convert_Int
              jmp Armazena_Num
              Negativo: inc esi
                        dec ecx
                        mov word [EBP-14],1 ;flag
                        jmp Convert_Int
              Invalido: mov eax,4
                        mov ebx,1
                        mov ecx,msg0
                        mov edx,17
                        int 80h
                        jmp FIM
              Armazena_Num: cmp word [EBP-14],1
                            je Nega_Num
                            mov ebx,[EBP+8]
                            mov [ebx],eax ;Armazenando numero convertido na memoria
                            PutInt AX
                            PutCh 0ah
                            jmp FIM
              Nega_Num: neg eax
                        mov word [EBP-14],0
                        jmp Armazena_Num
              FIM: pop edi
                   pop esi
                   pop edx
                   pop ecx
                   pop ebx
                   pop eax
                   leave
                   ret 4