#include "overlayapi.h"
#include <QQuickWindow>
#include <QGuiApplication>
#include "dxwindow.h"
#include <QQuickRenderControl>
#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickGraphicsDevice>
#include "../libedjournal/edjournal.h"
#include <PathCch.h>
#include "bootstrap.h"
#pragma comment(lib, "shlwapi.lib")

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
static OverlayAPI* overlay = 0;

void
injectOverlay()
{
    wchar_t injectorPath[MAX_PATH] = {0};
    GetModuleFileNameW((HINSTANCE)&__ImageBase, injectorPath, MAX_PATH);
    PathCchRemoveFileSpec(injectorPath, MAX_PATH);
    overlay = new OverlayAPI(injectorPath);
    overlay->startHook();
}

void
unloadOverlay()
{
    if (overlay) {
        overlay->stopHook();
        delete overlay;
    }
    FreeLibraryAndExitThread((HINSTANCE)&__ImageBase, 0);
}

// These need to exist for the duration of the QGuiApplication, but aren't used otherwise.
static int EMPTY_ARGC = 0;
static char* EMPTY_ARGV[1] = {0};

OverlayAPI::OverlayAPI(wchar_t* rootLibraryPath)
    : m_libPath(QString::fromWCharArray(rootLibraryPath))
{
    SetDllDirectoryW(rootLibraryPath); //FIXME: Do we really need this call?
    QCoreApplication::addLibraryPath(m_libPath);
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Direct3D11);
    QQuickWindow::setDefaultAlphaBuffer(true);
    s_instance = this;
}

OverlayAPI::~OverlayAPI()
{
    delete m_window;
    delete m_renderControl;
    delete m_app;
    if (m_device) {
        m_device->Release();
    }
    if (m_context) {
        m_context->Release();
    }
    s_instance = 0;
}

OverlayAPI* OverlayAPI::s_instance = 0;

HRESULT
OverlayAPI::hookRedirect(IDXGISwapChain* chain, UINT syncInterval, UINT flags) {
    return s_instance->doPresent(chain, syncInterval, flags);
}

HRESULT
OverlayAPI::hookResize(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags)
{
    return s_instance->doResize(pSwapChain, bufferCount, width, height, newFormat, flags);
}

ULONG
OverlayAPI::hookDevRelease(IDXGIDevice* dev)
{
    return s_instance->doDevRelease(dev);
}

ULONG
OverlayAPI::hookSwapChainRelease(IDXGISwapChain* chain)
{
    return s_instance->doSwapChainRelease(chain);
}

ULONG
OverlayAPI::doDevRelease(IDXGIDevice* dev)
{
    qDebug() << "device release" << dev;
    /*if (m_rtv) {
        m_rtv->Release();
    }
    m_rtv = 0;
    if (m_window) {
        m_window->releaseResources();
    }*/
    return 0;
}

ULONG
OverlayAPI::doSwapChainRelease(IDXGISwapChain* chain)
{
    //qDebug() << "swapchain release" << chain;
    /*if (m_rtv) {
        m_rtv->Release();
    }
    m_rtv = 0;
    if (m_window) {
        m_window->releaseResources();
    }*/
    return 0;
}

bool
OverlayAPI::startHook()
{
    return RenderLoopHook::instance()->activate(hookRedirect, hookResize, hookDevRelease, hookSwapChainRelease);
}

bool
OverlayAPI::stopHook()
{
    return RenderLoopHook::instance()->deactivate();
}

HRESULT
OverlayAPI::doResize(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags)
{
    qDebug() << "Resize!" << width << height;
    if (m_rtv) {
        m_rtv->Release();
    }
    m_rtv = 0;
    QSize newSize(width, height);
    if (width == 0 || height == 0) {
        DXGI_SWAP_CHAIN_DESC desc;
        RECT rect;
        pSwapChain->GetDesc(&desc);
        GetWindowRect(desc.OutputWindow, &rect);
        if (width == 0) {
            newSize.setWidth(rect.right - rect.left);
        }
        if (height == 0) {
            newSize.setHeight(rect.bottom - rect.top);
        }
    }
    QResizeEvent mappedEvent(newSize, m_window->size());
    QCoreApplication::sendEvent(m_window, &mappedEvent);
    //m_window->resize(QSize(width, height);)
    m_window->contentItem()->setSize(newSize);
    //m_rootItem->setSize(QSize(width, height));
    return 0;
}

LRESULT
OverlayAPI::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND targetWnd = (HWND)s_instance->m_window->winId();
    return SendMessage(targetWnd, uMsg, wParam, lParam);
    //return s_instance->m_gameWndProc(hWnd, uMsg, wParam, lParam);
}

HRESULT
OverlayAPI::doPresent(IDXGISwapChain* chain, UINT syncInterval, UINT flags)
{
    if (m_app == 0) {
        m_app = new QGuiApplication(EMPTY_ARGC, EMPTY_ARGV);
    }

    if (m_window == 0) {
        IDXGISwapChain2* upgradedChain = (IDXGISwapChain2*)chain;
        HRESULT res = upgradedChain->GetDevice(__uuidof(ID3D11Device), (PVOID*)&m_device);
        if (!SUCCEEDED(res)) {
            qWarning() << "Could not grab device!" << res;
            return res;
        }
        m_device->GetImmediateContext(&m_context);

        DXGI_SWAP_CHAIN_DESC desc;
        RECT rect;
        chain->GetDesc(&desc);
        GetWindowRect(desc.OutputWindow, &rect);
        const QSize initialSize = QSize(rect.right - rect.left, rect.bottom - rect.top);

        m_gameWndProc = (WNDPROC)SetWindowLongPtr(desc.OutputWindow, GWLP_WNDPROC, (LONG_PTR)wndProc);

        m_renderControl = new QQuickRenderControl();

        QObject::connect(m_renderControl, &QQuickRenderControl::renderRequested, [this] {m_quickDirty = true;});
        QObject::connect(m_renderControl, &QQuickRenderControl::sceneChanged, [this] { m_quickDirty = true;});

        m_window = new DXWindow(m_renderControl, m_device, m_context);
        m_window->setGraphicsDevice(QQuickGraphicsDevice::fromDeviceAndContext(m_device, m_context));

        m_renderControl->initialize();
        QColor transparent;
        transparent.setAlphaF(0.0);
        m_window->setColor(transparent);
        QSurfaceFormat alphaFormat(m_window->format());
        alphaFormat.setAlphaBufferSize(8);
        m_window->setFormat(alphaFormat);

        m_qmlEngine = new QQmlEngine;
        m_journal = new EDJournal;

        m_qmlEngine->rootContext()->setContextProperty("Journal", m_journal);
        qDebug() << "Adding" << m_libPath << "to imports";
        m_qmlEngine->addPluginPath(m_libPath);
        m_qmlEngine->addImportPath(m_libPath);
        //m_qmlComponent = new QQmlComponent(m_qmlEngine, QUrl(QLatin1String("qrc:/main.qml")));
        m_qmlComponent = new QQmlComponent(m_qmlEngine, QUrl::fromLocalFile(QLatin1String("C:/Users/tdfischer/Documents/EDNavComputer/libnavgui/main.qml")));
        if (m_qmlComponent->isError()) {
            for (const QQmlError &error : m_qmlComponent->errors())
                qWarning() << error.url() << error.line() << error;
        }
        QObject* rootObject = m_qmlComponent->create();
        m_rootItem = qobject_cast<QQuickItem*>(rootObject);
        m_window->setGeometry(0, 0, initialSize.width(), initialSize.height());

        m_rootItem->setParentItem(m_window->contentItem());
    }

    if (m_rtv == 0) {
        ID3D11Texture2D* pBackBuffer;
        chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        m_device->CreateRenderTargetView(pBackBuffer, NULL, &m_rtv);
        pBackBuffer->Release();
        //m_window->contentItem()->setSize(m_window->size());
        qDebug() << "RTV ready!";
    }

    m_app->processEvents();
    if (m_window != 0) {
        m_quickDirty = true;

        if (m_quickDirty) {
            m_renderControl->polishItems();
            m_renderControl->beginFrame();
            m_renderControl->sync();
            m_renderControl->render();
            m_renderControl->endFrame();
            m_quickDirty = false;
        }

        m_context->OMSetRenderTargets(1, &m_rtv, NULL);
        m_window->render();
    }
    return 0;
}
