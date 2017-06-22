;**********************************Verificar para o 1*****************************************
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
push N1
call Escreve_Inteiro
mov eax,1
mov ebx,0
int 80h
Leer_Inteiro: enter 20,0 
              push eax
              push ebx
              push ecx
              push edx
              push esi
              push edi
              mov esi,0 ;contador
              mov word [EBP-20],0 ;Zerando flag
              mov eax,3
              mov ebx,0
              mov ecx,EBP
              sub ecx,18
              mov edx,12
              int 80h
              mov ecx,eax
              sub ecx,1 
              mov eax,0 ;Zerando acumulador
              mov edi,10
              mov ebx,0
              cmp byte [EBP-18],0x2D
              je Negativo
              Convert_Int: cmp byte [EBP-18+esi],0x30
                           jl Invalido
                           cmp byte [EBP-18+esi],0x39
                           jg Invalido
                           mov bl,[EBP-18+esi]
                           cmp esi,0
                           je First_Digit
                           cmp word [EBP-20],1
                           je Verif_Digit
                           Mult: mul edi
                           First_Digit: add eax,ebx
                                        sub eax,0x30
                                        inc esi
                           loop Convert_Int
                           jmp Armazena_Num
                           Verif_Digit: cmp esi,1
                                        je First_Digit
                                        jmp Mult
              Negativo: inc esi
                        dec ecx
                        mov word [EBP-20],1 ;flag
                        jmp Convert_Int
              Invalido: mov eax,4
                        mov ebx,1
                        mov ecx,msg0
                        mov edx,17
                        int 80h
                        jmp FIM
              Armazena_Num: cmp word [EBP-20],1
                            je Nega_Num
                            mov ebx,[EBP+8]
                            mov dword [ebx],eax ;Armazenando numero convertido na memoria
                            jmp FIM
              Nega_Num: neg eax
                        mov word [EBP-20],0
                        jmp Armazena_Num
              FIM: pop edi
                   pop esi
                   pop edx
                   pop ecx
                   pop ebx
                   pop eax
                   leave
                   ret 4

Escreve_Inteiro: enter 12,0 ;Escreve_Inteiro apenas para teste
                 push eax
                 push ebx
                 push ecx
                 push edx
                 push esi
                 push edi
                 mov ebx,0 ;Flag
                 mov esi,0 ;contador
                 mov ecx,10
                 mov edi,[EBP+8]
                 mov eax,[edi]
                 cmp eax,0
                 je Zero
                 cmp dword [edi],0 ;Verificando se o numero eh negativo
                 jl Neg
         Positivo: cdq 
               cmp eax,0
               je Imprime
               div ecx
               add edx,0x30 ;(Valor%10) + 0x30
               cmp edx,0x30
               jl Numero_Inv
               cmp edx,0x39
               jg Numero_Inv
               mov [EBP-12+esi],edx
               inc esi
               jmp Positivo
         Neg: mov ebx,1
               neg eax
               Neg_Loop: cdq
                     cmp eax,0
                     je Imprime
                     div ecx
                     add edx,0x30 ;(Valor%10) + 0x30
                     cmp edx,0x30
                     jl Numero_Inv
                     cmp edx,0x39
                     jg Numero_Inv
                     mov [EBP-12+esi],dl
                     inc esi
                     jmp Neg_Loop
         Zero: add eax,0x30
               mov [EBP-12+esi],eax
               inc esi
               jmp Imprime
         Numero_Inv: mov eax,4
                     mov ebx,1
                     mov ecx,msg0
                     mov edx,17
                     int 80h
                     jmp Exit_Func
         Imprime: cmp ebx,1
                  je insert_sign
                  mov byte [EBP-12+esi],0ah
                  mov edi,esi
                  mov eax,12
                  sub eax,esi
                  mov edi,eax
                  mov esi,eax
                  inc esi
                  Sys_Write: mov eax,4
                             mov ebx,1 
                             mov ecx,EBP
                             sub ecx,esi
                             mov edx,1
                             int 80h
                             inc esi
                             cmp esi,13
                             jne Sys_Write
                             mov eax,4
                             mov ebx,1
                             mov ecx,EBP
                             sub ecx,edi
                             mov edx,1
                             int 80h
                             jmp Exit_Func
                 insert_sign: mov byte [EBP-12+esi],0x2D
                              inc esi
                              mov ebx,0
                              jmp Imprime
                 Exit_Func: pop edi
                            pop esi
                            pop edx
                            pop ecx
                            pop ebx
                            pop eax
                            leave
                            ret 4