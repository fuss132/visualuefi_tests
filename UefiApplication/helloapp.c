//
// Basic UEFI Libraries
//
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

//
// Boot and Runtime Services
//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

//
// Shell Library
//
#include <Library/ShellLib.h>

// We run on any UEFI Specification
extern CONST UINT32 _gUefiDriverRevision = 0;

// Package name
CHAR8 *gEfiCallerBaseName = "TestApp";

EFI_STATUS
EFIAPI
UefiUnload (
    IN EFI_HANDLE ImageHandle
    )
{
    // This code should be compiled out and never called 
    ASSERT(FALSE);
}

EFI_STATUS
EFIAPI
UefiMain (IN EFI_HANDLE image_handle, IN EFI_SYSTEM_TABLE* system_table)
{
    EFI_STATUS efiStatus;

	// Initialize lib
	efiStatus = ShellInitialize();
	if (EFI_ERROR(efiStatus))
	{
		Print(L"Failed to initialize shell: %lx\n", efiStatus);
		return efiStatus;
	}

	// Clear screen
	system_table->ConOut->ClearScreen(system_table->ConOut);

    // Print Hello World
    Print(L"Hello World!");

    return efiStatus;
}

