public atomic_lock
public atomic_unlock
public atomic_increment


; assumes threads aren't using the same rsp...

.code

;------------------------------------------------------------------------------

atomic_lock proc

	xor eax, eax
	lock cmpxchg qword ptr [rcx], rsp
	jne atomic_lock_blocked
	dec eax
	ret

atomic_lock_blocked:
	ret

atomic_lock endp

;------------------------------------------------------------------------------

atomic_unlock proc

	mov rax, qword ptr [rcx]
	lock xor qword ptr [rcx], rax
	ret

atomic_unlock endp

;------------------------------------------------------------------------------

atomic_increment proc

	lock inc qword ptr [rcx]
	ret

atomic_increment endp

;------------------------------------------------------------------------------


end
