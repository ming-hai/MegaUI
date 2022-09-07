﻿#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "../base/MegaUITypeInt.h"
#include "../core/Element.h"

#pragma pack(push, __MEGA_UI_PACKING)

namespace YY
{
    namespace MegaUI
    {
#define _MEGA_UI_WINDOW_PROPERTY_TABLE(_APPLY) \
    _APPLY(Title, PF_LocalOnly | PF_ReadOnly, 0, &Value::GetStringNull, nullptr, nullptr, nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(), ValueType::uString)


        class Render;

        class Window : public Element
        {
            _APPLY_MEGA_UI_STATIC_CALSS_INFO_EXTERN(Window, Element, ClassInfoBase<Window>, 0u, _MEGA_UI_WINDOW_PROPERTY_TABLE);
        protected:
            HWND hWnd;
            Render* pRender;
            D2D1_SIZE_U LastRenderSize;
        public:
            Window();

            virtual ~Window();

            Window(const Window&) = delete;
            void __fastcall operator=(const Window&) = delete;

            HRESULT __fastcall InitializeWindow(
                _In_opt_ LPCWSTR _szTitle,
                _In_opt_ HWND _hWndParent,
                _In_opt_ HICON _hIcon,
                _In_ int _dX,
                _In_ int _dY,
                _In_ DWORD _fExStyle,
                _In_ DWORD _fStyle,
                _In_ UINT _nOptions
                );

            static UINT __fastcall AsyncDestroyMsg();

            void __fastcall DestroyWindow();
            
            bool __fastcall IsMinimized() const;

            void __fastcall ShowWindow(int _iCmdShow);

            void __fastcall InvalidateRect(_In_opt_ const Rect* _pRect);

            __inline void __fastcall InvalidateRect(const Rect& _Rect)
            {
                InvalidateRect(&_Rect);
            }
        protected:
            static LRESULT CALLBACK WndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam);

            virtual LRESULT __thiscall CurrentWndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam);

            HRESULT __fastcall OnPaint();

            HRESULT __fastcall PaintElement(
                _In_ Render* _pRender,
                _In_ Element* _pElement,
                _In_ const Rect& _ParentBounds,
                _In_ const Rect& _ParentPaintRect);

            void __fastcall OnSize(UINT _uWidth, UINT _uHeight);
        };
    }
} // namespace YY