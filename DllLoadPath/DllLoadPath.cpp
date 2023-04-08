#include <phnt_windows.h>
#include <phnt.h>
#include <cstdio>
#include <sddl.h>

#pragma comment(lib,"ntdll.lib")

int main()
{
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	UNICODE_STRING DllPath;
	UNICODE_STRING CurrentDirectory;

	RtlInitUnicodeString(&ImagePathName, LR"(\??\C:\Windows\System32\WerFault.exe)");
	RtlInitUnicodeString(&CommandLine, LR"(C:\Users\User\DLLs1\WerFault.exe)");
	RtlInitUnicodeString(&DllPath, LR"(C:\Users\User\DLLs2)");
	RtlInitUnicodeString(&CurrentDirectory, LR"(C:\Windows\system32\)");

	PRTL_USER_PROCESS_PARAMETERS UserProcessParam = {};
	RTL_USER_PROCESS_INFORMATION ProcessInfo = {};

	NTSTATUS status = RtlCreateProcessParameters(&UserProcessParam, &CommandLine, &DllPath, &CurrentDirectory, &CommandLine, NULL, NULL, NULL, NULL, NULL);
	if (!NT_SUCCESS(status))
	{
		printf("RtlCreateProcessParameters NTSTATUS: 0x%08X\n", status);
		return-1;
	}

	status = RtlCreateUserProcess(&ImagePathName, NULL, UserProcessParam, NULL, NULL, NULL, FALSE, NULL, NULL, &ProcessInfo);
	if (!NT_SUCCESS(status))
	{
		printf("RtlCreateUserProcess NTSTATUS: 0x%08X\n", status);
		return -1;
	}
	status = NtResumeThread(ProcessInfo.ThreadHandle, nullptr);
	if (!NT_SUCCESS(status))
	{
		printf("NtResumeThread NTSTATUS: 0x%08X\n", status);
		return -1;
	}
	WaitForSingleObject(ProcessInfo.ThreadHandle, INFINITE);

	return 0;
}