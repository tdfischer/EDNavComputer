#include "dxwindow.h"

#include <QWindow>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QQuickItem>
#include <QQuickGraphicsDevice>
#include <QQuickRenderControl>
#include <QQuickRenderTarget>
#include <d3d11.h>

#include "quad.frag.h"
#include "quad.vert.h"

#include <QFile>

#include "../libedjournal/edjournal.h"
#include <QQmlContext>

DXWindow::DXWindow(QQuickRenderControl* rendercontrol, ID3D11Device* device, ID3D11DeviceContext* context)
    : QQuickWindow(rendercontrol)
{
    m_device = device;
    m_context = context;
    m_device->AddRef();
    m_context->AddRef();
}

DXWindow::~DXWindow()
{
    releaseResources();
    m_device->Release();
    m_context->Release();
}

void
DXWindow::exposeEvent(QExposeEvent*)
{
    if (isExposed()) {
        render();
    }
}

void
DXWindow::resizeEvent(QResizeEvent* e)
{
    qDebug() << "Resize event!";
    releaseResources();
    QQuickWindow::resizeEvent(e);
}

bool
DXWindow::event(QEvent* e)
{
    // These should be executed on the game's
    switch(e->type()) {
        case QEvent::UpdateRequest:
        render();break;
    case QEvent::PlatformSurface:
        //destroy swapchain??
        break;
    default:break;
    }
    return QWindow::event(e);
}

bool
DXWindow::releaseResources()
{
    qDebug() << "Release!";
    if (!m_res.valid) {
        return true;
    }
    m_res.vertexShader->Release();
    m_res.pixelShader->Release();
    m_res.texture->Release();
    m_res.textureSrv->Release();
    m_res.sampler->Release();
    m_res.blendState->Release();
    m_res.valid = false;
    return true;
}

bool
DXWindow::initResources()
{
    HRESULT hr = m_device->CreateVertexShader(g_quad_vs_main, sizeof(g_quad_vs_main), nullptr, &m_res.vertexShader);
    if (FAILED(hr)) {
        qWarning("Failed to create vertex shader!");
        return false;
    }

    hr = m_device->CreatePixelShader(g_quad_ps_main, sizeof(g_quad_ps_main), nullptr, &m_res.pixelShader);
    if (FAILED(hr)) {
        qWarning("Failed to create pixel shader");
        return false;
    }

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width();
    texDesc.Height = height();
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;

    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texDesc.Usage = D3D11_USAGE_DEFAULT;

    hr = m_device->CreateTexture2D(&texDesc, nullptr, &m_res.texture);
    if (FAILED(hr)) {
        qWarning("Failed to create texture");
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = m_device->CreateShaderResourceView(m_res.texture, &srvDesc, &m_res.textureSrv);
    if (FAILED(hr)) {
        qWarning("Failed to create srv");
        return false;
    }

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.MaxAnisotropy = 1.0f;

    hr = m_device->CreateSamplerState(&sampDesc, &m_res.sampler);
    if (FAILED(hr)) {
        qWarning("Failed to create sampler state");
        return false;
    }

    D3D11_BLEND_DESC blendDesc;
    memset(&blendDesc, 0, sizeof(blendDesc));
    blendDesc.IndependentBlendEnable = true;
    D3D11_RENDER_TARGET_BLEND_DESC blend;
    memset(&blend, 0, sizeof(blend));
    blend.BlendEnable = true;
    blend.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend.BlendOp = D3D11_BLEND_OP_ADD;
    blend.SrcBlendAlpha = D3D11_BLEND_ONE;
    blend.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0] = blend;
    m_device->CreateBlendState(&blendDesc, &m_res.blendState);

    m_res.valid = true;

    qDebug() << "Surface created!!";

    setRenderTarget(QQuickRenderTarget::fromD3D11Texture(m_res.texture, size()));

    return true;
}

// Updates the dx11 surface and renders if needed
void
DXWindow::render()
{
    if (!isExposed()) {
        //return;
    }

    if (!m_res.valid) {
        initResources();
    }

    const D3D11_VIEWPORT viewport = { 0.0f, 0.0f, float(width()), float(height()),
                                          0.f, 1.0f };
    m_context->RSSetViewports(1, &viewport);

    // Draw QML texture to device
    m_context->VSSetShader(m_res.vertexShader, nullptr, 0);
    m_context->PSSetShader(m_res.pixelShader, nullptr, 0);
    m_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_context->IASetInputLayout(nullptr);
    m_context->OMSetDepthStencilState(nullptr, 0);

    float blendConstants[] = {1, 1, 1, 1 };
    m_context->OMSetBlendState(m_res.blendState, blendConstants, 0xffffffff);
    m_context->RSSetState(nullptr);

    m_context->PSSetShaderResources(0, 1, &m_res.textureSrv);
    m_context->PSSetSamplers(0, 1, &m_res.sampler);
    m_context->Draw(6, 0);
}
