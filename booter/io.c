#include "io.h"

unsigned char inb(unsigned short port)
{
	_io_port_in = port;
	return inb_asm();
}

void outb(unsigned short port, unsigned char data)
{
	_io_port_in = port;
	_io_data_in = data;
	outb_asm();
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
		memcpy(mapped + shift, data + done, min(PAGE_SIZE - shift, size - done));

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
		memcpy(data + done, mapped + shift, min(PAGE_SIZE - shift, size - done));

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
			return 1;
		}
	}
	return 0;
}

char write_payload_addr(size_t addr)
{
	return phy_memcpy_to(BOOT_IMAGE_PAYLOAD_PTR, (char*)&addr, 4);
}
