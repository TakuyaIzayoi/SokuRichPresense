#define SWRS_USES_HASH

//C-Related Stuff
#include <cstring>
#include <cstdlib>
#include <string>
#include <ctime>

#include <string.h>
#include <windows.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <iostream>
#include <d3d9.h>
#include <vector>
#include <sstream>
#include <map>

//Soku Related Items
#include "swrs.h"
#include "fields.h"
#include "address.h"

#include "discord_register.h"
#include "discord_rpc.h"

#include "sokuRichPresence.h"

// Original function calls, needed to create a new game, render, process game loop and destroy game.
#define CBattleManager_Create(p) \
  Ccall(p, s_origCBattleManager_OnCreate, void*, ())()
#define CBattleManager_Render(p) \
  Ccall(p, s_origCBattleManager_OnRender, void, ())()
#define CBattleManager_Process(p) \
  Ccall(p, s_origCBattleManager_OnProcess, int, ())()
#define CBattleManager_Destruct(p, dyn) \
  Ccall(p, s_origCBattleManager_OnDestruct, void*, (int))(dyn);

// These hold the reference to the instruction memory where the original game function lives. 
// unsigned long long stands for the DWORD (which is 4bytes for some reason on this machine)
static unsigned long long s_origCBattleManager_OnCreate;
static unsigned long long s_origCBattleManager_OnDestruct;
static unsigned long long s_origCBattleManager_OnRender;
static unsigned long long s_origCBattleManager_OnProcess;



#define ADDR_BMGR_P1 0x0C
#define ADDR_BMGR_P2 0x10


// OnCreate is called when entering character select menu.
// OnDestruct is called when leaving the game, or upon re-entering the character select menu (which then calls OnCreate).

// OnRender is called before OnProcess, and loops.






void* __fastcall CBattleManager_OnCreate(void *This) {
	CBattleManager_Create(This);
	std::cout << "OnCreate Called: "<< sizeof(void* (C::*)()) << std::endl;
	
	
	InitDiscord();
	NewPresence();
	
	
	if (g_mainMode == SWRSMODE_VSCLIENT)
	{
		discordPresence.details = "VS Network (P1)";
		p1name = g_pprofP1; //gets player name info in netplay.
		p2name = g_pprofP2; //gets player name info in netplay.
	}
	else if (g_mainMode == SWRSMODE_VSSERVER)
	{
		discordPresence.details = "VS Network (P2)";
		p1name = g_pprofP1; //gets player name info in netplay.
		p2name = g_pprofP2; //gets player name info in netplay.
	}
	else if (g_mainMode == SWRSMODE_VSWATCH)
	{
		discordPresence.details = "Spectating";
		p1name = g_pprofP1; //gets player name info in netplay.
		p2name = g_pprofP2; //gets player name info in netplay.
	}

	else if (g_mainMode == SWRSMODE_PRACTICE)
	{
		discordPresence.details = "Practice Mode";
		// p1name = g_profP1; //gets player name info in netplay.
		// p2name = g_pprofP2; //gets player name info in netplay.
	}
	
	else
	{
		discordPresence.details = "Some Other Gamemode";
		// p1name = g_profP1; //gets player name info in non netplay.
	}
	// discordPresence.state = "Character Select";
	

	
	
	Discord_UpdatePresence(&discordPresence);
	
	
	return This;
}

	//loops 
void __fastcall CBattleManager_OnRender(void *This) {
	CBattleManager_Render(This);
	


}

	//fights stuff? in battle.
int __fastcall CBattleManager_OnProcess(void *This) {
	int ret;
	ret = CBattleManager_Process(This);
	int battleManager = ACCESS_INT(ADDR_BATTLE_MANAGER, 0);
	

	
	
	// discordPresence.state = "Yosu (Marisa)";
    // discordPresence.details = "Thiena (Aya)";	
	// discordPresence.startTimestamp = time(NULL);
    // Discord_UpdatePresence(&discordPresence);

	// Get address to the player data based on data inside "Battle Manager"
	void* p1 = ACCESS_PTR(battleManager, ADDR_BMGR_P1);	
	void* p2 = ACCESS_PTR(battleManager, ADDR_BMGR_P2);
	
	p1char = ACCESS_CHAR(p1, CF_CHARACTER_INDEX) + 65; //starts from the letter A.
	p2char = ACCESS_CHAR(p2, CF_CHARACTER_INDEX) + 65; //starts from the letter A.

	// if (g_sceneId == SWRSSCENE_SELECT)
	// {
		// discordPresence.state = "Character Select";
	// }
	if (g_sceneId == SWRSSCENE_BATTLE && mutex == 0) //will be your mutex for combat.
	{
		// discordPresence.state = "In Combat";
		discordPresence.startTimestamp = time(NULL);
		mutex = 1;
		SendDiscordLocalRP();
	}

	// if (g_sceneId != SWRSSCENE_BATTLE && mutex == 1) //will be your mutex for combat.
	// {
		// mutex = 0;
		// // discordPresence.startTimestamp = (NULL);
		
		
		// NewPresence();
		// Discord_UpdatePresence(&discordPresence);
		// // SendDiscordLocalRP();
	// }

/* HEALTH DISPLAY */
	//ACCESS_<variable_type>() is both used for accessing and writing to the resource.
	//Character variables are in fields.h l.1 "Character class"
	// short p1Health = ACCESS_SHORT(p1, CF_CURRENT_HEALTH);
	// short p1Spirit = ACCESS_SHORT(p1, CF_CURRENT_SPIRIT);
	// short p2Health = ACCESS_SHORT(p2, CF_CURRENT_HEALTH);
	// short p2Spirit = ACCESS_SHORT(p2, CF_CURRENT_SPIRIT);
	

	//Press CTRL, effect visible in VS mode, as practice mode hardsets HP 
	if (GetKeyState(VK_CONTROL) & 0x8000) {
		// std::cout << "P1: " << p1Health << "(" << (float) (p1Spirit) / 200 << ")" << wau << nameWau
			  // << " [VS] P2: " << p2Health << "(" << (float) (p2Spirit) / 200 << ")" << std::endl;
		// ACCESS_SHORT(p1, CF_CURRENT_HEALTH) = 5000;
		if (g_mainMode == SWRSMODE_VSSERVER || g_mainMode == SWRSMODE_VSCLIENT || g_mainMode == SWRSMODE_VSWATCH) //Versus Network Online Play.
		{
			std::cout << p1name << " " << obtainChar(p1char) << " VS " << p2name << " " << (obtainChar(p2char)) << std::endl;
			SendDiscordNetplayRP();
		}
		if (g_mainMode == SWRSMODE_PRACTICE)
		{
			std::cout << obtainChar(p1char) << std::endl;
			SendDiscordLocalRP();
		}
	}


// /* GRAZING */
	// void* p1FrameData = ACCESS_PTR(p1, CF_CURRENT_FRAME_DATA);
	// void* p2FrameData = ACCESS_PTR(p2, CF_CURRENT_FRAME_DATA);
	// int p1FrameFlag = ACCESS_INT(p1FrameData, FF_FFLAGS);
	// int p2FrameFlag = ACCESS_INT(p2FrameData, FF_FFLAGS);

	// // Flags are in fields.h, l.99 "Frame flags"
	// if (p1FrameFlag & FF_GRAZE) //binary operation on a bit to check if p1 is grazing or not.
		// std::cout << "-";
	// else
		// std::cout << " ";


	return ret;
}


void* __fastcall CBattleManager_OnDestruct(void *This, int mystery, int dyn) {
	void* ret;
	ret = CBattleManager_Destruct(This, dyn);
	std::cout << "OnDestruct Called" << std::endl;
	
	// NewPresence();
	// discordPresence.details = "Character Select";
	// Discord_UpdatePresence(&discordPresence);
	Discord_Shutdown();

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
		
		// InitDiscord();
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

g++ -std=c++11 *.cpp -shared -D_WIN32_WINNT=0x0501 -o SokuRP.dll
*/

///////Personal Crap

//determines char based on the array.


std::string obtainChar(char iconKey)
{
	return Characters[iconKey];
}

void SendDiscordNetplayRP()
{	
		
		// std::cout << "Start making string" << std::endl;
		
		std::string wau;
		std::string charName;
		// wau << p1name << " " << obtainChar(p1char) << " VS " << p2name << " " << (obtainChar(p2char)) << std::endl;
		// wau.append(p1name);
		// wau.append(" ");
		charName = (obtainChar(p1char));
		wau = wau + charName;
		wau.append(" VS ");
		// wau.append(p2name);
		// wau.append(" ");
		wau = wau + (obtainChar(p2char));
		
		
		// std::cout << "Appended" << std::endl;
		
		// char* cstr = char[128];
		std::strcpy(cstr, wau.c_str());
		std::strcpy(iconKey, wau.c_str());
		iconKey[0] = iconKey[0]+32;
		discordPresence.state = cstr;
		
		std::strcpy(cstr, charName.c_str());
		discordPresence.largeImageText = cstr;
		// cstr[0]=cstr[0]+32;
		discordPresence.largeImageKey = iconKey;
		Discord_UpdatePresence(&discordPresence);
		// memset(cstr,0,strlen(cstr));
		
		
		// std::cout << "Sent!" << std::endl;
}

void SendDiscordLocalRP()
{	
		
		// std::cout << "Start making string" << std::endl;
		
		std::string wau;
		// std::string charName;
		// wau << p1name << " " << obtainChar(p1char) << " VS " << p2name << " " << (obtainChar(p2char)) << std::endl;		
		
		wau = (obtainChar(p1char));
		// wau = wau + charName;

		std::strcpy(cstr, wau.c_str());
		std::strcpy(iconKey, wau.c_str());
		discordPresence.state = cstr;
		discordPresence.largeImageText = cstr;
		iconKey[0]=iconKey[0]+32;
		discordPresence.largeImageKey = iconKey;
		Discord_UpdatePresence(&discordPresence);
		// memset(cstr,0,strlen(cstr));
		
		
		// std::cout << "Sent!" << std::endl;
}

void InitDiscord()
{
    memset(&handlers, 0, sizeof(handlers));
    // handlers.ready = handleDiscordReady;
    // handlers.errored = handleDiscordError;
    // handlers.disconnected = handleDiscordDisconnected;
    // handlers.joinGame = handleDiscordJoinGame;
    // handlers.spectateGame = handleDiscordSpectateGame;
    // handlers.joinRequest = handleDiscordJoinRequest;

    // Discord_Initialize(const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId, int pipe)
    Discord_Initialize("570515970381185024", &handlers, 1, "1234");
	
	
	memset(&discordPresence, 0, sizeof(discordPresence));
}

	//sends the update to the application/discord
	
static void NewPresence()
{;

	memset(&discordPresence, 0, sizeof(discordPresence));
    // discordPresence.state = "Yosu (Marisa)";
    // discordPresence.details = "Thiena (Aya)";
    discordPresence.largeImageKey = "sokuicon";
    // discordPresence.largeImageText = "placeholder";
    // discordPresence.partyId = "ae488379-351d-4a4f-ad32-2b9b01c91657";
    // discordPresence.spectateSecret = "MTIzNDV8MTIzNDV8MTMyNDU0"; //can prob use for spectating games. WOW
    // discordPresence.joinSecret = "MTI4NzM0OjFpMmhuZToxMjMxMjM= ";//can prob be used for hosting games.
    Discord_UpdatePresence(&discordPresence);
}