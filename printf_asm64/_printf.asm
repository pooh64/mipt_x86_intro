global  _printf

section .data

; _printf function reqires buffer

BUFLEN 		equ 	8
BUF:		times BUFLEN db 0
BUFEND		equ	BUF + BUFLEN
CHAR_NEWLINE:   db      10
CHAR_HORTAB:	db	09

section .text

_printf:

	pop r10		;save retaddr
	
	push r9
	push r8
	push rcx
	push rdx
	push rsi
	
	mov rsi, rdi
	call printf
	
	pop rax
	pop rax
	pop rax
	pop rax
	pop rax
	
	push r10	
	mov eax, 0
	ret

;================================================
;	Proc printf
; Input:
;	stack: argn, ... , arg1
;	Offset of str in rsi
; Uses:
;	rsi, rax, rdx, rdi
;================================================
printf:
	push rbp
	mov rbp, rsp
	add rbp, 8 		; 2nd +8 will do on first %

.cycle:

	cmp byte [rsi], '%'
	je .formats

	cmp byte [rsi], '\'
	je .spec_symb

	call print_char
	jmp .next


.formats:

	add rbp, 8 		; getting arg
	inc rsi
	call clean_buf

	cmp byte [rsi], 's'
	je .str

	cmp byte [rsi], 'c'
	je .chr

	cmp byte [rsi], 'd'
	je .dec

	cmp byte [rsi], 'b'
	je .bin

	cmp byte [rsi], 'o'
	je .oct

	cmp byte [rsi], 'x'
	je .hex

	; error here

.spec_symb:

	inc rsi

	cmp byte [rsi], 'n'
	je .newline

	cmp byte [rsi], 't'
	je .hortab

	;error here

.next:
	inc rsi
	cmp byte [rsi], 0
	jne .cycle

	pop rbp
	ret

;================================================
;
; Here are the calls of functions
;
;================================================
.cur_char:			; print char
	call print_char
	jmp .next

.str:				; print str
	push rsi
	mov rsi, [rbp]
	call print_str
	pop rsi
	jmp .next

.chr:
	push rsi
	mov rsi, rbp
	call print_char
	pop rsi
	jmp .next

.dec:
	push rsi
	xor rax, rax
	xor rdx, rdx
	mov eax, dword [rbp]
	mov edx, dword [rbp + 4]
	call print_dec
	pop rsi
	jmp .next

.bin:
	push rsi
	mov rax, qword [rbp]
	call print_bin
	pop rsi
	jmp .next

.oct:
	push rsi
	mov rax, qword [rbp]
	call print_oct
	pop rsi
	jmp .next

.hex:
	push rsi
	mov rax, qword [rbp]
	call print_hex
	pop rsi
	jmp .next

;================================================
;
; 	Special symb print
;
;================================================

.newline:
	push rsi
	mov rsi, CHAR_NEWLINE
	call print_char
	pop rsi
	jmp .next

.hortab:
	push rsi
	mov rsi, CHAR_HORTAB
	call print_char
	pop rsi
	jmp .next

;================================================
; 	Decimal print
; input: value in edx:eax
; uses rsi, rax, rdx, rdi
;================================================
print_dec:
	mov rsi, BUFEND
	mov rdi, 10

.cycle:			; eax - result, edx = %10
	div edi

	dec rsi
	add dl, '0'
	mov byte [rsi], dl

	cmp eax, 0
	je .end

	xor edx, edx
	jmp .cycle

.end:
	call print_buf
	ret

;================================================
;	Binary print
; input: value in rax
; uses rsi, rax, rdx
;================================================
print_bin:
	mov rsi, BUFEND

.cycle:			; rax - result, rdx = %2
	mov rdx, rax
	and rdx, 1
	shr rax, 1

	dec rsi
	add dl, '0'
	mov byte [rsi], dl

	cmp rax, 0
	je .end

	jmp .cycle

.end:
	call print_buf
	ret

;================================================
;	Octal print
; input: value in rax
; uses rsi, rax, rdx
;================================================
print_oct:
	mov rsi, BUFEND

.cycle:			; rax - result, rdx = %8
	mov rdx, rax
	and rdx, 111b
	shr rax, 3

	dec rsi
	add dl, '0'
	mov byte [rsi], dl

	cmp rax, 0
	je .end

	jmp .cycle

.end:
	call print_buf
	ret

;================================================
;	Hexadecimal print
; input: value in rax
; uses rsi, rax, rdx
;================================================
print_hex:
	mov rsi, BUFEND

.cycle:			; rax - result, rdx = %16
	mov rdx, rax
	and rdx, 1111b
	shr rax, 4

	dec rsi

	cmp dl, 10
	jb .number
	add dl, 'a' - 10
	jmp .char_complete

.number:
	add dl, '0'

.char_complete:

	mov byte [rsi], dl

	cmp rax, 0
	je .end

	jmp .cycle

.end:
	call print_buf
	ret

;================================================
; 	Buffer clean
; uses: rdx
;================================================
clean_buf:
	mov rdx, BUF

.cycle:
	mov byte [rdx], 0
	inc rdx
	cmp rdx, BUFEND
	jne .cycle

	ret

;================================================
;	One char print
; input: char offset in rsi
; uses rax, rdi, rdx
;================================================
print_char:
	mov     rax, 1                  ; system call 1 is write
        mov     rdi, 1                  ; file handle 1 is stdout
        mov     rdx, 1                  ; number of bytes
        syscall
	ret

;================================================
; 	Buffer print
; uses rax, rdi, rdx, rsi
;================================================
print_buf:
	mov     rax, 1                  ; system call 1 is write
        mov     rdi, 1                  ; file handle 1 is stdout
        mov     rdx, BUFLEN             ; number of bytes
	mov 	rsi, BUF
        syscall
	ret

;================================================
; 	Str print
; input: str offset in rsi
; uses rax, rdx, rdi, rsi
;================================================
print_str:
	mov     rax, 1                  ; system call 1 is write
        mov     rdi, 1                  ; file handle 1 is stdout
	mov     rdx, 1			; number of bytes
.cycle:
        syscall
	inc rsi
	cmp byte [rsi], 0
	jne .cycle

	ret
