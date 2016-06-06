#ifndef BOOTER_IO_H
#define BOOTER_IO_H

#include <ntddk.h>
#include <ntstrsafe.h>
#include <wdf.h>

void __cdecl reboot();
void __cdecl cmos_set_warm_reset();

#define BIOS_WARM_RESET_VECTOR ((size_t)0x467L)
#define BOOT_IMAGE_BASE ((size_t)0x8000L)

char *memcpy2(char *dst, const char *src, size_t size);
char phy_memcpy_to(size_t pos, const char *data, size_t size);
char phy_memcpy_from(char *data, size_t pos, size_t size);
char write_boot_image(char *data, size_t size);

#endif
