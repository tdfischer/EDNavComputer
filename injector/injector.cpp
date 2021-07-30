#include <windows.h>
#include "bootstrap.h"
#include <PathCch.h>

// Tiny shim that allows us to load the real injector using required dll search paths

typedef void(__cdecl* OverlayInjectorFunc)();
typedef void(__cdecl* OverlayUnloadFunc)();
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

int startInjection()
{
    wchar_t injectorPath[MAX_PATH] = {0};
    GetModuleFileNameW((HINSTANCE)&__ImageBase, injectorPath, MAX_PATH);
    PathCchRemoveFileSpec(injectorPath, MAX_PATH);
    PathCchAppend(injectorPath, MAX_PATH, L"injector.dll");

    HMODULE injectionModule = GetModuleHandleW(injectorPath);
    if (injectionModule) {
        OutputDebugString("Injector already loaded! Attempting to unload instead...");
        OverlayUnloadFunc unloaderFunc = (OverlayUnloadFunc)GetProcAddress(injectionModule, "unloadOverlay");
        if (unloaderFunc) {
            unloaderFunc();
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)unloaderFunc, NULL, NULL, NULL);
        } else {
            OutputDebugString("Failed to unload overlay!");
        }
        FreeLibraryAndExitThread((HINSTANCE)&__ImageBase, 0);
        return 0;
    }

    injectionModule = LoadLibraryExW(injectorPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!injectionModule) {
        OutputDebugString("EDNavComputer injector load error!");
        return 1;
    }

    OverlayInjectorFunc injectorFunc = (OverlayInjectorFunc)GetProcAddress(injectionModule, "injectOverlay");
    if (injectorFunc) {
        OutputDebugString("Injection successful. Booting navigation computer...");
        //injectorFunc();
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) injectorFunc, NULL, NULL, NULL);
        FreeLibraryAndExitThread((HINSTANCE)&__ImageBase, 0);
        return 0;
    } else {
        OutputDebugString("Failed to find injectOverlay function in injector. Did the API break??");
        FreeLibraryAndExitThread((HINSTANCE)&__ImageBase, 1);
        return 1;
    }
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDLL);
            OutputDebugStringA("EDNavComputer shim was successfully injected. Running bootloader...");
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)startInjection, NULL, NULL, NULL);
        break;
    }
    return TRUE;
}
