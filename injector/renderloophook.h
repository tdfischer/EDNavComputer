#ifndef RENDERLOOPHOOK_H
#define RENDERLOOPHOOK_H

#include <functional>
#include <dxgi1_3.h>

class RenderLoopHook
{
public:
    typedef HRESULT(__fastcall *IDXGISwapChainPresent)(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags);
    typedef HRESULT(__fastcall *IDXGISwapChainResizeBuffers)(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags);
    typedef ULONG(__fastcall *IDXGIDeviceRelease)(IDXGIDevice* pDevice);
    typedef ULONG(__fastcall *IDXGISwapChainRelease)(IDXGISwapChain* pSwapChain);
    bool activate(IDXGISwapChainPresent targetPresent, IDXGISwapChainResizeBuffers targetResize, IDXGIDeviceRelease targetDevRelease, IDXGISwapChainRelease targetChainRelease);
    bool deactivate();
    static RenderLoopHook* instance();

private:
    RenderLoopHook();
    ~RenderLoopHook();
    bool hookDX11();
    static RenderLoopHook* s_instance;
    static HRESULT __fastcall hookedPresent(IDXGISwapChain *pChain, UINT SyncInterval, UINT Flags);
    static HRESULT __fastcall hookedResize(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags);
    static ULONG __fastcall hookedDeviceRelease(IDXGIDevice *pDevice);
    static ULONG __fastcall hookedSwapChainRelease(IDXGISwapChain *pDevice);
    IDXGISwapChainPresent m_targetPresent;
    IDXGISwapChainPresent m_originalPresent;

    IDXGISwapChainResizeBuffers m_targetResize;
    IDXGISwapChainResizeBuffers m_originalResize;

    IDXGIDeviceRelease m_targetDevRelease;
    IDXGIDeviceRelease m_originalDevRelease;

    IDXGISwapChainRelease m_targetSwapChainRelease;
    IDXGISwapChainRelease m_originalSwapChainRelease;
};

#endif // RENDERLOOPHOOK_H
