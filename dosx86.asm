begin proc near
	mov ax, -4096
	push ax
	mov ax, sp
	pop bx
	add ax, bx
	mov sp, ax
	mov ax, 1
	push ax
	mov ax, 12
	push ax
	mov ax, sp
	pop bx
	add ax, bx
	mov bx, ax
	mov ax, [bx]
	pop bx
	mov [ax], bx
	mov ax, 3
	push ax
	mov ax, 20
	push ax
	mov ax, sp
	pop bx
	add ax, bx
	mov bx, ax
	mov ax, [bx]
	pop bx
	mov [ax], bx
	mov ax, 2
	push ax
	mov ax, 16
	push ax
	mov ax, sp
	pop bx
	add ax, bx
	mov bx, ax
	mov ax, [bx]
	pop bx
	mov [ax], bx
	mov ax, 1
	push ax
	mov ax, 0
	push ax
	mov ax, sp
	pop bx
	add ax, bx
	mov bx, ax
	mov ax, [bx]
	pop bx
	mov [ax], bx
	mov ax, 2
	push ax
	mov ax, 4
	push ax
	mov ax, sp
	pop bx
	add ax, bx
	mov bx, ax
	mov ax, [bx]
	pop bx
	mov [ax], bx
	mov ax, 3
	push ax
	mov ax, 12
	push ax
	mov ax, sp
	pop bx
	add ax, bx
	mov bx, ax
	mov ax, [bx]
	pop bx
	mov [ax], bx
	mov ax, 4
	push ax
	mov ax, 16
	push ax
	mov ax, sp
	pop bx
	add ax, bx
	mov bx, ax
	mov ax, [bx]
	pop bx
	mov [ax], bx
	mov ax, 5
	push ax
	mov ax, 20
	push ax
	mov ax, sp
	pop bx
	add ax, bx
	mov bx, ax
	mov ax, [bx]
	pop bx
	mov [ax], bx
	mov ax, 4096
	push ax
	mov ax, sp
	pop bx
	add ax, bx
	mov sp, ax
	ret
begin endp
