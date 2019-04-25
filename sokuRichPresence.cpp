#define SWRS_USES_HASH

#include <cstring>
#include <cstdlib>
#include <string>

#include <windows.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <iostream>
#include <d3d9.h>
#include <vector>
#include <sstream>

#include "swrs.h"
#include "fields.h"
#include "address.h"

#include "include/discord_register.h"
#include "include/discord_rpc.h"


// Original function calls, needed to create a new game, render, process game loop and destroy game.
#define CBattleManager_Create(p) \
  Ccall(p, s_origCBattleManager_OnCreate, void*, ())()
#define CBattleManager_Render(p) \
  Ccall(p, s_origCBattleManager_OnRender, void, ())()
#define CBattleManager_Process(p) \
  Ccall(p, s_origCBattleManager_OnProcess, int, ())()
#define CBattleManager_Destruct(p, dyn) \
  Ccall(p, s_origCBattleManager_OnDestruct, void*, (int))(dyn)

// These hold the reference to the instruction memory where the original game function lives. 
// unsigned long long stands for the DWORD (which is 4bytes for some reason on this machine)
static unsigned long long s_origCBattleManager_OnCreate;
static unsigned long long s_origCBattleManager_OnDestruct;
static unsigned long long s_origCBattleManager_OnRender;
static unsigned long long s_origCBattleManager_OnProcess;
DiscordRichPresence discordPresence;

#define ADDR_BMGR_P1 0x0C
#define ADDR_BMGR_P2 0x10

void InitDiscord()
{
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    // handlers.ready = handleDiscordReady;
    // handlers.errored = handleDiscordError;
    // handlers.disconnected = handleDiscordDisconnected;
    // handlers.joinGame = handleDiscordJoinGame;
    // handlers.spectateGame = handleDiscordSpectateGame;
    // handlers.joinRequest = handleDiscordJoinRequest;

    // Discord_Initialize(const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId, int pipe)
    Discord_Initialize("570515970381185024", &handlers, 1, "1234");
}

static void UpdatePresence()
{
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.state = "Yosu (Marisa)";
    discordPresence.details = "Thiena (Aya) vs";
    discordPresence.largeImageKey = "sokuicon";
    discordPresence.largeImageText = "placeholder";
    discordPresence.partyId = "ae488379-351d-4a4f-ad32-2b9b01c91657";
    discordPresence.spectateSecret = "MTIzNDV8MTIzNDV8MTMyNDU0";
    discordPresence.joinSecret = "MTI4NzM0OjFpMmhuZToxMjMxMjM= ";
    Discord_UpdatePresence(&discordPresence);
}



void* __fastcall CBattleManager_OnCreate(void *This) {
	CBattleManager_Create(This);
	std::cout << "OnCreate Called: "<< sizeof(void* (C::*)()) << std::endl;

	UpdatePresence();
	return This;
}


void __fastcall CBattleManager_OnRender(void *This) {
	CBattleManager_Render(This);
}


int __fastcall CBattleManager_OnProcess(void *This) {
	int ret;
	ret = CBattleManager_Process(This);
	int battleManager = ACCESS_INT(ADDR_BATTLE_MANAGER, 0);

	// Get address to the player data based on data inside "Battle Manager"
	void* p1 = ACCESS_PTR(battleManager, ADDR_BMGR_P1);	
	void* p2 = ACCESS_PTR(battleManager, ADDR_BMGR_P2);
	

/* HEALTH DISPLAY */
	//ACCESS_<variable_type>() is both used for accessing and writing to the resource.
	//Character variables are in fields.h l.1 "Character class"
	short p1Health = ACCESS_SHORT(p1, CF_CURRENT_HEALTH);
	short p1Spirit = ACCESS_SHORT(p1, CF_CURRENT_SPIRIT);
	short p2Health = ACCESS_SHORT(p2, CF_CURRENT_HEALTH);
	short p2Spirit = ACCESS_SHORT(p2, CF_CURRENT_SPIRIT);
	

	//Press CTRL, effect visible in VS mode, as practice mode hardsets HP 
	if (GetKeyState(VK_CONTROL) & 0x8000) {
		std::cout << "P1: " << p1Health << "(" << (float) (p1Spirit) / 200 << ")"
			  << " [VS] P2: " << p2Health << "(" << (float) (p2Spirit) / 200 << ")" << std::endl;
		ACCESS_SHORT(p1, CF_CURRENT_HEALTH) = 5000;
	}


/* GRAZING */
	void* p1FrameData = ACCESS_PTR(p1, CF_CURRENT_FRAME_DATA);
	void* p2FrameData = ACCESS_PTR(p2, CF_CURRENT_FRAME_DATA);
	int p1FrameFlag = ACCESS_INT(p1FrameData, FF_FFLAGS);
	int p2FrameFlag = ACCESS_INT(p2FrameData, FF_FFLAGS);

	// Flags are in fields.h, l.99 "Frame flags"
	if (p1FrameFlag & FF_GRAZE) //binary operation on a bit to check if p1 is grazing or not.
		std::cout << "-";
	else
		std::cout << " ";


	return ret;
}


void* __fastcall CBattleManager_OnDestruct(void *This, int mystery, int dyn) {
	void* ret;
	ret = CBattleManager_Destruct(This, dyn);
	std::cout << "OnDestruct Called" << std::endl;

	return ret;
}


void OpenConsole() {
	if (AllocConsole()) {
		FILE* stream; // initializing this makes the game crash, but not doing so causes a warning, hence the pragma
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		std::wcin.clear();
		std::cin.clear();
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
	}
	std::cout << "Console allocated:" << std::endl;
}

/* Entry point of the module */
extern "C" {
	
	__declspec(dllexport) bool CheckVersion(const BYTE hash[16])
	{
		return ::memcmp(TARGET_HASH, hash, sizeof TARGET_HASH) == 0;
	}

	__declspec(dllexport) bool Initialize(HMODULE hMyModule, HMODULE hParentModule)
	{
		OpenConsole(); //debug console
		InitDiscord();
		DWORD old;

		::VirtualProtect((PVOID)text_Offset, text_Size, PAGE_EXECUTE_WRITECOPY, &old);
		s_origCBattleManager_OnCreate =
			TamperNearJmpOpr(CBattleManager_Creater, union_cast<DWORD>(CBattleManager_OnCreate));
		::VirtualProtect((PVOID)text_Offset, text_Size, old, &old);
		::VirtualProtect((PVOID)rdata_Offset, rdata_Size, PAGE_WRITECOPY, &old);
		s_origCBattleManager_OnDestruct =
			TamperDword(vtbl_CBattleManager + 0x00, union_cast<DWORD>(CBattleManager_OnDestruct));
		s_origCBattleManager_OnRender =
			TamperDword(vtbl_CBattleManager + 0x38, union_cast<DWORD>(CBattleManager_OnRender));
		s_origCBattleManager_OnProcess =
			TamperDword(vtbl_CBattleManager + 0x0c, union_cast<DWORD>(CBattleManager_OnProcess));
		::VirtualProtect((PVOID)rdata_Offset, rdata_Size, old, &old);
		::FlushInstructionCache(GetCurrentProcess(), NULL, 0);

		return true;
	}
}
/*
Line 282 in swrs.h may cause problems (__forceinline) and is commented out, however VS does not seem to care. 


OnCreate is called when entering character select menu.
OnDestruct is called when leaving the game, or upon re-entering the character select menu (which then calls OnCreate).

OnRender is called before OnProcess, and loops.

--- INSTALLATION OF YOUR MODULE ---
Add the path to your module in SWRSToys.ini, without the ";".

Compile manually with the following line:
g++ Template.cpp -shared -o Template.dll

g++ -std=c++11 *.cpp -shared -o test.dll -D_WIN32_WINNT=0x0501
*/