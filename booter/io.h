#ifndef BOOTER_IO_H
#define BOOTER_IO_H

#include <ntddk.h>
#include <ntstrsafe.h>
#include <wdf.h>

extern unsigned short _io_port_in;
extern unsigned char _io_data_in;

unsigned char __cdecl inb_asm();
void __cdecl outb_asm();
void __cdecl reboot();
void __cdecl cmos_set_warm_reset();

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char data);

#define BIOS_WARM_RESET_VECTOR ((size_t)0x467L)
#define BOOT_IMAGE_BASE ((size_t)0x8000L)
#define BIOS_IMAGE_BASE ((size_t)0x80000L)
#define BIOS_IMAGE_ENTRY_SEG_OFF ((size_t)0x9000fff0L)

char phy_memcpy_to(size_t pos, const char *data, size_t size);
char phy_memcpy_from(char *data, size_t pos, size_t size);
char write_boot_image(char *data, size_t size);
char write_bios_image(char *data, size_t size);

#endif
