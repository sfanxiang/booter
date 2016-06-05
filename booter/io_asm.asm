ifndef X64
.386
.model flat, c
endif

.code

reboot proc
	; cli
_reboot_test:
	in al, 64h
	test al, 2
	jnz _reboot_test
	mov al, 0feh
	out 64h, al
	ret
reboot endp

cmos_set_warm_reset proc
	; cli
	in al, 70h
	and al, 80h
	or al, 0fh
	out 70h, al
	mov al, 0ah
	out 71h, al
	; sti
	ret
cmos_set_warm_reset endp

end
