#pragma once
#include "debug_socket.h"
#include "norm.h"
#include "hook.h"

/* hooks all available functions. */
int gamemode_detour(std::shared_ptr<norm_dll::norm> c_state);

bool initialize_called();

#if CLIENT_VER == 20150000
typedef void(__thiscall *CGameMode__Initialize)(void*);
typedef void(__thiscall *CGameMode__OnInit)(void*, const char *);
typedef void(__thiscall *CGameMode__Zc_Npcack_Mapmove)(void*, const char *);
#endif