﻿#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

#include "..\..\base\MegaUITypeInt.h"
#include "..\Render.h"
#include "..\..\base\alloc.h"
#include <MegaUI/base/Font.h>
#include <MegaUI/base/ComPtr.h>
#include <MegaUI/Render/D2D/DWrite/DWriteHelper.h>

#pragma comment(lib, "d2d1.lib")

#pragma pack(push, __MEGA_UI_PACKING)

namespace YY
{
    namespace MegaUI
    {
        // D2D本身内部就是双缓冲……,此版本适用于Windows Vista SP2以及Windows 7
        class D2D1_0Render : public Render
        {
        private:
            HWND hWnd;
            ID2D1Factory* pD2DFactory;
            ID2D1HwndRenderTarget* pRenderTarget;
            D2D1_SIZE_U PixelSize;
            IDWriteFactory* pDWriteFactory;
            DWriteHelper DWrite;

        public:
            D2D1_0Render(HWND _hWnd, ID2D1Factory* _pD2DFactory, ID2D1HwndRenderTarget* _pRenderTarget, const D2D1_SIZE_U& _PixelSize)
                : hWnd(_hWnd)
                , pD2DFactory(_pD2DFactory)
                , pRenderTarget(_pRenderTarget)
                , PixelSize(_PixelSize)
                , pDWriteFactory(nullptr)
            {
            }

            D2D1_0Render(const D2D1_0Render&) = delete;

            ~D2D1_0Render()
            {
                if (pRenderTarget)
                    pRenderTarget->Release();

                if (pD2DFactory)
                    pD2DFactory->Release();
                
                if (pDWriteFactory)
                    pDWriteFactory->Release();
            }

            void __MEGA_UI_API operator=(const D2D1_0Render&) = delete;

            HRESULT __MEGA_UI_API InitializeRenderTarget()
            {
                if (!pRenderTarget)
                {
                    auto _hr = pD2DFactory->CreateHwndRenderTarget(
                        D2D1::RenderTargetProperties(),
                        D2D1::HwndRenderTargetProperties(hWnd, PixelSize),
                        &pRenderTarget);

                    if (FAILED(_hr))
                        return _hr;

                    if (!pRenderTarget)
                    {
                        return E_UNEXPECTED;
                    }
                }


                return S_OK;
            }

            static HRESULT __MEGA_UI_API CreateRender(_In_ HWND _hWnd, _Outptr_ Render** _ppRender)
            {
                if (_ppRender == nullptr)
                    return E_INVALIDARG;
                    
                *_ppRender = nullptr;

                if (_hWnd == NULL)
                    return E_INVALIDARG;

                RECT _ClientRect;
                if (!GetClientRect(_hWnd, &_ClientRect))
                    return E_UNEXPECTED;

                HRESULT _hr = S_OK;
                ID2D1Factory* _pD2DFactory = nullptr;
                ID2D1HwndRenderTarget* _pRenderTarget = nullptr;

                do
                {
                    _hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_pD2DFactory);
                    if (FAILED(_hr))
                        break;

                    if (!_pD2DFactory)
                    {
                        _hr =  E_UNEXPECTED;
                        break;
                    }

                    D2D1_SIZE_U _PixelSize = D2D1::SizeU(
                        static_cast<UINT>(_ClientRect.right - _ClientRect.left),
                        static_cast<UINT>(_ClientRect.bottom - _ClientRect.top));


                    _hr = _pD2DFactory->CreateHwndRenderTarget(
                        D2D1::RenderTargetProperties(),
                        D2D1::HwndRenderTargetProperties(_hWnd, _PixelSize),
                        &_pRenderTarget);

                    if (FAILED(_hr))
                        break;
                        
                    if (!_pRenderTarget)
                    {
                        _hr = E_UNEXPECTED;
                        break;
                    }

                    auto _pD2DRender = HNew<D2D1_0Render>(_hWnd, _pD2DFactory, _pRenderTarget, _PixelSize);
                    if (!_pD2DRender)
                    {
                        _hr = E_OUTOFMEMORY;
                        break;
                    }

                    *_ppRender = _pD2DRender;
                    return S_OK;
                } while (false);

                if (_pRenderTarget)
                    _pRenderTarget->Release();

                if (_pD2DFactory)
                    _pD2DFactory->Release();

                return _hr;
            }

            virtual HRESULT __MEGA_UI_API BeginDraw(Rect* _pNeedPaintRect) override
            {
                if (_pNeedPaintRect)
                {
                    _pNeedPaintRect->Left = 0;
                    _pNeedPaintRect->Top = 0;
                    _pNeedPaintRect->Right = (float)PixelSize.width;
                    _pNeedPaintRect->Bottom = (float)PixelSize.height;
                }
                auto _hr = InitializeRenderTarget();
                if (FAILED(_hr))
                    return _hr;
                
                pRenderTarget->BeginDraw();
                return S_OK;
            }

            virtual HRESULT __MEGA_UI_API EndDraw() override
            {
                if (!pRenderTarget)
                    return E_UNEXPECTED;

                HRESULT _hr = _hr = pRenderTarget->EndDraw();
 
                // 需要重新呈现
                if (_hr == D2DERR_RECREATE_TARGET)
                {
                    _hr = S_OK;
                    pRenderTarget->Release();
                    pRenderTarget = nullptr;
                }

                return _hr;
            }

            virtual void __MEGA_UI_API PushAxisAlignedClip(
                _In_ const Rect& _ClipRect) override
            {
                pRenderTarget->PushAxisAlignedClip(_ClipRect, D2D1_ANTIALIAS_MODE_ALIASED);
            }

            virtual void __MEGA_UI_API PopAxisAlignedClip() override
            {
                pRenderTarget->PopAxisAlignedClip();
            }

            virtual void __MEGA_UI_API FillRectangle(
                _In_ const Rect& _Rect,
                _In_ ID2D1Brush* _pBrush) override
            {
                pRenderTarget->FillRectangle(_Rect, _pBrush);
            }

            virtual HRESULT __MEGA_UI_API SetPixelSize(
                _In_ const D2D1_SIZE_U& _PixelSize) override
            {
                if (_PixelSize.width == PixelSize.width && _PixelSize.height == PixelSize.height)
                    return S_OK;

                PixelSize.width = _PixelSize.width;
                PixelSize.height = _PixelSize.height;
                if (pRenderTarget)
                    pRenderTarget->Resize(PixelSize);

                return S_OK;
            }

            D2D1_SIZE_U __MEGA_UI_API GetPixelSize()
            {
                return PixelSize;
            }

            virtual
            HRESULT
            __MEGA_UI_API
            CreateSolidColorBrush(
                Color _Color,
                _Outptr_ ID2D1SolidColorBrush** _ppSolidColorBrush) override
            {
                return pRenderTarget->CreateSolidColorBrush(_Color, _ppSolidColorBrush);
            }
            
            virtual
            void
            __MEGA_UI_API
            DrawString(
                _In_ uStringView _szText,
                _In_ const Font& _FontInfo,
                _In_ const Rect& _LayoutRect,
                _In_ int32_t _fTextAlign
                ) override
            {
                DWrite.DrawString(pRenderTarget, _szText, _FontInfo, _LayoutRect, _fTextAlign);
            }

            virtual
            void
            __MEGA_UI_API
            MeasureString(
                _In_ uStringView _szText,
                _In_ const Font& _FontInfo,
                _In_ const Size& _LayoutSize,
                _In_ int32_t _fTextAlign,
                _Out_ Size* _pExtent
                ) override
            {
                DWrite.MeasureString(_szText, _FontInfo, _LayoutSize, _fTextAlign, _pExtent);
            }
        };
    } // namespace MegaUI
} // namespace YY

#pragma pack(pop)
