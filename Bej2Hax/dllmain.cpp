// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "Patch.h"

Patch *swapAnywhere;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			swapAnywhere = new Patch(0x5A8A0C, { 0x90, 0x90 }); // nop out a jz
			swapAnywhere->Apply();
			
			break;
		}

		case DLL_PROCESS_DETACH: {
			if (swapAnywhere) {
				swapAnywhere->Reset();
				delete swapAnywhere;
			}

			break;
		}
	}
	return TRUE;
}

