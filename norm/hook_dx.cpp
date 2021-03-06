#include "stdafx.h"
#include "norm.h"
#include "hook_dx.h"
#include "detours.h"
#include "client_ver.h"
#include "CProxyIDirectDraw7.h"

static std::shared_ptr<norm_dll::norm> c_state;
CProxyIDirectDraw7 *lpcDD;

#if (CLIENT_VER == 20180620 || CLIENT_VER == 20180621)
DWORD DirectDrawCreateEx_addr = 0x00A58D04;
#define DX
#elif CLIENT_VER == 20150000
DWORD DirectDrawCreateEx_addr = 0x0096c42a;
#define DX
#endif
typedef HRESULT(WINAPI *pDirectDrawCreateEx)(GUID *lpGuid, LPVOID *lplpDD, const IID *const iid, IUnknown *pUnkOuter);

HRESULT WINAPI DirectDrawCreateEx_hook(GUID *lpGuid, LPVOID *lplpDD, const IID *const iid, IUnknown *pUnkOuter)
{
	c_state->dbg_sock->do_send("DirectDrawCreateEx called!");

	HRESULT Result = ((pDirectDrawCreateEx)DirectDrawCreateEx_addr)(lpGuid, lplpDD, iid, pUnkOuter);
	if (FAILED(Result))
		return Result;

	*lplpDD = lpcDD = new CProxyIDirectDraw7((IDirectDraw7*)*lplpDD, c_state);
	lpcDD->setThis(lpcDD);
	c_state->dbg_sock->do_send("DirectDrawCreateEx called! Fin");
	return Result;
}

int dx_detour(std::shared_ptr<norm_dll::norm> state_) {
	int err = 0;
	int hook_count = 0;
	char info_buf[256];
	c_state = state_;

#ifdef DX
	err = DetourAttach(&(LPVOID&)DirectDrawCreateEx_addr, DirectDrawCreateEx_hook);
	CHECK(info_buf, err);
	c_state->dbg_sock->do_send(info_buf);
	hook_count++;
#endif

	sprintf_s(info_buf, "DX hooks available: %d", hook_count);
	c_state->dbg_sock->do_send(info_buf);

	return hook_count;
}

