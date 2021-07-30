#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include "libedjournal/edjournal.h"

#include <windows.h>
#include <QDir>
#include "libnavgui/gui.h"

void inject(DWORD pid) {
    QString injectLibraryPath = QDir::toNativeSeparators(QDir("injector").absoluteFilePath("injector-shim.dll"));
    if (!QFile(injectLibraryPath).exists()) {
        qDebug() << "Could not find" << injectLibraryPath;
        return;
    }
    qDebug() << "Injecting" << injectLibraryPath.toUtf8() << "into" << pid;
    wchar_t wPath[1024];
    memset(wPath, 0, sizeof(wchar_t) * 1024);
    injectLibraryPath.toWCharArray(wPath);
    OutputDebugStringW(wPath);
    HANDLE procHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);
    if (procHandle == NULL) {
        qDebug() << "Failed to open process";
        return;
    }

    BOOL is32Bit;
    IsWow64Process(procHandle, &is32Bit);
    if (is32Bit) {
        qDebug() << "Process is actually 32 bits!";
        return;
    }

    int bytesToAlloc = (1 + lstrlenW(wPath)) * sizeof(WCHAR);
    void* remoteBuf = (void*)VirtualAllocEx(procHandle, NULL, bytesToAlloc, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (WriteProcessMemory(procHandle, remoteBuf, wPath, bytesToAlloc, NULL) == 0) {
        qDebug() << "Failed to write to process memory";
        return;
    }

    void* loadLibFunc = (void*)GetProcAddress(GetModuleHandle(L"kernel32"), "LoadLibraryW");
    if (loadLibFunc == NULL) {
        qDebug() << "Failed to GetProcAddress";
        return;
    }
    HANDLE remoteThread = CreateRemoteThread(procHandle, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibFunc, (LPVOID)remoteBuf, 0, NULL);
    if (remoteThread == NULL) {
        qDebug() << "Failed to create remote thread";
        return;
    } else {
        qDebug() << "Injected at" << loadLibFunc;
    }
}

int main(int argc, char *argv[])
{
    //Temporary testing version of main() that only injects the app
    //HWND windowHandle = FindWindow(NULL, L"Elite - Dangerous (CLIENT)");
    HWND windowHandle = FindWindow(NULL, L"MultithreadedRendering11");
    DWORD pid;
    GetWindowThreadProcessId(windowHandle, &pid);
    inject(pid);
    return 0;
#if 0
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine* engine = makeGuiEngine();

    qDebug() << QQuickWindow::graphicsApi();
    return app.exec();
#endif
}
