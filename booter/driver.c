#include "file.h"
#include "io.h"

#include <ntddk.h>
#include <ntstrsafe.h>
#include <wdf.h>

#define RUN_FILE (L"\\DosDevices\\C:\\booter_run.txt")
#define RUN_MAX 10
#define IMAGE_FILE (L"\\DosDevices\\C:\\booter_image")
#define IMAGE_MAX 0x20000

#define MESSAGE_FILE (L"\\DosDevices\\C:\\booter_message.txt")
#define TEST_MESSAGE "this is a test message from booter\n"
#define DUMP_FILE (L"\\DosDevices\\C:\\booter_dump")

DRIVER_INITIALIZE DriverEntry;

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	static char run_buffer[RUN_MAX + 1];
	static char image_buffer[IMAGE_MAX + 1];
	size_t run_size, image_size;
	char *p;
	
	run_size = read_file(RUN_FILE, 0, run_buffer, RUN_MAX, 0);
	for (size_t i = 0; i < run_size; i++) {
		switch (run_buffer[i]) {
		case 'm':
			if (!write_file(MESSAGE_FILE, 0, TEST_MESSAGE, sizeof(TEST_MESSAGE) - 1)) {
				return STATUS_SUCCESS;
			}
			break;
		case 'w':
			image_size = read_file(IMAGE_FILE, 0, image_buffer, IMAGE_MAX, 0);
			if (image_size == 0) {
				return STATUS_SUCCESS;
			}
			if (!write_boot_image(image_buffer, image_size)) {
				return STATUS_SUCCESS;
			}
			cmos_set_warm_reset();
			break;
		case 'd':
			p = MmAllocateNonCachedMemory(LOW_PHY_MEM_SIZE);
			if (p == NULL) return STATUS_SUCCESS;
			phy_memcpy_from(p, 0, LOW_PHY_MEM_SIZE);
			char res = write_file(DUMP_FILE, 0, p, LOW_PHY_MEM_SIZE);
			MmFreeNonCachedMemory(p, LOW_PHY_MEM_SIZE);
			if (!res) return STATUS_SUCCESS;
			break;
		case 'o':
			if (run_size - i > 3) {
				outb((unsigned short)run_buffer[i + 1] + ((unsigned short)run_buffer[i + 2]) * 256, run_buffer[i + 3]);
			}
			break;
		case 'i':
			if (run_size - i > 2) {
				unsigned char data = inb((unsigned short)run_buffer[i + 1] + ((unsigned short)run_buffer[i + 2]) * 256);
				write_file(MESSAGE_FILE, 0, (char*)&data, sizeof(data));
			}
			break;
		case 'r':
			reboot();
			break;
		default:
			break;
		}
	}

	return STATUS_SUCCESS;
}
