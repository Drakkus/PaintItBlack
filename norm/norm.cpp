#include "stdafx.h"
#include "detours.h"
#include "norm.h"

#include "hook_chat.h"
#include "hook_renderer.h"
#include "hook_socket.h"
#include "hook_session.h"
#include "hook_dx.h"
#include "hook_gamemode.h"

#include "mod_overlay.h"
#include "mod_statistics.h"
#include "mod_timestamp.h"
#include "mod_overlay_new.h"

#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

// Installs a mod based on the classname.
#define INSTALL_MOD(modname)	mods.push_back(std::make_shared<modname>(this));//auto m_##modname = std::make_shared<modname>(this); \
								/* m_##modname->register_hooks(); */ mods.push_back(m_##modname);
								

/* dll class */
namespace norm_dll {

norm::norm()
{
	//dbg_sock->do_send("Releasing norm object");
	//for (auto it : mods)
	//	it.reset();
}

void norm::install_mods()
{
	// Disable a mod by commenting out the specific line.
	//INSTALL_MOD(overlay);
	INSTALL_MOD(statistics);
	INSTALL_MOD(timestamp);
	INSTALL_MOD(overlay_new);
}

void norm::start()
{
	char info_buf[256];
	/* Connect to the debug socket */
	dbg_sock = std::make_shared<debug_socket>();
	//c_state = std::make_shared<state>(dbg_sock);
	int err = dbg_sock->do_connect();
	if (err != 0) {
		MessageBox(0, (LPCWSTR)"Unable to connect to the debug socket!", (LPCWSTR)"norm.dll error!", MB_OK);
		dbg_sock->disabled = 1;
	}
	dbg_sock->do_send("Hello Debugger!");

	/* Hook functions. */
	err = DetourTransactionBegin();
	CHECK(info_buf, err);
	dbg_sock->do_send(info_buf);

	err = DetourUpdateThread(GetCurrentThread());
	CHECK(info_buf, err);
	dbg_sock->do_send(info_buf);

	// attach hooks and install mods
	int total_hooks = 0;
	auto sptr = shared_from_this();
	total_hooks += chat_detour(sptr);
	total_hooks += socket_detour(sptr);
	total_hooks += renderer_detour(sptr);
	total_hooks += session_detour(sptr);
	total_hooks += dx_detour(sptr);
	total_hooks += gamemode_detour(sptr);

	this->install_mods();

	err = DetourTransactionCommit();
	CHECK(info_buf, err);
	dbg_sock->do_send(info_buf);

	sprintf_s(info_buf, "Total hooks available: %d", total_hooks);
	dbg_sock->do_send(info_buf);
}


norm::~norm()
{
}
}