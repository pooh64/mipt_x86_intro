BITS 64

	push rax
	push rdi
	push rsi
	push rdx
	push rcx
	
	mov rax, [rsp + 8 + 5 * 8]
	sub rsp, 8
	mov rsi, rsp
	mov rsi, rsp
	mov byte [rsi], 0ah
	mov rdi, 10
	
	; positive -> rcx = 0
	xor rcx, rcx
	cmp rax, 0
	jge cycle
	
	; abs(rax) and rcx = 0 - rax != 0
	sub rcx, rax
	mov rax, rcx
	
cycle:
	dec rsi
	xor rdx, rdx
	idiv rdi
	add dl, '0'
	mov byte [rsi], dl
	cmp rax, 0
	jg cycle
	
	
	cmp rcx, 0
	je positive
	
	dec rsi
	mov byte [rsi], '-'
	
positive:
	
	mov rax, 1
	mov rdi, 1
	mov rdx, rsp
	sub rdx, rsi
	inc rdx
	
	syscall
	
	add rsp, 8
	
	pop rcx
	pop rdx
	pop rsi
	pop rdi
	pop rax

	ret
	
	
