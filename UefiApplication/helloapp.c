// Basic UEFI Libraries
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

// Boot and Runtime Services
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

// Shell Library
#include <Library/ShellLib.h>

// We run on any UEFI Specification
extern CONST UINT32 _gUefiDriverRevision = 0;

// Package name
CHAR8 *gEfiCallerBaseName = "TestApp";

//
// Function prototypes
//
EFI_STATUS setupGUI(EFI_SYSTEM_TABLE*);

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
UefiMain(IN EFI_HANDLE image_handle, IN EFI_SYSTEM_TABLE* system_table)
{
	EFI_STATUS efiStatus;
	EFI_INPUT_KEY key;

	// Initialize lib
	efiStatus = ShellInitialize();
	if (EFI_ERROR(efiStatus))
	{
		Print(L"Failed to initialize shell: %lx\n", efiStatus);
		return efiStatus;
	}

	// Setup GUI
	efiStatus = setupGUI(system_table);
	if (EFI_ERROR(efiStatus))
	{
		Print(L"Failed to initialize GUI: %lx\n", efiStatus);
		return efiStatus;
	}

	// Print Hello World
	Print(L"Hello World!\n\n");

	while (1) {
		// -- CHECK FOR KEYBOARD EVENTS --
		system_table->BootServices->CheckEvent(system_table->ConIn->WaitForKey);
		system_table->ConIn->ReadKeyStroke(system_table->ConIn, &key);

		if (key.ScanCode == SCAN_UP) {
			Print(L"Pressed up arrow key\n");
		}
		else if (key.ScanCode == SCAN_DOWN) {
			Print(L"Pressed down arrow key\n");
		}
	}

	return EFI_SUCCESS;
}

//
// Function to setup screen resolution by determining
// the best available mode & activating it
//
EFI_STATUS setupGUI(EFI_SYSTEM_TABLE* system_table) {
	// Disable cursor
	system_table->ConOut->EnableCursor(system_table->ConOut, FALSE);

	// Clear input buffer
	system_table->ConIn->Reset(system_table->ConIn, FALSE);

	// Clear screen
	system_table->ConOut->ClearScreen(system_table->ConOut);

	return EFI_SUCCESS;
}
