%include "io.mac"
section .data
msg1 db "Numero Invalido! Digite Novamente.",0ah
msg0 db "Numero Invalido!",0ah
section .bss
N1 resd 4

section .text
global _start
_start:
push DWORD 17 ;Tem que ser tamanho da string +1
push N1
call LeerString
push DWORD 17 ;Aqui tbm
push N1
call EscreverString
mov eax,1
mov ebx,0
int 80h
LeerString: enter 4,0
			push ebx
			push ecx
			push edx
			push esi
			push edi
			mov edi,[EBP+8]
			mov DWORD [EBP-4],0
			mov esi,[EBP+12]
			Leitura: mov eax,3
					 mov ebx,0
					 mov ecx,edi
					 mov edx,1
					 int 80h
					 cmp byte [edi],0ah
					 je Fim_Leitura
					 add edi,1
					 inc DWORD [EBP-4]
					 cmp [EBP-4],esi
					 je Fim_Leitura
					 jmp Leitura
			Fim_Leitura: mov eax,[EBP-4]
						 pop edi
						 pop esi
						 pop edx
						 pop ecx
						 pop ebx
						 leave
						 ret 8
EscreverString: enter 4,0
				push ebx
				push ecx
				push edx
				push esi
				push edi
				mov edi,[EBP+8]
				mov DWORD [EBP-4],0
				mov esi,[EBP+12]
				Escrita: mov eax,4
						 mov ebx,0
						 mov ecx,edi
						 mov edx,1
						 int 80h
						 cmp byte [edi],0ah
						 je Fim_Escrita
						 add edi,1
						 inc DWORD [EBP-4]
						 cmp [EBP-4],esi
						 je Fim_Escrita
						 jmp Escrita
				Fim_Escrita: mov eax,[EBP-4]
							 pop edi
							 pop esi
							 pop edx
							 pop ecx
							 pop ebx
							 leave
							 ret 8