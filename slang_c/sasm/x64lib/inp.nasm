BITS 64

	push rbx
	push rcx
	push rsi
	push rdi
	push rdx
	sub rsp, 32

	; sys_read
	mov rax, 0
reread:
	mov rdi, 0
	mov rsi, rsp
	; maximum lenght input "-" + "9223372036854775807" (19 symb) + "\n"
	mov rdx, 21
	syscall

	; number of readed bytes dec in rcx
	mov rcx, rax
	sub rcx, 1

	; positive -> rdx != 0, negative -> rdx = 0
	cmp byte [rsi], '-'
	jne positive
	xor rdx, rdx
	inc rsi
	dec rcx
positive:
	mov rdi, 10
	xor rax, rax
	xor rbx, rbx

	; "", "-" input
	cmp rcx, 0
	jbe reread

label: 
	imul rax, rdi
	mov bl, byte [rsi]
	sub bl, '0'
	add rax, rbx
	inc rsi
	loop label

	; if positive jmp end
	cmp rdx, 0
	jne end
	sub rdx, rax
	mov rax, rdx

end:
	add rsp, 32
	pop rdx
	pop rdi
	pop rsi
	pop rcx
	pop rbx

	ret