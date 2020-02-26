#pragma once

#include <Windows.h>

#define InitializeObjectAttributes( i, o, a, r, s ) {    \
      (i)->Length = sizeof( OBJECT_ATTRIBUTES );         \
      (i)->RootDirectory = r;                            \
      (i)->Attributes = a;                               \
      (i)->ObjectName = o;                               \
      (i)->SecurityDescriptor = s;                       \
      (i)->SecurityQualityOfService = NULL;              \
   }

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	LPCWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG Length;
	PVOID RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

typedef NTSTATUS(WINAPI* MyRtlGetVersion)(
	LPOSVERSIONINFOEXW);

typedef void (WINAPI* _RtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString
	);

//Windows 10 / Server 2016 specific syscalls
extern "C" NTSTATUS ZwOpenKey10(PHANDLE KeyHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes);
extern "C" NTSTATUS NtSetValueKey10(HANDLE KeyHandle, PUNICODE_STRING ValueName, ULONG TitleIndex, ULONG Type, PVOID Data, ULONG DataSize);
extern "C" NTSTATUS NtDeleteValueKey10(HANDLE KeyHandle, PUNICODE_STRING ValueName);
extern "C" NTSTATUS NtClose10(HANDLE hKey);


//Windows 7 SP1 / Server 2008 R2 specific syscalls
extern "C" NTSTATUS ZwOpenKey7SP1(PHANDLE KeyHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes);
extern "C" NTSTATUS NtSetValueKey7SP1(HANDLE KeyHandle, PUNICODE_STRING ValueName, ULONG TitleIndex, ULONG Type, PVOID Data, ULONG DataSize);
extern "C" NTSTATUS NtDeleteValueKey7SP1(HANDLE KeyHandle, PUNICODE_STRING ValueName);
extern "C" NTSTATUS NtClose7SP1(HANDLE hKey);

NTSTATUS(*ZwOpenKey)(
	PHANDLE KeyHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes
	);

NTSTATUS(*NtSetValueKey)(
	HANDLE          KeyHandle,
	PUNICODE_STRING ValueName,
	ULONG           TitleIndex,
	ULONG           Type,
	PVOID           Data,
	ULONG           DataSize
	);

NTSTATUS(*NtDeleteValueKey)(
	HANDLE          KeyHandle,
	PUNICODE_STRING ValueName
	);

NTSTATUS(*NtClose)(
	HANDLE hKey
	);