public crehelper_fill_xmmregs

.code

crehelper_fill_xmmregs proc

	; rcx -> aligned ptr to 256 bytes
	movdqa xmm0, xmmword ptr [rcx]
	movdqa xmm1, xmmword ptr [rcx + 16]
	movdqa xmm2, xmmword ptr [rcx + 32]
	movdqa xmm3, xmmword ptr [rcx + 48]
	movdqa xmm4, xmmword ptr [rcx + 64]
	movdqa xmm5, xmmword ptr [rcx + 80]
	movdqa xmm6, xmmword ptr [rcx + 96]
	movdqa xmm7, xmmword ptr [rcx + 112]

	add rcx, 128
	movdqa xmm8, xmmword ptr [rcx]
	movdqa xmm9, xmmword ptr [rcx + 16]
	movdqa xmm10, xmmword ptr [rcx + 32]
	movdqa xmm11, xmmword ptr [rcx + 48]
	movdqa xmm12, xmmword ptr [rcx + 64]
	movdqa xmm13, xmmword ptr [rcx + 80]
	movdqa xmm14, xmmword ptr [rcx + 96]
	movdqa xmm15, xmmword ptr [rcx + 112]

	ret

crehelper_fill_xmmregs endp

end
