#include "file.h"
#include "io.h"

#include <ntddk.h>
#include <ntstrsafe.h>
#include <wdf.h>

#define RUN_FILE (L"\\DosDevices\\C:\\booter_run.txt")
#define RUN_MAX 10
#define IMAGE_FILE (L"\\DosDevices\\C:\\booter_image")
#define IMAGE_MAX 4095

#define MESSAGE_FILE (L"\\DosDevices\\C:\\booter_message.txt")
#define TEST_MESSAGE "this is a test message from booter\n"

DRIVER_INITIALIZE DriverEntry;

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	char run_buffer[RUN_MAX + 1], image_buffer[IMAGE_MAX + 1];
	size_t run_size, image_size; 
	
	run_size = read_file(RUN_FILE, run_buffer, RUN_MAX);
	for (size_t i = 0; i < run_size; i++) {
		switch (run_buffer[i]) {
		case 'm':
			write_file(MESSAGE_FILE, TEST_MESSAGE, sizeof(TEST_MESSAGE) - 1);
			break;
		case 'w':
			image_size = read_file(IMAGE_FILE, image_buffer, IMAGE_MAX);
			write_boot_image(image_buffer, image_size);
			cmos_set_warm_reset();
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
