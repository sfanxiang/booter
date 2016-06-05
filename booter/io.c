#include "io.h"

char write_boot_image(char *data, size_t size)
{
	PHYSICAL_ADDRESS pa;

	pa.QuadPart = BIOS_WARM_RESET_VECTOR;
	UINT32 *bios_space = MmMapIoSpace(pa, 4, MmNonCached);
	if (bios_space == NULL) return 0;

	pa.QuadPart = BOOT_IMAGE_BASE;
	char *boot_image = MmMapIoSpace(pa, size, MmNonCached);
	if (boot_image == NULL) {
		MmUnmapIoSpace(bios_space, 4);
		return 0;
	}

	memcpy(boot_image, data, size);
	*bios_space = BOOT_IMAGE_BASE;

	MmUnmapIoSpace(bios_space, 4);
	MmUnmapIoSpace(boot_image, size);

	return 1;
}
