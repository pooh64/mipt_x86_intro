.model tiny
.data
	buf_len = 30d
	buf    DB buf_len DUP (0)
buf_end:

	buffin DB 10, "Well done$", 10, "$" 
buffin_end:
 
.code
org 100h
	buf_len_wrong = 0ffh
	psw_hash = 1011h
	code_hash = 0c48h
;============================
;============================
start:
; password = tooezforroma, hash = psw_hash 
	
;	codemodding example:
;	mov bx, offset patchtest
;	mov ax, 0b8h
;	mov [bx], al
;	patchtest: 	mov dx, 0ffh
	

	; check hashsum of codeseg
	; just print heart if patched :)
	; or set patch_1 to "mov [bx], ax" and corrupt file :D  
	call check_patch

	; read psw with wrong bufsize (just for exploit)
	mov ah, 0ah		
	mov dx, offset buf
	mov [buf], buf_len_wrong
	int 21h

	; protection from jmp through check
	mov bx, offset patch_2
	mov dx, 0b8h
	mov [bx], dl
	
	; check hash of psw and jmp to exit if hashsum is true
	xor cx, cx
	mov bx, offset buf + 1
	mov cl, [bx]
	inc bx
	add cx, bx
	call hash
	cmp ax, psw_hash
	je exit

	; terminate if wrong psw
	mov ax, 0800h
	int 21h
 
;============================
; here we generate hashsum in ax
; bx - start of buf, cx - end of buf	
; bx corrupted

hash:
	xor ax, ax
hash_c:
patch_1:
	xor ax, [bx]
	add bx, 2
	cmp cx, bx
	ja hash_c

	ret
	
;============================
;============================
patch_detected:
	mov ah, 02h
	mov dl, 0ah
	int 21h
	mov dl, 03h
	int 21h	
	ret 

;============================
exit:	
	push cs
	mov ax, 4c00h
	pop ds
	mov dx, offset buffin
patch_2:	mov dx, 0900h
	int 21h	

	mov ax, 0800h
	int 21h
	mov ax, 4c00h
	int 21h	
		
start_end:

;============================
check_patch:

	mov bx, offset start
	mov cx, offset start_end
	call hash
	cmp ax, code_hash
	je check_patch_end
	
	; ultradefence
	; mod patch_1 -> mov [bx], ax
	; call patched hash from start
	mov bx, offset patch_1
	mov dx, 8817h	
	mov [bx], dx
	call check_patch
	
	call patch_detected

check_patch_end:
	ret
	
end start
