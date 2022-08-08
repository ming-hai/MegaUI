﻿#pragma once

#include <stdint.h>

#include <Windows.h>

#include "..\base\MegaUITypeInt.h"
#include "..\base\DynamicArray.h"
#include "..\base\StringBase.h"
#include "..\base\Color.h"
#include "..\base\Rect.h"

#pragma pack(push, __MEGA_UI_PACKING)

namespace YY
{
    namespace MegaUI
    {
        class Element;
        typedef DynamicArray<Element*, true, false> ElementList;

        class Layout;

#define _MEGA_UI_VALUE_TPYE_MAP(_APPLY)                      \
        _APPLY(int32_t,     int32_t,             int32Value) \
        _APPLY(boolean,     bool,                boolValue)  \
        _APPLY(uString,     uString,             szValue  )  \
        _APPLY(POINT,       POINT,               ptVal    )  \
        _APPLY(SIZE,        SIZE,                sizeVal  )  \
        _APPLY(Rect,        Rect,                rectVal  )  \
        _APPLY(Element,     Element*,            pEleValue)  \
        _APPLY(ElementList, ElementList,         ListVal  )  \
        _APPLY(ATOM,        ATOM,                uAtomVal )  \
        _APPLY(HCURSOR,     HCURSOR,             hCursorVal) \
        _APPLY(Layout,      Layout*,             pLayout  )  \
        _APPLY(Color,       Color,               ColorValue)

		enum class ValueType
        {
            // 此值不可用
            Unavailable = -2,
            // 尚未设置
            Unset       = -1,
            // 什么也没有
            Null        = 0,

#define _APPLY(_eTYPE, _TYPE, _VAR) _eTYPE,
            _MEGA_UI_VALUE_TPYE_MAP(_APPLY)
#undef _APPLY
        };
        
        template<typename _Type>
        struct ConstValueSharedData
        {
            uint_t eType : 6;
            uint_t bSkipFree : 1;
#ifdef _WIN64
            uint_t cRef : 57;
#else
            uint_t cRef : 25;
#endif
            _Type Value;
        };

        template<>
        struct ConstValueSharedData<void>
        {
            uint_t eType : 6;
            uint_t bSkipFree : 1;
#ifdef _WIN64
            uint_t cRef : 57;
#else
            uint_t cRef : 25;
#endif
        };

#define _APPLY(_eTYPE, _TYPE, _VAR)           \
        template<>                                \
        struct ValueTypeToType<ValueType::_eTYPE> \
        {                                         \
            using _Type = _TYPE;                  \
            using SharedData = ConstValueSharedData<_Type>; \
        };

        template<ValueType eType>
        struct ValueTypeToType
        {
            using _Type = void;
            using SharedData = ConstValueSharedData<_Type>;
        };
        _MEGA_UI_VALUE_TPYE_MAP(_APPLY)
#undef _APPLY

#define _DEFINE_CONST_VALUE(_VAR, _eTYPE, ...) static constexpr const ValueTypeToType<_eTYPE>::SharedData _VAR = {(uint_t)_eTYPE, 1, uint_t(-1), __VA_ARGS__}

#define _RETUNR_CONST_VALUE(_eTYPE, ...)               \
        _DEFINE_CONST_VALUE(Ret, _eTYPE, __VA_ARGS__); \
        return Value((Value::SharedData*)(&Ret));

		class Value
        {
        public:
            struct SharedData
            {
                union
                {
                    struct
                    {
                        uint_t eType : 6;
                        // 不要释放内部缓冲区
                        uint_t bSkipFree : 1;
#ifdef _WIN64
                        uint_t cRef : 57;
#else
                        uint_t cRef : 25;
#endif
                    };

                    uint_t uRawType;
                };

                union
                {
                    byte_t RawBuffer[1];
#define _APPLY(_eTYPE, _TYPE, _VAR) _TYPE _VAR;
                    _MEGA_UI_VALUE_TPYE_MAP(_APPLY)
#undef _APPLY
                };

                SharedData() = delete;
                SharedData(const SharedData&) = delete;
                ~SharedData() = delete;

                SharedData& operator=(const SharedData&) = delete;


                void __fastcall AddRef();
                void __fastcall Release();
            };

        private:
            SharedData* pSharedData;

        public:
            Value(SharedData* _pSharedData = nullptr)
                : pSharedData(_pSharedData)
            {
            }

            Value(const Value& _Other)
                : pSharedData(_Other.pSharedData)
            {
                if (pSharedData)
                    pSharedData->AddRef();
            }

            Value(Value&& _Other) noexcept
                : pSharedData(_Other.pSharedData)
            {
                _Other.pSharedData = nullptr;
            }

            ~Value()
            {
                if (pSharedData)
                    pSharedData->Release();
            }
            
            Value& __fastcall operator=(const Value& _Other);
            
            Value& __fastcall operator=(Value&& _Other) noexcept;
            
            Value& __fastcall operator=(std::nullptr_t);

            bool __fastcall operator==(_In_ const Value& _Other) const;

            bool __fastcall operator!=(_In_ const Value& _Other) const;

            bool __fastcall operator==(std::nullptr_t) const;
            
            bool __fastcall operator!=(std::nullptr_t) const;

            template<int32_t iValue>
            static Value __fastcall GetInt32ConstValue()
            {
                _RETUNR_CONST_VALUE(ValueType::int32_t, iValue);
            }

            static Value __fastcall GetAtomZero();
            static Value __fastcall GetBoolFalse();
            static Value __fastcall GetBoolTrue();
            //static _Ret_notnull_ Value* __fastcall GetColorTrans();
            static Value __fastcall GetCursorNull();
            static Value __fastcall GetElListNull();
            static Value __fastcall GetElementNull();
            static Value __fastcall GetInt32Zero();
            static Value __fastcall GetNull();
            static Value __fastcall GetPointZero();
            static Value __fastcall GetRectZero();
            static Value __fastcall GetSizeZero();
            static Value __fastcall GetStringNull();
            static Value __fastcall GetUnavailable();
            static Value __fastcall GetUnset();
            static Value __fastcall GetLayoutNull();
            static Value __fastcall GetColorTransparant();


            ValueType __fastcall GetType();

            // Value creation methods
            static Value __fastcall CreateInt32(_In_ int32_t _iValue);
            static Value __fastcall CreateBool(_In_ bool _bValue);
            static Value __fastcall CreateElementRef(_In_opt_ Element* _pValue);
            static Value __fastcall CreateElementList(_In_ const ElementList& _pListValue);
            static Value __fastcall CreateString(_In_ const uString& _szValue);
            //static _Ret_maybenull_ Value* __fastcall CreateString(_In_ uint16_t _uId, _In_opt_ HINSTANCE _hResLoad = NULL);
            static Value __fastcall CreatePoint(_In_ int32_t _iX, _In_ int32_t _iY);
            
            __inline static Value __fastcall CreatePoint(_In_ POINT _Point)
            {
                return CreatePoint(_Point.x, _Point.y);
            }

            static Value __fastcall CreateSize(_In_ int32_t _iCX, _In_ int32_t _iCY);

            __inline static Value __fastcall CreateSize(_In_ SIZE _Size)
            {
                return CreateSize(_Size.cx, _Size.cy);
            }
            static Value __fastcall CreateRect(_In_ int32_t _iLeft, _In_ int32_t _iTop, _In_ int32_t _iRight, _In_ int32_t _iBottom);
            //static _Ret_maybenull_ Value* __fastcall CreateDFCFill(_In_ uint32_t _uType, _In_ uint32_t _uState);
            static Value __fastcall CreateAtom(_In_z_ raw_const_ustring_t _szValue);
            static Value __fastcall CreateAtom(_In_ ATOM _uAtomValue);
            static Value __fastcall CreateCursor(_In_z_ raw_const_ustring_t _szValue);
            static Value __fastcall CreateCursor(_In_ HCURSOR _hCursorValue);

            __inline static Value __fastcall CreateColorARGB(_In_ uint8_t _Alpha, _In_ uint8_t _Red, _In_ uint8_t _Green, _In_ uint8_t _Blue)
            {
                return CreateColorRGBA((COLORREF(_Red) << 0) | (COLORREF(_Green) << 8) | (COLORREF(_Blue) << 16) | (COLORREF(_Alpha) << 24));
            }
            
            __inline static Value __fastcall CreateColorRGB(_In_ uint8_t _Red, _In_ uint8_t _Green, _In_ uint8_t _Blue)
            {
                return CreateColorRGBA((COLORREF(_Red) << 0) | (COLORREF(_Green) << 8) | (COLORREF(_Blue) << 16) | (COLORREF(0xFFu) << 24));
            }
            
            __inline static Value __fastcall CreateColor(_In_ Color Color)
            {
                return CreateColorRGBA(Color.ColorRGBA);
            }
            
            __inline static Value __fastcall CreateColorRGB(_In_ COLORREF ColorRGB)
            {
                return CreateColorRGBA(ColorRGB | (0xFFu << 24));
            }

            static Value __fastcall CreateColorRGBA(_In_ COLORREF ColorRGBA);
            

            int32_t __fastcall GetInt32();
            bool __fastcall GetBool();
            SIZE __fastcall GetSize();
            POINT __fastcall GetPoint();
            uint8_t* __fastcall GetRawBuffer();
            Color __fastcall GetColor();
        };

        template<ValueType _eType>
        class ValueIs : public Value
        {
        private:
            constexpr static ValueType eType = _eType;
        public:
            using _Type = typename ValueTypeToType<eType>::_Type;

            ValueIs(const Value& _Other)
                : Value(_Other)
            {
            }

            ValueIs(Value&& _Other)
                : Value(std::move(_Other))
            {
            }

            bool __fastcall HasValue()
            {
                return GetType() == eType;
            }

            _Type& __fastcall GetValue()
            {
                if (!HasValue())
                {
                    throw Exception();
                }

                return *(_Type*)GetRawBuffer();
            }
        };
    } // namespace MegaUI
} // namespace YY

#pragma pack(pop)
