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

	char buffer[4096];
	size_t size; 
	
	size = read_file(RUN_FILE, buffer, RUN_MAX);
	for (char *p = buffer; *p != '\0'; p++) {
		switch (*p) {
		case 'm':
			write_file(MESSAGE_FILE, TEST_MESSAGE, sizeof(TEST_MESSAGE));
			break;
		case 'w':
			// write
			break;
		case 'r':
			reboot();
			break;
		}
	}

	// write_file(L"\\DosDevices\\C:\\test.txt", "test\n", 5);

	return STATUS_SUCCESS;
}
