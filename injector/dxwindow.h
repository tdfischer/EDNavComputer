#ifndef DXWINDOW_H
#define DXWINDOW_H

#include <QQuickWindow>
#include <dxgi1_3.h>
#include <d3d11_1.h>

class QExposeEvent;
class QMouseEvent;
struct ID3D11Device;
struct ID3D11DeviceContext;
// Possible setup:
// Create Window object in its own thread
class DXWindow : public QQuickWindow
{
    Q_OBJECT
public:
    DXWindow(QQuickRenderControl* renderControl, ID3D11Device* device, ID3D11DeviceContext* context);
    ~DXWindow();
    void render();
    bool releaseResources();
protected:
    void exposeEvent(QExposeEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    bool event(QEvent* e) override;
private:
    bool initResources();
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;

    struct {
            bool valid = false;
            ID3D11VertexShader *vertexShader = nullptr;
            ID3D11PixelShader *pixelShader = nullptr;
            ID3D11Texture2D *texture = nullptr;
            ID3D11ShaderResourceView *textureSrv = nullptr;
            ID3D11SamplerState *sampler = nullptr;
            ID3D11BlendState *blendState = nullptr;
        } m_res;
};

#endif // DXWINDOW_H
