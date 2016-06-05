#ifndef BOOTER_FILE_H
#define BOOTER_FILE_H

#include <ntddk.h>
#include <ntstrsafe.h>
#include <wdf.h>

char modify_file(PCWSTR name, ULONG shared, const char *data, const size_t size, const long long skip);
char write_file(PCWSTR name, ULONG shared, const char *data, const size_t size);
size_t read_file(PCWSTR name, ULONG shared, char *buffer, const size_t size, const long long skip);

#endif
