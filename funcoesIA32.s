section .bss
String: resd 100

section .data
tam: dd 100
nwln: dd 13,10
tam2: dd 2

section .text
    global _main
_main:
    push tam
    push String
    call LeerString

    push tam
    push String
    call EscreverString

    push tam2
    push nwln
    call EscreverString

    mov eax,1
    mov ebx,0
    int 80h

LeerString:
    enter 2,0
    mov word [ebp-2],0
    mov edi,[ebp+8]
    mov esi,[ebp+12]

leitura_string:
    mov eax,3
    mov ebx,0
    mov ecx,edi
    mov edx,1
    int 80h
    cmp byte [edi],10 ; compara com /n
    je fim_leitura_string
    inc word [ebp-2]
    add edi,4
    dec esi
    cmp esi,0
    jne leitura_string

fim_leitura_string:
    mov ax,[ebp-2]
    leave
    ret 8


EscreverString:
    enter 2,0
    mov word [ebp-2],0
    mov edi,[ebp+8]
    mov esi,[ebp+12]

escrita_string:
    mov eax,4
    mov ebx,1
    mov ecx,edi
    mov edx,1
    int 80h
    inc word [ebp-2]
    add edi,4
    dec esi
    cmp esi,0
    jne escrita_string

fim_escrita_string:
    mov ax,[ebp-2]
    leave
    ret 8
