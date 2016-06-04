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


	movdqa xmmword ptr [rdi], xmm0
	movdqa xmmword ptr [rdi], xmm1
	movdqa xmmword ptr [rdi], xmm2
	movdqa xmmword ptr [rdi], xmm3
	movdqa xmmword ptr [rdi], xmm4
	movdqa xmmword ptr [rdi], xmm5
	movdqa xmmword ptr [rdi], xmm6
	movdqa xmmword ptr [rdi], xmm7
	movdqa xmmword ptr [rdi], xmm8
	movdqa xmmword ptr [rdi], xmm9
	movdqa xmmword ptr [rdi], xmm10
	movdqa xmmword ptr [rdi], xmm11
	movdqa xmmword ptr [rdi], xmm12
	movdqa xmmword ptr [rdi], xmm13
	movdqa xmmword ptr [rdi], xmm14
	movdqa xmmword ptr [rdi], xmm15

	int 3
	int 3
	int 3
	int 3
	int 3
	int 3
	int 3


	cmp rdi, rdx
	jl label9
	ret
	label9:
	movdqa xmmword ptr [rdi], xmm0
	add rdi, 16


	cmp rdi, rdx
	jl label8
	ret
	label8:
	movdqa xmmword ptr [rdi], xmm8
	add rdi, 16



	int 3
	int 3
	int 3
	int 3	
	int 3
	int 3
	int 3

	cmp rsi, rax
	jge label1
	movdqa xmm0, xmmword ptr [rsi]
	add rsi, 16
	label1:

	cmp rsi, rax
	jge label2
	movdqa xmm7, xmmword ptr [rsi]
	add rsi, 16
	label2:


scratch_exit:
	ret

scratch_asm endp

end