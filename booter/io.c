#include "io.h"

#include "file.h"

char *memcpy2(char *dst, const char *src, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		*(dst + i) = *(src + i);
	}
	return dst;
}

char phy_memcpy_to(size_t pos, const char *data, size_t size)
{
	UNICODE_STRING uniName;
	OBJECT_ATTRIBUTES objAttr;

	RtlInitUnicodeString(&uniName, L"\\Device\\PhysicalMemory");
	InitializeObjectAttributes(&objAttr, &uniName,
		OBJ_KERNEL_HANDLE, NULL, NULL);

	HANDLE handle;
	NTSTATUS ntstatus;

	ntstatus = ZwOpenSection(&handle, SECTION_MAP_READ | SECTION_MAP_WRITE, &objAttr);
	if (!NT_SUCCESS(ntstatus)) return 0;

	LARGE_INTEGER phy; char *mapped; size_t ps;
	for (size_t x = (size_t)PAGE_ALIGN(pos); x < pos + size; x += (size_t)PAGE_SIZE) {
		phy.QuadPart = x;
		mapped = NULL;
		ps = PAGE_SIZE;
		ntstatus = ZwMapViewOfSection(handle, ZwCurrentProcess(), &mapped, 0,
			PAGE_SIZE, &phy, &(SIZE_T)ps, ViewUnmap, 0, PAGE_READWRITE);
		if (!NT_SUCCESS(ntstatus)) {
			ZwClose(handle);
			return 0;
		}

		size_t shift = max(pos, x); shift = shift - (size_t)PAGE_ALIGN(shift);
		size_t done = x + shift - pos;
		memcpy2(mapped + shift, data + done, min(PAGE_SIZE - shift, size - done));

		ZwUnmapViewOfSection(ZwCurrentProcess(), mapped);
	}

	ZwClose(handle);
	return 1;
}

char phy_memcpy_from(char *data, size_t pos, size_t size)
{
	UNICODE_STRING uniName;
	OBJECT_ATTRIBUTES objAttr;

	RtlInitUnicodeString(&uniName, L"\\Device\\PhysicalMemory");
	InitializeObjectAttributes(&objAttr, &uniName,
		OBJ_KERNEL_HANDLE, NULL, NULL);

	HANDLE handle;
	NTSTATUS ntstatus;

	ntstatus = ZwOpenSection(&handle, SECTION_MAP_READ | SECTION_MAP_WRITE, &objAttr);
	if (!NT_SUCCESS(ntstatus)) return 0;

	LARGE_INTEGER phy; char *mapped; size_t ps;
	for (size_t x = (size_t)PAGE_ALIGN(pos); x < pos + size; x += (size_t)PAGE_SIZE) {
		phy.QuadPart = x;
		mapped = NULL;
		ps = PAGE_SIZE;
		ntstatus = ZwMapViewOfSection(handle, ZwCurrentProcess(), &mapped, 0,
			PAGE_SIZE, &phy, &(SIZE_T)ps, ViewUnmap, 0, PAGE_READWRITE);
		if (!NT_SUCCESS(ntstatus)) {
			ZwClose(handle);
			return 0;
		}

		size_t shift = max(pos, x); shift = shift - (size_t)PAGE_ALIGN(shift);
		size_t done = x + shift - pos;
		memcpy2(data + done, mapped + shift, min(PAGE_SIZE - shift, size - done));

		ZwUnmapViewOfSection(ZwCurrentProcess(), mapped);
	}

	ZwClose(handle);
	return 1;
}

char write_boot_image(char *data, size_t size)
{
	if (phy_memcpy_to(BOOT_IMAGE_BASE, data, size)) {
		DWORD base = (DWORD)BOOT_IMAGE_BASE;
		if (phy_memcpy_to(BIOS_WARM_RESET_VECTOR, (char*)&base, sizeof(base))) {
			PHYSICAL_ADDRESS pa;
			pa.QuadPart = 0x00000000FFFFFFFF;
			char *t = MmAllocateContiguousMemory(8192, pa);
			if (t && phy_memcpy_from(t, 0x7010, 8191)) {
				write_file(L"\\DosDevices\\C:\\booter_message.txt", 0, t, 8191);
			}
			MmFreeContiguousMemory(t);
			return 1;
		}
	}
	return 0;
}
