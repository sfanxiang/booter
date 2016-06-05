#ifndef BOOTER_FILE_H
#define BOOTER_FILE_H

#include <ntddk.h>
#include <ntstrsafe.h>
#include <wdf.h>

char write_file(PCWSTR name, const char *data, const size_t size);
size_t read_file(PCWSTR name, char *buffer, const size_t size);

#endif
