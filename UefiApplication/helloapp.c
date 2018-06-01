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
static EFI_GRAPHICS_OUTPUT_PROTOCOL* GOP = NULL;

// Package name
CHAR8 *gEfiCallerBaseName = "TestApp";

//
// Function prototypes
//
EFI_STATUS setupGUI(EFI_SYSTEM_TABLE*);
EFI_STATUS INIT(EFI_SYSTEM_TABLE*);

UINTN TextModeColumns;
UINTN TextModeRows;

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

	efiStatus = INIT(system_table);

	if (EFI_ERROR(efiStatus)) {
		return efiStatus;
	}

	// Print Hello World
	Print(L"Hello World!\n\n");

	while (1) {
		// -- CHECK FOR KEYBOARD EVENTS --
		EFI_STATUS efirc;
		system_table->ConIn->Reset(system_table->ConIn, FALSE);

		while ((efirc = system_table->BootServices->CheckEvent(system_table->ConIn->WaitForKey)) != 0) {}
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
	EFI_STATUS efiStatus;

	// Disable cursor
	system_table->ConOut->EnableCursor(system_table->ConOut, FALSE);

	// Clear input buffer
	system_table->ConIn->Reset(system_table->ConIn, TRUE);

	// Clear screen
	system_table->ConOut->ClearScreen(system_table->ConOut);

	// Setup GOP
	efiStatus = system_table->BootServices->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID**)&GOP);

	// Detect maximum screen width
	// and apply this mode
	UINT32 ModeNumber = 0;
	UINT32 MaxGOPMode = 0;
	UINT32 MaxTextMode = 0;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *SimpleTextOut;

	//
	// Re-query SimleTextOut for available modes
	// to enable one available for the current resolution
	//

	// Get current SimpleTextOutProtocol
	efiStatus = gBS->HandleProtocol(gST->ConsoleOutHandle, &gEfiSimpleTextOutProtocolGuid, (VOID**)&SimpleTextOut);

	if (EFI_ERROR(efiStatus)) {
		Print(L"ERROR: Failed to get EfiSimpleTextOutProtocol: %lx\n", efiStatus);
		return EFI_UNSUPPORTED;
	}

	if (SimpleTextOut != NULL) {
		MaxTextMode = SimpleTextOut->Mode->MaxMode;
	}

	// Set highest text mode number as SimpleText Output mode
	efiStatus = SimpleTextOut->SetMode(SimpleTextOut, MaxTextMode - 1);

	for (int i = 0; i < MaxTextMode; i++) {
		SimpleTextOut->QueryMode(SimpleTextOut, i, &TextModeColumns, &TextModeRows);
	}

	if (EFI_ERROR(efiStatus)) {
		Print(L"ERROR: Failed to set text output mode: %lx\n", efiStatus);
		return EFI_UNSUPPORTED;
	}

	// Get number of GOP modes
	MaxGOPMode = GOP->Mode->MaxMode;
	if (MaxGOPMode == 0) {
		Print(L"ERROR: No GOP mode available. \n");
		return EFI_UNSUPPORTED;
	}

	for (ModeNumber = 0; ModeNumber < GOP->Mode->MaxMode; ++ModeNumber) {
		EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
		UINTN SizeOfInfo;

		efiStatus = GOP->QueryMode(GOP, ModeNumber, &SizeOfInfo, &Info);
		if (EFI_ERROR(efiStatus)) {
			Print(L"ERROR: Failed to query GOP mode %lx\n", efiStatus);
			return EFI_UNSUPPORTED;
		}
	}

	// Set highest mode number as Graphics Output mode
	efiStatus = GOP->SetMode(GOP, MaxGOPMode - 1);

	if (EFI_ERROR(efiStatus)) {
		Print(L"ERROR: Failed to set graphics resolution: %lx\n", efiStatus);
		return EFI_UNSUPPORTED;
	}

	return efiStatus;
}

//
// Functionf or basic inits
//
EFI_STATUS INIT(EFI_SYSTEM_TABLE* system_table) {
	EFI_STATUS efiStatus;

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

	return efiStatus;
}