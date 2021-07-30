#ifndef OVERLAYAPI_H
#define OVERLAYAPI_H

#include <QString>
#include <windows.h>
#include <winerror.h>
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <winnt.h>
#include "renderloophook.h"
#include "../libedjournal/edjournal.h"

class DXWindow;
class QGuiApplication;
class QQuickRenderControl;
class QQmlEngine;
class QQmlComponent;
class QQuickItem;

class OverlayAPI {
private:
public:
    OverlayAPI(wchar_t* rootLibraryPath);
    ~OverlayAPI();

public:
    bool startHook();
    bool stopHook();
private:
    HRESULT doPresent(IDXGISwapChain* chain, UINT syncInterval, UINT flags);
    HRESULT doResize(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags);
    ULONG doDevRelease(IDXGIDevice* dev);
    ULONG doSwapChainRelease(IDXGISwapChain* chain);
    static HRESULT hookRedirect(IDXGISwapChain* chain, UINT syncInterval, UINT flags);
    static HRESULT hookResize(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags);
    static ULONG hookDevRelease(IDXGIDevice* dev);
    static ULONG hookSwapChainRelease(IDXGISwapChain* chain);
    static OverlayAPI* s_instance;

    QString m_libPath;
    DXWindow* m_window = 0;
    QGuiApplication* m_app = 0;
    ID3D11RenderTargetView* m_rtv = 0;
    ID3D11DeviceContext* m_context;
    ID3D11Device* m_device;
    QQuickRenderControl* m_renderControl;
    QQmlEngine* m_qmlEngine;
    QQmlComponent* m_qmlComponent;
    QQuickItem* m_rootItem;
    EDJournal* m_journal;

    bool m_quickDirty = false;

    WNDPROC m_gameWndProc;
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

extern "C" {
// Used in injector.cpp
__declspec(dllexport) void __cdecl injectOverlay();
__declspec(dllexport) void __cdecl unloadOverlay();
}

#endif // OVERLAYAPI_H
