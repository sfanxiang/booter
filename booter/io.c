#include "io.h"

#include "file.h"

char phy_memcpy_to(size_t pos, const char *data, size_t size)
{
	UNICODE_STRING uniName;
	OBJECT_ATTRIBUTES objAttr;

	RtlInitUnicodeString(&uniName, L"\\Device\\PhysicalMemory");
	InitializeObjectAttributes(&objAttr, &uniName,
		0, NULL, NULL);

	HANDLE handle;
	NTSTATUS ntstatus;
	
	ntstatus = ZwOpenSection(&handle, SECTION_MAP_READ | SECTION_MAP_WRITE, &objAttr);
	if (!NT_SUCCESS(ntstatus)) return 0;

	LARGE_INTEGER phy; char *mapped;
	for (size_t x = (size_t)PAGE_ALIGN(pos); x < pos + size; x += (size_t)PAGE_SIZE) {
		phy.QuadPart = x;
		mapped = NULL;
		size_t ps = PAGE_SIZE;
		ntstatus = ZwMapViewOfSection(handle, ZwCurrentProcess(), &mapped, 0,
			PAGE_SIZE, &phy, &(SIZE_T)ps, ViewUnmap, 0, PAGE_READWRITE);
		if (!NT_SUCCESS(ntstatus)) return 0;

		size_t shift = max(pos, x); shift = shift - (size_t)PAGE_ALIGN(shift);
		size_t done = x + shift - pos;
		memcpy(mapped + shift, data + done, min(PAGE_SIZE, size - done));

		ZwUnmapViewOfSection(ZwCurrentProcess(), mapped);
	}

	return 1;
}

char phy_memcpy_from(char *data, size_t pos, size_t size)
{
	UNICODE_STRING uniName;
	OBJECT_ATTRIBUTES objAttr;

	RtlInitUnicodeString(&uniName, L"\\Device\\PhysicalMemory");
	InitializeObjectAttributes(&objAttr, &uniName,
		0, NULL, NULL);

	HANDLE handle;
	NTSTATUS ntstatus;

	ntstatus = ZwOpenSection(&handle, SECTION_MAP_READ, &objAttr);
	if (!NT_SUCCESS(ntstatus)) return 0;

	LARGE_INTEGER phy; char *mapped;
	for (size_t x = (size_t)PAGE_ALIGN(pos); x < pos + size; x += (size_t)PAGE_SIZE) {
		phy.QuadPart = x;
		mapped = NULL;
		size_t ps = PAGE_SIZE;
		ntstatus = ZwMapViewOfSection(handle, ZwCurrentProcess(), &mapped, 0,
			PAGE_SIZE, &phy, &(SIZE_T)ps, ViewUnmap, 0, PAGE_READONLY);
		if (!NT_SUCCESS(ntstatus)) return 0;

		size_t shift = max(pos, x); shift = shift - (size_t)PAGE_ALIGN(shift);
		size_t done = x + shift - pos;
		memcpy(data + done, mapped + shift, min(PAGE_SIZE, size - done));

		ZwUnmapViewOfSection(ZwCurrentProcess(), mapped);
	}

	return 1;
}

char write_boot_image(char *data, size_t size)
{
	if (phy_memcpy_to(BOOT_IMAGE_BASE, data, size)) {
		DWORD base = (DWORD)BOOT_IMAGE_BASE;
		if (phy_memcpy_to(BIOS_WARM_RESET_VECTOR, (char*)&base, sizeof(base))) {
			return 1;
		}
	}
	return 0;
}
