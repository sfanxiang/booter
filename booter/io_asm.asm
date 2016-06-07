ifndef X64
.386
.model flat, c
endif

.data
public _io_port_in, _io_data_in
_io_port_in dw 0
_io_data_in db 0

.code

inb_asm proc
	mov dx, [_io_port_in]
	in al, dx
	ret
inb_asm endp

outb_asm proc
	mov dx, [_io_port_in]
	mov al, [_io_data_in]
	out dx, al
	ret
outb_asm endp

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
