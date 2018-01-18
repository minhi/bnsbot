#include <Windows.h>
#include <stdio.h>
#include <vector>

#include "Memory.h"
#include "PolyHook.h"
#include "bns.h"
#include "coord.h"
#include "path.h"

#define offset 0xF911E0;

typedef void *(__fastcall *oFunc)(void *rcx, void *rdx, void *r8);
oFunc Func;

typedef bool *(__fastcall *oMove)(void *rcx, float xmm1, int r8);
oMove Move;

typedef bool *(__fastcall *oMove2)(DWORD rcx, float xmm1, float xmm2, float xmm3);
oMove2 Move2;
oMove2 Move2_orig;

typedef void *(__fastcall *oSend)(DWORD rcx, DWORD rdx, DWORD r8);
oSend Send;

bns::sigs::ObjectCoord ObjectCoord_orig;

bool hkObjectCoord(uintptr_t rcx, uintptr_t rdx) {
	float *coords = (float*)(rdx + 0x30);
	//printf("%p = (%f, %f, %f)\n", (void *)rdx, coords[0], coords[1], coords[2]);
	return ObjectCoord_orig(rcx, rdx);
}

bool hkMove2(DWORD rcx, float xmm1, float xmm2, float xmm3) {
	printf("hooky %p\n", rcx);
	return Move2_orig(rcx, xmm1, xmm2, xmm3);
}

DWORD WINAPI MainThread(LPVOID param) {
	printf("Created MainThread.\n");

	bns::Bns *bns = bns::Bns::getInstance();
	if (!bns) {
		printf("Error getting Bns instance\n");
		return -1;
	}

	uintptr_t base = (uintptr_t)GetModuleHandle(0);
	Func = (oFunc)(base + 0xF911E0);

	printf("Base address: %p\n", (void *)base);
	printf("Send function: %p\n", (void *)Func);

	uintptr_t rcx = *(uintptr_t *)(base + 0x01816148);
	printf("rcx: %p\n", (void *)rcx);

	uintptr_t rdx = *(uintptr_t *)(*(uintptr_t *)(base + 0x018140E0) + 0x48);
	printf("rdx: %p\n", (void *)rdx);

	uintptr_t base_shipping = (uintptr_t)GetModuleHandle(L"bsengine_shipping64.dll");
	Move = (oMove)(base_shipping + 0x1DEA0F0);

	printf("Base_shipping address: %p\n", (void *)base_shipping);
	printf("Move function: %p\n", (void *)Move);

	Move2 = (oMove2)(base_shipping + 0x1DED1D0);
	printf("Move2 function: %p\n", (void *)Move2);

	byte location[] =
	{
		0x40, 0x16, 0x21, 0x41, 0x01, 0x00, 0x00, 0x00, 0x3D, 0x00, 0xAA, 0x88, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x3A, 0x00, 0x5C, 0x00, 0x50, 0x00, 0x00, 0x00, 0xF7, 0xCA, 0x24, 0x06, 0x9D, 0xF6,
		0x05, 0xCB, 0x2C, 0x06, 0x9D, 0xF6, 0x46, 0x00, 0xDF, 0x2B, 0xFF, 0xFF, 0x91, 0x18, 0x00, 0x00,
		0x75, 0xDA, 0xFF, 0xFF, 0x16, 0x2C, 0xFF, 0xFF, 0xB0, 0x18, 0x00, 0x00, 0x75, 0xDA, 0xFF, 0xFF,
		0xFF, 0x00, 0x12, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x5C, 0x00, 0x42, 0x00, 0x6E, 0x00,
		0xB0, 0x36, 0xFE, 0x01, 0x01, 0x00, 0x00, 0x00, 0xB0, 0x36, 0xFE, 0x01, 0x01, 0x00, 0x00, 0x00,
		0x04, 0x00, 0x79, 0x00, 0x29, 0x00, 0x00, 0x00, 0x10, 0x7B, 0x90, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x50, 0x7B, 0x90, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x99, 0x53, 0x49, 0x00, 0x90, 0x03, 0x80
	};

	byte data[] =
	{
		0xB0, 0xE7, 0x0F, 0x41, 0x01, 0x00, 0x00, 0x00, 0xCB, 0x03, 0x09, 0x58, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0x69, 0x00, 0x00,
		0x3D, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x7F, 0x43, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
		0x9F, 0x00, 0xFF, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00/*,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0xF4, 0xD9, 0xF8, 0xFE, 0x00, 0x14, 0x01, 0x89,
		0x3C, 0x00, 0x66, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0xD0, 0x98, 0x09, 0x58, 0x01, 0x00, 0x00, 0x00*/
	};

	byte sumLmb1[] =
	{
		0xB0, 0xE7, 0x0F, 0x41, 0x01, 0x00, 0x00, 0x00, 0xCB, 0x03, 0x6C, 0x58, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF5, 0x65, 0x00, 0x00,
		0x33, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0xD6, 0x20, 0x04, 0x00,
		0xA6, 0x00, 0xFF, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x19, 0x24, 0x05, 0x00, 0x00, 0x00, 0x00, 0xAE, 0xA1, 0x7E, 0xA3, 0x00, 0x43, 0x04, 0x91,
		0x31, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0xF0, 0x66, 0x6C, 0x58, 0x00, 0x00, 0x00, 0x00
	};

	byte sumLmb2[] =
	{
		0xB0, 0xE7, 0x0F, 0x41, 0x01, 0x00, 0x00, 0x00, 0xCB, 0x03, 0x8C, 0xCE, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x35, 0x1A, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF6, 0x65, 0x00, 0x00,
		0x33, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x35, 0x02, 0x04, 0x00, // <-- determines target
		0xA1, 0x00, 0xFF, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x80, 0x5A, 0x3F, 0x00, 0x00, 0x98, 0x3C, 0xA1, 0x8F, 0x01, 0xC9, 0x00, 0xB0, 0x00, 0x80,
		0x3C, 0x00, 0x66, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0xF0, 0xD9, 0x8C, 0xCE, 0x00, 0x00, 0x00, 0x00
	};

	byte rmb[] =
	{
		0xB0, 0xE7, 0x0F, 0x41, 0x01, 0x00, 0x00, 0x00, 0xCB, 0x03, 0x61, 0x58, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC5, 0x66, 0x00, 0x00,
		0x0B, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x20, 0x05, 0x04, 0x00,
		0xA1, 0x00, 0xFF, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
		0x80, 0x02, 0xE2, 0x4B, 0x01, 0x00, 0x00, 0x00, 0x80, 0x02, 0xE2, 0x4B, 0x01, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0xA0, 0x06, 0xE2, 0x4B, 0x01, 0x00, 0x00, 0x00,
		0xA0, 0x06, 0xE2, 0x4B, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00,
		0x40, 0x05, 0xE2, 0x4B, 0x01, 0x00, 0x00, 0x00, 0x40, 0x05, 0xE2, 0x4B, 0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE6, 0x86, 0x5B, 0x0A, 0x00, 0xB0, 0x00, 0x80,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x34, 0x61, 0x58, 0x00, 0x00, 0x00, 0x00
	};

	byte mushin[] =
	{
		0x78, 0x48, 0x11, 0x41, 0x01, 0x00, 0x00, 0x00, 0x91, 0x04, 0xD5, 0x81, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x74, 0x00, 0x20, 0x00, 0x4D, 0x00, 0x14, 0x00, 0xBE, 0x61, 0x06, 0x00, 0x69, 0x00, // <- 0x06 here?
		0x01, 0x00, 0x00, 0x00, 0x79, 0x5F, 0x04, 0x00, 0x01, 0x00, 0x6F, 0x00, 0x77, 0x00, 0x65, 0x00,
		0x00, 0x25, 0x0A, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x25, 0x0A, 0x2C, 0x01, 0x00, 0x00, 0x00,
		0x3D, 0x00, 0x22, 0x00, 0x00, 0x00, 0x01, 0x00, 0x29, 0xDD, 0x52, 0x64, 0x2B, 0xDF, 0x00, 0x93,
		0x70, 0x5D, 0x59, 0x25, 0x01, 0x00, 0x00, 0x00, 0xE0, 0xA6, 0xD5, 0x81, 0x01, 0x00, 0x00, 0x00,
		0x20, 0x00, 0x2B, 0x00, 0x39, 0x00, 0x30, 0x00, 0x30, 0x00, 0x20, 0x00, 0x61, 0x00, 0x70, 0x00,
		0x20, 0x00, 0x66, 0x00, 0x61, 0x00, 0x73, 0x00, 0x74, 0x00, 0x20, 0x00, 0x35, 0x00, 0x2F, 0x00
	};

	// Variables storing the "player" pointer
	std::vector<uintptr_t> offsets = { 0x043E0B50, 0x80, 0x60, 0x250, 0x0 };
	std::vector<uintptr_t> offsets2 = { 0x0, 0x584, 0x0, 0x68, 0x29C, 0x0 };

	// Find base address using pattern scanning
	BYTE *pattern = (BYTE *) "\x48\x8B\x05\x7B\xC2\x0B\x03\x48\x8B\x90\x84\x05\x00\x00\x48\x8B\x02\x48\x8B\x50\x68\x48\x8B\x82\x9C\x02\x00\x00";
	char *mask = "xxx????xxxxxxxxxxxxxxxxxxxxx";
	BYTE *pOpcode_player = (BYTE *)Pattern(base_shipping, 0xB000000, pattern, mask); // opcode mov rax, qword ptr ds: [xxxxxx] (we want to find out xxxxx)
	if (!pOpcode_player) {
		printf("ERROR: pOpcode_player\n");
		return -1;
	}

	printf("opcode_player: %p\n", pOpcode_player);

	BYTE player_adr_raw[4];
	memcpy(player_adr_raw, pOpcode_player + 3, 4 * sizeof(BYTE));

	printf("player_adr_raw: ");
	for (int i = 0; i < 4; i++) {
		printf("%x", player_adr_raw[i]);
	}
	printf("\n");

	DWORD pPlayer_base = 0;
	// Byte-array to DWORD
	for (int i = 0; i < 4; i++) {
		pPlayer_base += player_adr_raw[i] << 8 * i;
	}

	printf("pPlaye_base_relative: %p\n", pPlayer_base);

	pPlayer_base += (DWORD)pOpcode_player + 0x07; // Relative address, so we want to add the address of the next instruction to it.

	printf("pPlaye_base: %p\n", pPlayer_base);

	DWORD pPlayer = GetAddressByPointer(pPlayer_base, offsets2);
	if (!pPlayer) {
		printf("ERROR: pPlayer is NULL\n");
		return -1;
	}

	float *currentCoords = (float *)(pPlayer + 0x80); // Address to array of current coordinates

	printf("pPlayer: %p\n", (void *)pPlayer);
	printf("currentCoords = %p (%f,%f,%f)\n", currentCoords, currentCoords[0], currentCoords[1], currentCoords[2]);

	std::shared_ptr<PLH::Detour> Detour_Ex(new PLH::Detour);
	Detour_Ex->SetupHook((BYTE *)Move2, (BYTE *)hkMove2); //can cast to byte* to
	Detour_Ex->Hook();
	Move2_orig = Detour_Ex->GetOriginal<oMove2>();

	while (!GetAsyncKeyState(VK_END)) {
		if (GetAsyncKeyState(VK_NUMPAD0)) {
				printf("Do something..\n");
				Func((void *)rcx,
					(void *)rdx,
					(void *)data);
				Sleep(250);
		}
		else if (GetAsyncKeyState(VK_NUMPAD1)) {

			printf("boom\n");
			Func((void *)rcx,
				(void *)rdx,
				(void *)mushin);
			Sleep(250);
		}
		else if (GetAsyncKeyState(VK_NUMPAD2)) {
			printf("boom\n");
			Func((void *)rcx,
				(void *)rdx,
				(void *)sumLmb1);
			Sleep(250);
		}
		else if (GetAsyncKeyState(VK_NUMPAD3)) {
			printf("boom\n");
			Func((void *)rcx,
				(void *)rdx,
				(void *)rmb);
			Sleep(250);
		}
		else if (GetAsyncKeyState(VK_NUMPAD4)) {
			printf("boomo\n");
			using namespace bot;
			std::vector<Coord> v = {
				{ -53460.5f, 5289.35f, 9982.39f },
				{ -53360.5f, 5289.35f, 9982.39f },
				{ -53360.5f, 5189.35f, 9982.39f },
				{ -53460.5f, 5189.35f, 9982.39f },
				{ -53460.5f, 5289.35f, 9982.39f },
			};
			//bns->Move(bns->GetPlayer(), -53460.5, 5289.35, 9982.39);
			Command *p = new Path(v);
			p->Execute();
			Sleep(250);
		}
		else if (GetAsyncKeyState(VK_NUMPAD5)) {
			printf("Reset player structure\n");
		}
		else if (GetAsyncKeyState(VK_NUMPAD6)) {
			printf("Sending\n");
			DWORD base_struct = 0x00000000E00673D0;
			DWORD skill = 0x69CD;

			Send = (oSend)(base + 0x5444D0);
			printf("Send address: %p\n", Send);

			while (!GetAsyncKeyState(VK_END)) {
				Send(0x00000001115D2660, 0x0000000000000001, 0x0000000000000001);
				Sleep(50);
			}

		}
	}

	printf("Quitting.\n");
	Detour_Ex->UnHook();
	FreeConsole();
	FreeLibraryAndExitThread((HMODULE)param, 0);
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		AllocConsole();
		SetConsoleTitleA("bnsbot (nice name btw)");
		AttachConsole(GetCurrentProcessId());

		FILE *pFile = nullptr;
		freopen_s(&pFile, "CON", "r", stdin);
		freopen_s(&pFile, "CON", "w", stdout);
		freopen_s(&pFile, "CON", "w", stderr);

		printf("===== BNS BOT (totally working right now, nice name btw) =====\n");

		CreateThread(0, 0, MainThread, hinstDLL, 0, 0);

		DisableThreadLibraryCalls(hinstDLL);
	}
	else if (fdwReason == DLL_PROCESS_ATTACH) {
		FreeConsole();
	}

	return true;
}