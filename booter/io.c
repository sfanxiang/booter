#include "io.h"

#include "file.h"

char write_boot_image(char *data, size_t size)
{
	if (modify_file(L"/Device/PhysicalMemory", FILE_SHARE_READ | FILE_SHARE_WRITE, data, size, BOOT_IMAGE_BASE)) {
		DWORD base = (DWORD)BOOT_IMAGE_BASE;
		if (modify_file(L"/Device/PhysicalMemory", FILE_SHARE_READ | FILE_SHARE_WRITE, (char*)&base, sizeof(base), BIOS_WARM_RESET_VECTOR))
		{
			write_file(L"\\DosDevices\\C:\\booter_message.txt", 0, "success\n", sizeof("success\n") - 1);
			return 1;
		}
	}
	return 0;
}
