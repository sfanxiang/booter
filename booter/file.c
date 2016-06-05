#include "file.h"

char modify_file(PCWSTR name, ULONG shared, const char *data, const size_t size, const long long skip)
{
	UNICODE_STRING uniName;
	OBJECT_ATTRIBUTES objAttr;

	RtlInitUnicodeString(&uniName, name);
	InitializeObjectAttributes(&objAttr, &uniName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	HANDLE handle;
	NTSTATUS ntstatus;
	IO_STATUS_BLOCK ioStatusBlock;

	ntstatus = ZwOpenFile(&handle,
		GENERIC_WRITE,
		&objAttr, &ioStatusBlock,
		shared,
		FILE_SYNCHRONOUS_IO_NONALERT);
	
	if (NT_SUCCESS(ntstatus)) {
		LARGE_INTEGER byteOffset;
		byteOffset.QuadPart = skip;
		ntstatus = ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock,
			(void*)data, (ULONG)size, &byteOffset, NULL);
		ZwClose(handle);
	}

	return NT_SUCCESS(ntstatus);
}

char write_file(PCWSTR name, ULONG shared, const char *data, const size_t size)
{
	UNICODE_STRING uniName;
	OBJECT_ATTRIBUTES objAttr;

	RtlInitUnicodeString(&uniName, name);
	InitializeObjectAttributes(&objAttr, &uniName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	HANDLE handle;
	NTSTATUS ntstatus;
	IO_STATUS_BLOCK ioStatusBlock;

	ntstatus = ZwCreateFile(&handle,
		GENERIC_WRITE,
		&objAttr, &ioStatusBlock, NULL,
		FILE_ATTRIBUTE_NORMAL,
		shared,
		FILE_OVERWRITE_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0);

	if (NT_SUCCESS(ntstatus)) {
		ntstatus = ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock,
			(void*)data, (ULONG)size, NULL, NULL);
		ZwClose(handle);
	}

	return NT_SUCCESS(ntstatus);
}

size_t read_file(PCWSTR name, ULONG shared, char *buffer, const size_t size, const long long skip)
{
	UNICODE_STRING uniName;
	OBJECT_ATTRIBUTES objAttr;

	RtlInitUnicodeString(&uniName, name);
	InitializeObjectAttributes(&objAttr, &uniName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	HANDLE handle;
	NTSTATUS ntstatus;
	IO_STATUS_BLOCK ioStatusBlock;

	ntstatus = ZwCreateFile(&handle,
		GENERIC_READ,
		&objAttr, &ioStatusBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		shared,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0);

	if (NT_SUCCESS(ntstatus)) {
		LARGE_INTEGER byteOffset;
		byteOffset.QuadPart = skip;
		ntstatus = ZwReadFile(handle, NULL, NULL, NULL, &ioStatusBlock,
			buffer, (ULONG)size, &byteOffset, NULL);
		ZwClose(handle);

		if (NT_SUCCESS(ntstatus)) return (size_t)ioStatusBlock.Information;
	}

	return 0;
}
