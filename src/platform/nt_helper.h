#ifndef NT_HELPER_
#define NT_HELPER_ 1

#include <conio.h>
#include <spdlog/spdlog.h>
#include <windows.h>

namespace perfect_invaxion {
namespace winnt {

typedef enum _PROCESSINFOCLASS {
	ProcessBasicInformation, // q: PROCESS_BASIC_INFORMATION,
							 // PROCESS_EXTENDED_BASIC_INFORMATION
} PROCESSINFOCLASS;

typedef NTSTATUS (*NtQueryInformationProcess_type)(HANDLE ProcessHandle,
	PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation,
	ULONG ProcessInformationLength, PULONG ReturnLength);

typedef struct _ALK_PROCESS_BASIC_INFORMATION {
	PVOID Reserved1;
	void* PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	ULONG_PTR ParentProcessId;
} ALK_PROCESS_BASIC_INFORMATION;

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

bool is_launch_from_explorer()
{
	DWORD explorer_pid = 0;
	GetWindowThreadProcessId(GetShellWindow(), &explorer_pid);

	NtQueryInformationProcess_type NtQueryInformationProcess =
		reinterpret_cast<NtQueryInformationProcess_type>(GetProcAddress(
			LoadLibraryA("ntdll.dll"), "NtQueryInformationProcess"));
	if (NtQueryInformationProcess == nullptr)
		return false;

	ALK_PROCESS_BASIC_INFORMATION pbi;
	SecureZeroMemory(&pbi, sizeof(ALK_PROCESS_BASIC_INFORMATION));

	NTSTATUS status = NtQueryInformationProcess(GetCurrentProcess(),
		ProcessBasicInformation, reinterpret_cast<PVOID>(&pbi),
		sizeof(ALK_PROCESS_BASIC_INFORMATION), 0);
	if (!NT_SUCCESS(status))
		return false;

	return pbi.ParentProcessId == explorer_pid;
}

void pause()
{
	spdlog::info("press any key to continue...");
	_getch();
}

} // namespace winnt
} // namespace perfect_invaxion

#endif // NT_HELPER_
