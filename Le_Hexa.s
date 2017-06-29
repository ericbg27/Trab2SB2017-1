%include "io.mac"
section .data
msg1 db "Numero Invalido! Digite Novamente.",0ah
msg0 db "Numero Invalido!",0ah
section .bss
N1 resd 1

section .text
global _start
_start:
push N1
call LerHexa
PutInt [N1]
push dword [N1]
call EscreverHexa
mov eax,1
mov ebx,0
int 80h
LerHexa: enter 17,0
         push eax
         push ebx
         push ecx
         push edx
         push esi
         push edi
         mov dword [EBP-13],0
         Input_HEX: mov eax,3
                    mov ebx,0
                    mov ecx,EBP
                    sub ecx,9
                    mov edx,9
                    int 80h
         cmp byte [EBP-9],0x2D
         je Invalido_HEX
         mov edi,eax
         sub edi,2
         mov esi,0
         xor edx,edx
         Verifica_Char: cmp byte [EBP-9+esi],0x30
                        jl Invalido_HEX
                        cmp byte [EBP-9+esi],0x39
                        jg Verifica_Char_HEX
                        xor ebx,ebx
                        mov BL,[EBP-9+esi]
                        sub ebx,0x30
                        jmp Convert_to_Num
         Verifica_Char_HEX: cmp byte [EBP-9+esi],0x41
                            jl Invalido_HEX
                            cmp byte [EBP-9+esi],0x46
                            jg Invalido_HEX
                            xor ebx,ebx
                            mov BL,[EBP-9+esi]
                            sub ebx,0x41
                            add ebx,10
                            jmp Convert_to_Num
         Convert_to_Num: cmp edi,0
                         je Last_Digit
                         mov ecx,edi
                         mov eax,1
                         mov [EBP-17],ebx
                         mov ebx,16
                         Loop_Convert: mul ebx
                                       loop Loop_Convert
                         mov ebx,[EBP-17]
                         dec edi
                         mul ebx
                         add [EBP-13],eax
                         inc esi
                         jmp Verifica_Char  
                         Last_Digit: add [EBP-13],ebx
                         jmp FIM_HEX
         Invalido_HEX: mov eax,4
                       mov ebx,1
                       mov ecx,msg1
                       mov edx,35
                       int 80h
                       jmp Input_HEX
         FIM_HEX: mov eax,[EBP-13]
                  mov ebx,[EBP+8]
                  mov [ebx],eax
                  pop edi
                  pop esi
                  pop edx
                  pop ecx
                  pop ebx
                  pop edx
                  leave
                  ret 4
EscreverHexa: enter 9,0 ;Dando errado, verificar
              push eax
              push ebx
              push ecx
              push edx
              push esi
              push edi
              mov eax,[EBP+8]
              mov edi,-1
              xor esi,esi
              mov ebx,16
              cmp eax,0
              jl Inv_HEX
              Div_Loop: cdq
                        div ebx
                        mov [EBP+edi],edx
                        inc esi
                        dec edi
                        cmp eax,0
                        je Print_HEX
                        jmp Div_Loop
              Inv_HEX: mov eax,4
                       mov ebx,1
                       mov ecx,msg0
                       mov edx,17
                       int 80h
                       jmp F_HEX
             Print_HEX: mov byte [EBP+edi],0ah
                        inc esi
                        dec edi
                        mov eax,4
                        mov ebx,1
                        mov ecx,EBP
                        add ecx,edi
                        mov edx,esi
                        int 80h
             F_HEX: pop edi
                    pop esi
                    pop edx
                    pop ecx
                    pop ebx
                    pop eax
                    leave
                    ret 4

