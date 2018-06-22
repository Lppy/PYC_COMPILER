.model small

.data
RET_VAL dw 0

.stack 32768

.code
begin proc near
	mov ax, @data
	mov ds, ax
	call main

	mov ax, RET_VAL
	mov bl, 10
	mov cx, 10
	mov bh, 0
lo1:
	div bl
	mov dl, ah
	add dl, 30h
	push dx
	inc bh
	and ax, 00ffh
	loopnz lo1
	mov cl, bh
	mov ah, 02h
lo2:
	pop dx
	int 21h
	loop lo2
mov ah, 4ch
	int 21h
begin endp

main: 
	mov ax, -4096
	push ax
	mov ax, RET_VAL
	pop bx
	add ax, bx
	mov RET_VAL, ax
	mov ax, 1
	mov sp, ax
	mov ax, 4096
	push ax
	mov ax, RET_VAL
	pop bx
	add ax, bx
	mov RET_VAL, ax
	ret
 endp

end begin

