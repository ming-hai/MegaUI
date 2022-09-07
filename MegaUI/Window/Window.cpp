﻿#include "pch.h"

#include "Window.h"
#include "../core/ClassInfoBase.h"

#pragma warning(disable : 28251)

namespace YY
{
    namespace MegaUI
    {
        _APPLY_MEGA_UI_STATIC_CALSS_INFO(Window, _MEGA_UI_WINDOW_PROPERTY_TABLE);

        Window::Window()
            : hWnd(nullptr)
            , pRender(nullptr)
            , LastRenderSize {}
        {
        }

        Window::~Window()
        {
            if (pRender)
                HDelete(pRender);
        }

        EXTERN_C extern IMAGE_DOS_HEADER __ImageBase;

        HRESULT __fastcall Window::InitializeWindow(LPCWSTR _szTitle, HWND _hWndParent, HICON _hIcon, int _dX, int _dY, DWORD _fExStyle, DWORD _fStyle, UINT _nOptions)
        {
            WNDCLASSEXW wcex;

            // Register host window class, if needed
            wcex.cbSize = sizeof(wcex);


            if (!GetClassInfoExW((HINSTANCE)&__ImageBase, L"MegaUI", &wcex))
            {
                ZeroMemory(&wcex, sizeof(wcex));

                wcex.cbSize = sizeof(wcex);
                wcex.style = CS_GLOBALCLASS;
                wcex.hInstance = (HINSTANCE)&__ImageBase;
                wcex.hIcon = _hIcon;
                wcex.hCursor = LoadCursorW(NULL, (LPWSTR)IDC_ARROW);
                wcex.hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
                wcex.lpszClassName = L"MegaUI";
                wcex.lpfnWndProc = Window::WndProc;

                if (RegisterClassExW(&wcex) == 0)
                    return E_UNEXPECTED;
            }

            auto _iWidth = GetWidth();
            if (_iWidth < 0)
                _iWidth = CW_USEDEFAULT;

            auto _iHeight = GetHeight();
            if (_iHeight < 0)
                _iHeight = CW_USEDEFAULT;

            hWnd = CreateWindowExW(
                _fExStyle,
                L"MegaUI",
                _szTitle,
                _fStyle | WS_CLIPCHILDREN,
                _dX, _dY, _iWidth, _iHeight,
                _hWndParent,
                0,
                (HINSTANCE)&__ImageBase,
                this);

            if (!hWnd)
                return E_UNEXPECTED;
            
            //SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)this);
            //SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)Window::WndProc);

            // If top-level, initialize keyboard cue state, start all hidden
            if (!_hWndParent)
                SendMessage(hWnd, WM_CHANGEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEACCEL | UISF_HIDEFOCUS), 0);

            return S_OK;
        }

        UINT __fastcall Window::AsyncDestroyMsg()
        {
            static UINT g_AsyncDestroyMsg = 0;

            if (g_AsyncDestroyMsg == 0)
            {
                g_AsyncDestroyMsg = ::RegisterWindowMessageW(L"MegaUIAsyncDestroy");
            }

            return g_AsyncDestroyMsg;
        }

        void __fastcall Window::DestroyWindow()
        {
            if (hWnd)
                ::PostMessageW(hWnd, AsyncDestroyMsg(), 0, 0);
        }

        bool __fastcall Window::IsMinimized() const
        {
            return hWnd && (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_MINIMIZE) != 0;
        }

        void __fastcall Window::ShowWindow(int _iCmdShow)
        {
            if (hWnd)
                ::ShowWindow(hWnd, _iCmdShow);
        }

        void __fastcall Window::InvalidateRect(const Rect* _pRect)
        {
            if (hWnd)
                ::InvalidateRect(hWnd, _pRect, FALSE);
        }

        LRESULT Window::WndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
        {
            if (_uMsg == Window::AsyncDestroyMsg())
            {
                ::DestroyWindow(_hWnd);
                return 0;
            }

            Window* _pNativeWindow = nullptr;

            if (_uMsg == WM_CREATE)
            {
                auto _pInfo = (CREATESTRUCTW*)_lParam;
                if (_pInfo && _pInfo->lpCreateParams)
                {
                    _pNativeWindow = (Window*)_pInfo->lpCreateParams;
                    SetWindowLongPtrW(_hWnd, GWLP_USERDATA, (LONG_PTR)_pNativeWindow);

                    //Rect Client2;
                    //::GetWindowRect(_hWnd, &Client2);

                    Rect Client;
                    ::GetClientRect(_hWnd, &Client);
                    //intptr_t _Cooike;
                    //_pNativeWindow->StartDefer(&_Cooike);
                    //_pNativeWindow->SetX(_pInfo->x);
                    //_pNativeWindow->SetY(_pInfo->y);
                    _pNativeWindow->OnSize(Client.GetWidth(), Client.GetHeight());
                    //_pNativeWindow->EndDefer(_Cooike);
                }
            }
            else
            {
                _pNativeWindow = (Window*)GetWindowLongPtrW(_hWnd, GWLP_USERDATA);
            }

            if (_pNativeWindow)
                return _pNativeWindow->CurrentWndProc(_hWnd, _uMsg, _wParam, _lParam);

            return DefWindowProcW(_hWnd, _uMsg, _wParam, _lParam);
        }

        LRESULT Window::CurrentWndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
        {
            switch (_uMsg)
            {
            case WM_CLOSE:
                DestroyWindow();
                return 0;
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                SetWindowLongPtrW(_hWnd, GWLP_USERDATA, NULL);
                hWnd = NULL;
                HDelete(this);
                break;
            case WM_PAINT:
                OnPaint();
                ValidateRect(_hWnd, NULL);
                return 0;
                break;
            case WM_SIZE:
                // 非最小化时不刷新 Host大小
                if (!IsMinimized())
                {
                    OnSize(LOWORD(_lParam), HIWORD(_lParam));
                }
                break;
            case WM_MOUSEMOVE:
            {
                break;
            }
            default:
                break;
            }

            return DefWindowProcW(_hWnd, _uMsg, _wParam, _lParam);
        }

        HRESULT __fastcall Window::OnPaint()
        {
            if (!pRender)
            {
                auto hr = CreateRender(hWnd, &pRender);
                if (FAILED(hr))
                    return hr;
            }

            pRender->SetPixelSize(LastRenderSize);

            Rect _NeedPaint;
            auto _hr = pRender->BeginDraw(&_NeedPaint);
            if (FAILED(_hr))
                return _hr;
            Rect _Bounds(0, 0, LastRenderSize.width, LastRenderSize.height);
            PaintElement(pRender, this, _Bounds, _NeedPaint);
            return pRender->EndDraw();
        }

        HRESULT __fastcall Window::PaintElement(Render* _pRender, Element* _pElement, const Rect& _ParentBounds, const Rect& _ParentPaintRect)
        {
            if (_pRender == nullptr || _pElement == nullptr)
                return E_INVALIDARG;
#if 0
            auto& RenderNode = _pElement->RenderNode;
            auto _uInvalidateMarks = RenderNode.uInvalidateMarks;
            if (_uInvalidateMarks & (ElementRenderNode::InvalidatePosition | ElementRenderNode::InvalidateExtent))
            {
                auto _NewBounds = RenderNode.Bounds;
                if (_uInvalidateMarks & ElementRenderNode::InvalidatePosition)
                {
                    auto _Location = _pElement->GetLocation();
                    _Location.x += _ParentBounds.left;
                    _Location.y += _ParentBounds.top;

                    if(_NewBounds == _Location)
                    {
                        _uInvalidateMarks &= ~ElementRenderNode::InvalidatePosition;
                    }
                    else
                    {
                        _NewBounds.SetPoint(_Location);
                        _uInvalidateMarks |= ElementRenderNode::InvalidateChild;
                    }
                }

                if (_uInvalidateMarks & ElementRenderNode::InvalidateExtent)
                {
                    auto _Extent = _pElement->GetExtent();
                    if (_NewBounds == _Extent)
                    {
                        _uInvalidateMarks &= ~ElementRenderNode::InvalidateExtent;
                    }
                    else
                    {
                        _NewBounds.SetSize(_Extent);
                    }
                }

                RenderNode.Bounds = _NewBounds;
            }
#else
            auto _Location = _pElement->GetLocation();
            auto _Extent = _pElement->GetExtent();

            Rect _BoundsElement;
            _BoundsElement.left = _ParentBounds.left + _Location.x;
            _BoundsElement.right = _BoundsElement.left + _Extent.cx;

            _BoundsElement.top = _ParentBounds.top + _Location.y;
            _BoundsElement.bottom = _BoundsElement.top + _Extent.cy;
#endif
            // 如果没有交集，那么我们可以不绘制
            Rect _PaintRect;
            if (!IntersectRect(&_PaintRect, &_ParentPaintRect, &_BoundsElement))
                return S_OK;

            const auto _bNeedClip = _PaintRect != _BoundsElement;
            if (_bNeedClip)
            {
                _pRender->PushAxisAlignedClip(_PaintRect);
            }

            _pElement->Paint(_pRender, _BoundsElement);

            if (_bNeedClip)
                _pRender->PopAxisAlignedClip();

            for (auto pItem : _pElement->GetChildren())
            {
                PaintElement(_pRender, pItem, _BoundsElement, _ParentPaintRect);
            }

            return S_OK;
        }

        void __fastcall Window::OnSize(UINT _uWidth, UINT _uHeight)
        {
            LastRenderSize.width = _uWidth;
            LastRenderSize.height = _uHeight;

            intptr_t _Cooike;
            StartDefer(&_Cooike);

            SetWidth(_uWidth);
            SetHeight(_uHeight);

            EndDefer(_Cooike);
        }


    } // namespace MegaUI
} // namespace YY
