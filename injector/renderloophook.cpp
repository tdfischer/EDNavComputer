#include "renderloophook.h"

#include "../detours/detours.h"
#include <d3d11.h>
#include <QDebug>

RenderLoopHook::RenderLoopHook()
    : m_targetPresent(0),
      m_originalPresent(0),
      m_targetResize(0),
      m_originalResize(0),
      m_targetDevRelease(0),
      m_originalDevRelease(0),
      m_targetSwapChainRelease(0),
      m_originalSwapChainRelease(0)
{
    s_instance = this;
}

RenderLoopHook::~RenderLoopHook()
{
    deactivate();
    s_instance = 0;
}

RenderLoopHook* RenderLoopHook::s_instance = 0;

RenderLoopHook*
RenderLoopHook::instance()
{
    if (s_instance == 0) {
        s_instance = new RenderLoopHook();
    }
    return s_instance;
}

bool
RenderLoopHook::deactivate()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(LPVOID&)m_originalPresent, (PBYTE)m_targetPresent);
    DetourDetach(&(LPVOID&)m_originalResize, (PBYTE)m_targetResize);
    DetourDetach(&(LPVOID&)m_originalDevRelease, (PBYTE)m_targetDevRelease);
    DetourDetach(&(LPVOID&)m_originalSwapChainRelease, (PBYTE)m_targetSwapChainRelease);
    DetourTransactionCommit();
    return true;
}

bool
RenderLoopHook::activate(IDXGISwapChainPresent target, IDXGISwapChainResizeBuffers resizeTarget, IDXGIDeviceRelease devReleaseTarget, IDXGISwapChainRelease swapchainReleaseTarget)
{
    m_targetPresent = target;
    m_targetResize = resizeTarget;
    m_targetDevRelease = devReleaseTarget;
    m_targetSwapChainRelease = swapchainReleaseTarget;
    //TODO: DX12, DX10 injectors
    return hookDX11();
}

HRESULT
RenderLoopHook::hookedPresent(IDXGISwapChain *pChain, UINT SyncInterval, UINT Flags)
{
    s_instance->m_targetPresent(pChain, SyncInterval, Flags);
    return s_instance->m_originalPresent(pChain, SyncInterval, Flags);
}

HRESULT
RenderLoopHook::hookedResize(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags)
{
    s_instance->m_targetResize(pSwapChain, bufferCount, width, height, newFormat, flags);
    return s_instance->m_originalResize(pSwapChain, bufferCount, width, height, newFormat, flags);
}

ULONG
RenderLoopHook::hookedDeviceRelease(IDXGIDevice* dev)
{
    s_instance->m_targetDevRelease(dev);
    return s_instance->m_originalDevRelease(dev);
}

ULONG
RenderLoopHook::hookedSwapChainRelease(IDXGISwapChain* chain)
{
    s_instance->m_targetSwapChainRelease(chain);
    return s_instance->m_originalSwapChainRelease(chain);
}

typedef HRESULT (WINAPI *D3D11CreateDevice_t)(
    IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL *,
    UINT, UINT, ID3D11Device **, D3D_FEATURE_LEVEL *, ID3D11DeviceContext **);
typedef HRESULT (WINAPI *D3D11CreateDeviceAndSwapChain_t)(
    IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL *,
    UINT, UINT, const DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **,
    ID3D11Device **, D3D_FEATURE_LEVEL *, ID3D11DeviceContext **);
static D3D11CreateDevice_t D3D11CreateDevicePtr = NULL;
static D3D11CreateDeviceAndSwapChain_t D3D11CreateDeviceAndSwapChainPtr = NULL;

bool
RenderLoopHook::hookDX11()
{
    qDebug() << "Grabbing d3d11.dll";
    HMODULE module = GetModuleHandle(TEXT("d3d11.dll"));
    D3D11CreateDevicePtr = (D3D11CreateDevice_t)GetProcAddress(module, "D3D11CreateDevice");
    D3D11CreateDeviceAndSwapChainPtr = (D3D11CreateDeviceAndSwapChain_t)GetProcAddress(module, "D3D11CreateDeviceAndSwapChain");
    qDebug() << "Grabbed CreateDevice pointers";

    UINT flags = D3D10_CREATE_DEVICE_SINGLETHREADED;
    ID3D11Device *device = NULL;
    ID3D11DeviceContext *context = NULL;
    IDXGISwapChain *chain = NULL;
    D3D_FEATURE_LEVEL featureLvl = D3D_FEATURE_LEVEL_11_0;

    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"EDNav";
    RegisterClass(&wc);

    HWND dummyWindow = CreateWindow(L"EDNav",L"EDNav", WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1, NULL, NULL, GetModuleHandle(NULL), NULL);
    qDebug() << "Created dummy window";

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    memset(&swapChainDesc, 0, sizeof(swapChainDesc));
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering =
        DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1; // No anti-aliasing
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.OutputWindow = dummyWindow;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    qDebug() << "Grabbing Present pointer...";
    HRESULT res = D3D11CreateDeviceAndSwapChainPtr(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, NULL, 0, D3D11_SDK_VERSION, &swapChainDesc, &chain, &device, &featureLvl, &context);
    if (SUCCEEDED(res)) {
        void **vtable = *(void ***)chain;
        void **devVtable = *(void ***)device;
        m_originalPresent = (IDXGISwapChainPresent)vtable[8];
        m_originalResize = (IDXGISwapChainResizeBuffers)vtable[13];
        m_originalDevRelease = (IDXGIDeviceRelease)devVtable[2];
        m_originalSwapChainRelease = (IDXGISwapChainRelease)vtable[2];
        OutputDebugString(L"Cleaning up the scaffolding...");
        chain->Release();
        context->Release();
        device->Release();

        qDebug() << "Initiating detour...";

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(LPVOID&)m_originalPresent, (PBYTE)hookedPresent);
        DetourAttach(&(LPVOID&)m_originalResize, (PBYTE)hookedResize);
        DetourAttach(&(LPVOID&)m_originalDevRelease, (PBYTE)hookedDeviceRelease);
        DetourAttach(&(LPVOID&)m_originalSwapChainRelease, (PBYTE)hookedSwapChainRelease);
        DetourTransactionCommit();
        qDebug() << "Swapped!";
    } else {
        char msgbuf[255];
        qWarning() << "Couldn't create a DX11 device. Error:" << QString("%1").arg(res, 0, 16);
    }
    DestroyWindow(dummyWindow);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return SUCCEEDED(res);
}
