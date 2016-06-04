ifndef X64
.386
.model flat, c
endif

reboot proc
	cli
	mov dx, 64h
_reboot_test;
	in al, dx
	test al, 2
	jnz _reboot_test
	mov al, 0feh
	out dx, al
	ret
reboot endp

end
