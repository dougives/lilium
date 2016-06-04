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


	movdqa xmm0, xmmword ptr [rsi]
	movdqa xmm1, xmmword ptr [rsi]
	movdqa xmm2, xmmword ptr [rsi]
	movdqa xmm3, xmmword ptr [rsi]
	movdqa xmm4, xmmword ptr [rsi]
	movdqa xmm5, xmmword ptr [rsi]
	movdqa xmm6, xmmword ptr [rsi]
	movdqa xmm7, xmmword ptr [rsi]


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
	jl label1
	ret
	label1:

	movdqa xmm7, xmmword ptr [rsi]
	add rsi, 16
	cmp rsi, rax
	jl label2
	ret
	label2:

	movdqa xmm8, xmmword ptr [rsi]
	add rsi, 16
	cmp rsi, rax
	jl label3
	ret
	label3:

	movdqa xmm15, xmmword ptr [rsi]
	add rsi, 16
	cmp rsi, rax
	jl label4
	ret
	label4:






scratch_exit:
	ret

scratch_asm endp

end