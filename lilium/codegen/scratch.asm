.code

scratch_asm proc

	push rsi
	push rdi
	push rbx
	mov rsi, rcx
	mov rdi, rdx
	mov rcx, r8
	mov rax, rcx
	mov rdx, rcx
	add rax, rsi
	add rdx, rdi
	
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3

	movdqa xmm0, xmmword ptr [rsi]
	add rsi, 16
	cmp rsi, rax
	jge scratch_exit
	movdqa xmm7, xmmword ptr [rsi]
	add rsi, 16
	cmp rsi, rax
	jge scratch_exit
	movdqa xmm8, xmmword ptr [rsi]
	add rsi, 16
	cmp rsi, rax
	jge scratch_exit
	movdqa xmm15, xmmword ptr [rsi]
	add rsi, 16
	cmp rsi, rax
	jge scratch_exit






scratch_exit:
	ret

scratch_asm endp

end