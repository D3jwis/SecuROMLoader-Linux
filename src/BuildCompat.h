#pragma once
#include "minhook/MinHook.h"
// MSVC accepts function pointers where MinHook expects LPVOID. Clang requires an explicit cast.
#ifdef __clang__
template<typename T, typename F> inline MH_STATUS WINAPI MH_CreateHook(T target, F detour, LPVOID* original)
{ return MH_CreateHook(reinterpret_cast<LPVOID>(target), reinterpret_cast<LPVOID>(detour), original); }
template<typename F> inline MH_STATUS WINAPI MH_CreateHookApi(LPCWSTR module, LPCSTR name, F detour, LPVOID* original)
{ return MH_CreateHookApi(module, name, reinterpret_cast<LPVOID>(detour), original); }
template<typename F> inline MH_STATUS WINAPI MH_DisableHook(F target)
{ return MH_DisableHook(reinterpret_cast<LPVOID>(target)); }
#endif
