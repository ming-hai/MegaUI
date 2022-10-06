﻿#include "pch.h"

#include "value.h"

#include "StyleSheet.h"
#include "Element.h"

#pragma warning(disable : 28251)

namespace YY
{
    namespace MegaUI
    {
        float __MEGA_UI_API UpdateDpi(_In_ float _iValue, _In_ int32_t _iOldDpi, _In_ int32_t _iNewDpi, _In_ ValueSuffixType _Type)
        {
            switch (_Type)
            {
            case ValueSuffixType::None:
            //case ValueSuffixType::Pixel:
                return _iValue;
                break;
            case ValueSuffixType::DevicePixel:
            case ValueSuffixType::FontPoint:
                return UpdatePixel(_iValue, _iOldDpi, _iNewDpi);
                break;
            default:
                std::abort();
                break;
            }
        }

        Rect __MEGA_UI_API UpdateDpi(Rect _Rect, int32_t _iNewDpi, ValueSuffix _Suffix)
        {
            if (_Suffix.RawView == 0)
            {
                if (_iNewDpi == 0)
                    throw Exception();

                _Rect.Left = UpdateDpi(_Rect.Left, _Suffix.Dpi, _iNewDpi, _Suffix.Type1);
                _Rect.Top = UpdateDpi(_Rect.Top, _Suffix.Dpi, _iNewDpi, _Suffix.Type2);
                _Rect.Right = UpdateDpi(_Rect.Right, _Suffix.Dpi, _iNewDpi, _Suffix.Type3);
                _Rect.Bottom = UpdateDpi(_Rect.Bottom, _Suffix.Dpi, _iNewDpi, _Suffix.Type4);
            }

            return _Rect;
        }

        void __MEGA_UI_API Value::SharedData::AddRef()
        {
            if (IsReadOnly())
                return;

            // 因为开头 7 位是eType 与 bSkipFree，所以每次对 cRef +1，等效于 uRawData + 0x80
            _InterlockedExchangeAdd(&uRawType, uint_t(0x80u));
        }

        void __MEGA_UI_API Value::SharedData::Release()
        {
            if (IsReadOnly())
                return;

            // 等效于 cRef >= 2
            // 0x100 = 0x80 * 2
            if (_InterlockedExchangeAdd(&uRawType, uint_t(-int_t(0x80))) >= uint_t(0x100u))
                return;

            // 引用计数归零
            if (!bSkipFree)
            {
                switch (ValueType(eType))
                {
                case ValueType::uString:
                    szValue.~StringBase();
                    break;
                case ValueType::ElementList:
                    ListVal.~ElementList();
                    break;
                case ValueType::ATOM:
                    if (uAtomVal)
                        DeleteAtom(uAtomVal);
                    break;
                case ValueType::StyleSheet:
                    if (pStyleSheet)
                        pStyleSheet->Release();
                    break;
                default:
                    break;
                }
            }

            HFree(this);
        }

        bool __MEGA_UI_API Value::SharedData::IsReadOnly()
        {
            return uRawType >= ~uint_t(0x7Fu);
        }

        bool __MEGA_UI_API Value::SharedData::NeedCalculate()
        {
            return IsReadOnly() == false && SuffixType.RawView != 0;
        }

        int32_t __MEGA_UI_API Value::SharedData::GetDpi()
        {
            return (IsReadOnly() == false && SuffixType.RawView) ? SuffixType.Dpi : 96;
        }

        Value __MEGA_UI_API Value::CreateAtomZero()
        {
            _RETUNR_CONST_VALUE(ValueType::ATOM, 0);
        }

        Value __MEGA_UI_API Value::CreateInt32Zero()
        {
            return CreateInt32<0>();
        }

        Value __MEGA_UI_API Value::CreateBoolFalse()
        {
            _RETUNR_CONST_VALUE(ValueType::boolean, false);
        }

        Value __MEGA_UI_API Value::CreateBoolTrue()
        {
            _RETUNR_CONST_VALUE(ValueType::boolean, true);
        }

        Value __MEGA_UI_API Value::CreateCursorNull()
        {
            _RETUNR_CONST_VALUE(ValueType::HCURSOR, NULL);
        }

        Value __MEGA_UI_API Value::CreateEmptyElementList()
        {            
            static const ConstValueSharedData<const uchar_t*> g_ListNull =
            {
                (uint_t)ValueType::ElementList,
                1,
                uint_max,
                nullptr,
            };

            return Value((Value::SharedData*)&g_ListNull);
        }

        Value __MEGA_UI_API Value::CreateElementNull()
        {
            _RETUNR_CONST_VALUE(ValueType::Element, nullptr);
        }

        Value __MEGA_UI_API Value::CreateNull()
        {
            return Value();
        }

        Value __MEGA_UI_API Value::CreatePointZero()
        {
            _RETUNR_CONST_VALUE(ValueType::Point, { 0, 0 });
        }

        Value __MEGA_UI_API Value::CreateRectZero()
        {
            _RETUNR_CONST_VALUE(ValueType::Rect, {0, 0, 0, 0});
        }

        Value __MEGA_UI_API Value::CreateSizeZero()
        {
            _RETUNR_CONST_VALUE(ValueType::Size, { 0, 0 });
        }

        Value __MEGA_UI_API Value::CreateEmptyString()
        {
            static const ConstValueSharedData<const uchar_t*> g_StringNull =
            {
                (uint_t)ValueType::uString,
                1,
                uint_max,
                uString::StringData::GetEmtpyStringData()->GetStringBuffer(),
            };

            return Value((Value::SharedData*)&g_StringNull);
        }

        Value __MEGA_UI_API Value::CreateUnavailable()
        {
            _RETUNR_CONST_VALUE(ValueType::Unavailable);
        }

        Value __MEGA_UI_API Value::CreateUnset()
        {
            _RETUNR_CONST_VALUE(ValueType::Unset);
        }

        Value __MEGA_UI_API Value::CreateLayoutNull()
        {
            _RETUNR_CONST_VALUE(ValueType::Layout, nullptr);
        }

        Value __MEGA_UI_API Value::CreateDefaultFont()
        {
            return CreateFont(
                L"Arial",
                20,
                FontWeight::Normal,
                FontStyle::None,
                Color(255, 0, 0, 0));
        }

        Value __MEGA_UI_API Value::CreateFont(uString _szFace, float _uFontSize, uint32_t _uWeight, uint32_t _fStyle, Color _Color, ValueSuffix _Suffix)
        {
            auto _pValue = (Value::SharedData*)HAlloc(sizeof(Value::SharedData));
            if (_pValue)
            {
                _pValue->eType = uint_t(ValueType::Font);
                _pValue->bSkipFree = 0;
                _pValue->cRef = 1;
                auto& FontValue = _pValue->FontValue;
                new (&FontValue) Font;

                FontValue.szFace = std::move(_szFace);
                FontValue.iSize = _uFontSize;
                FontValue.uWeight = _uWeight;
                FontValue.fStyle = _fStyle;
                FontValue.Color = _Color;

                _pValue->SuffixType.RawView = _Suffix.RawView;
            }

            return Value(_pValue);
        }

        Value __MEGA_UI_API Value::CreateColorTransparant()
        {
            _RETUNR_CONST_VALUE(ValueType::Color, {});
        }

        Value __MEGA_UI_API Value::CreateSheetNull()
        {
            _RETUNR_CONST_VALUE(ValueType::StyleSheet, nullptr);
        }
        
        Value& __MEGA_UI_API Value::operator=(const Value& _Other)
        {
            if (pSharedData != _Other.pSharedData)
            {
                if (pSharedData)
                    pSharedData->Release();

                pSharedData = _Other.pSharedData;

                if (pSharedData)
                    pSharedData->AddRef();
            }

            return *this;
        }

        Value& __MEGA_UI_API Value::operator=(Value&& _Other) noexcept
        {
            if (this != &_Other)
            {
                if (pSharedData)
                    pSharedData->Release();

                pSharedData = _Other.pSharedData;
                _Other.pSharedData = nullptr;
            }

            return *this;
        }

        Value& __MEGA_UI_API Value::operator=(std::nullptr_t)
        {
            if (pSharedData)
                pSharedData->Release();
            pSharedData = nullptr;
            return *this;
        }

        bool __MEGA_UI_API Value::operator==(const Value& _Other) const
        {
            return CmpValue(_Other, ValueCmpOperation::Equal);
        }

        bool __MEGA_UI_API Value::operator!=(const Value& _Other) const
        {
            return operator==(_Other) == false;
        }

        bool __MEGA_UI_API Value::operator==(std::nullptr_t) const
        {
            return pSharedData == nullptr;
        }

        bool __MEGA_UI_API Value::operator!=(std::nullptr_t) const
        {
            return pSharedData != nullptr;
        }

        ValueType __MEGA_UI_API Value::GetType() const
        {
            if (!pSharedData)
                return ValueType::Null;
            return ValueType(pSharedData->eType);
        }

        bool __MEGA_UI_API Value::HasValue() const
        {
            return pSharedData && pSharedData->eType > int_t(ValueType::Null);
        }

        Value __MEGA_UI_API Value::CreateInt32(int32_t _iValue)
        {
            if (_iValue == 0)
                return CreateInt32Zero();

            auto pValue = (Value::SharedData*)HAlloc(sizeof(Value::SharedData));
            if (pValue)
            {
                pValue->eType = uint_t(ValueType::int32_t);
                pValue->bSkipFree = 0;
                pValue->cRef = 1;
                pValue->int32Value = _iValue;
            }
            return Value(pValue);
        }

        Value __MEGA_UI_API Value::CreateFloat(float _iValue, ValueSuffix _Suffix)
        {
            auto pValue = (Value::SharedData*)HAlloc(sizeof(Value::SharedData));
            if (pValue)
            {
                pValue->eType = uint_t(ValueType::float_t);
                pValue->bSkipFree = 0;
                pValue->cRef = 1;
                pValue->floatValue = _iValue;
                pValue->SuffixType.RawView = _Suffix.RawView;
            }
            return Value(pValue);
        }

        Value __MEGA_UI_API Value::CreateBool(bool _bValue)
        {
            return _bValue ? CreateBoolTrue() : CreateBoolFalse();
        }
        
        Value __MEGA_UI_API Value::CreateElementRef(Element* _pValue)
        {
            if (_pValue == nullptr)
                return CreateElementNull();

            auto pValue = (Value::SharedData*)HAlloc(sizeof(Value::SharedData));
            if (pValue)
            {
                pValue->eType = uint_t(ValueType::Element);
                pValue->bSkipFree = 0;
                pValue->cRef = 1;
                pValue->pEleValue = _pValue;
            }
            return Value(pValue);
        }
        
        Value __MEGA_UI_API Value::CreateElementList(const ElementList& _pListValue)
        {
            if (_pListValue.GetSize() == 0)
                return CreateEmptyElementList();

            auto pValue = (Value::SharedData*)HAlloc(sizeof(Value::SharedData));
            if (pValue)
            {
                pValue->eType = uint_t(ValueType::ElementList);
                pValue->bSkipFree = 0;
                pValue->cRef = 1;
                new (&pValue->ListVal) ElementList(_pListValue);
            }
            return Value(pValue);
        }
        
        Value __MEGA_UI_API Value::CreateString(const uString& _szValue)
        {
            if (_szValue.GetSize() == 0)
                return CreateEmptyString();

            auto pValue = (Value::SharedData*)HAlloc(sizeof(Value::SharedData));
            if (pValue)
            {
                pValue->eType = uint_t(ValueType::uString);
                pValue->bSkipFree = 0;
                pValue->cRef = 1;
                new (&pValue->szValue) uString(_szValue);
            }
            return Value(pValue);
        }
        
        Value __MEGA_UI_API Value::CreatePoint(float _iX, float _iY)
        {
            auto pValue = (Value::SharedData*)HAlloc(sizeof(Value::SharedData));
            if (pValue)
            {
                pValue->eType = uint_t(ValueType::Point);
                pValue->bSkipFree = 0;
                pValue->cRef = 1;
                pValue->ptVal.X = _iX;
                pValue->ptVal.Y = _iY;

            }
            return Value(pValue);
        }
        
        Value __MEGA_UI_API Value::CreateSize(float _iCX, float _iCY, ValueSuffix _Suffix)
        {
            auto pValue = (Value::SharedData*)HAllocAndZero(sizeof(Value::SharedData));
            if (pValue)
            {
                pValue->eType = uint_t(ValueType::Size);
                pValue->bSkipFree = 0;
                pValue->cRef = 1;
                pValue->sizeVal.Width = _iCX;
                pValue->sizeVal.Height = _iCY;                
                
                pValue->SuffixType.RawView = _Suffix.RawView;                
            }
            return Value(pValue);
        }
        
        Value __MEGA_UI_API Value::CreateRect(float _iLeft, float _iTop, float _iRight, float _iBottom, ValueSuffix _Suffix)
        {
            auto pValue = (Value::SharedData*)HAllocAndZero(sizeof(Value::SharedData));
            if (pValue)
            {
                pValue->eType = uint_t(ValueType::Rect);
                pValue->bSkipFree = 0;
                pValue->cRef = 1;
                pValue->rectVal.Left = _iLeft;
                pValue->rectVal.Top = _iTop;
                pValue->rectVal.Right = _iRight;
                pValue->rectVal.Bottom = _iBottom;

                pValue->SuffixType.RawView = _Suffix.RawView;
            }
            return Value(pValue);
        }

        Value __MEGA_UI_API Value::CreateAtom(raw_const_ustring_t _szValue)
        {
            return Value::CreateAtom(AddAtomW(_szValue));
        }
        
        Value __MEGA_UI_API Value::CreateAtom(ATOM _uAtomValue)
        {
            auto pValue = (Value::SharedData*)HAlloc(sizeof(Value::SharedData));
            if (pValue)
            {
                pValue->eType = uint_t(ValueType::ATOM);
                pValue->bSkipFree = 0;
                pValue->cRef = 1;
                pValue->uAtomVal = _uAtomValue;
            }
            return Value(pValue);
        }
        
        Value __MEGA_UI_API Value::CreateCursor(raw_const_ustring_t _szValue)
        {
            if (_szValue == nullptr || _szValue[0] == 0)
                return nullptr;

            auto _hCursor = LoadCursorFromFileW(_szValue);

            if (_hCursor == NULL)
                return nullptr;

            auto _pValue = Value::CreateCursor(_hCursor);

            return _pValue;
        }
        
        Value __MEGA_UI_API Value::CreateCursor(HCURSOR _hCursorValue)
        {
            auto pValue = (Value::SharedData*)HAlloc(sizeof(Value::SharedData));
            if (pValue)
            {
                pValue->eType = uint_t(ValueType::HCURSOR);
                pValue->bSkipFree = 0;
                pValue->cRef = 1;
                pValue->hCursorVal = _hCursorValue;
            }
            return Value(pValue);
        }
        
        Value __MEGA_UI_API Value::CreateColor(Color _Color)
        {
            auto pValue = (Value::SharedData*)HAlloc(sizeof(Value::SharedData));
            if (pValue)
            {
                pValue->eType = uint_t(ValueType::Color);
                pValue->bSkipFree = 0;
                pValue->cRef = 1;
                pValue->ColorValue = _Color;
            }
            return Value(pValue);
        }

        Value __MEGA_UI_API Value::CreateStyleSheet(_In_ StyleSheet* _pStyleSheet)
        {
            if (!_pStyleSheet)
                return Value::CreateSheetNull();

            auto pValue = (Value::SharedData*)HAlloc(sizeof(Value::SharedData));
            if (pValue)
            {
                pValue->eType = uint_t(ValueType::StyleSheet);
                pValue->bSkipFree = 0;
                pValue->cRef = 1;
                pValue->pStyleSheet = _pStyleSheet;
                if (_pStyleSheet)
                    _pStyleSheet->AddRef();

            }
            return Value(pValue);
        }

        int32_t __MEGA_UI_API Value::GetInt32() const
        {
            if (GetType() != ValueType::int32_t)
                throw Exception();

            return pSharedData->int32Value;
        }

        float __MEGA_UI_API Value::GetFloat() const
        {
            if (GetType() != ValueType::float_t)
                throw Exception();

            return pSharedData->floatValue;
        }
        
        bool __MEGA_UI_API Value::GetBool() const
        {
            if (GetType() != ValueType::boolean)
                throw Exception();

            return pSharedData->boolValue;
        }
        
        Size __MEGA_UI_API Value::GetSize() const
        {
            if (GetType() != ValueType::Size)
                throw Exception();

            return pSharedData->sizeVal;
        }
        
        Point __MEGA_UI_API Value::GetPoint() const
        {
            if (GetType() != ValueType::Point)
                throw Exception();

            return pSharedData->ptVal;
        }
        
        uint8_t* __MEGA_UI_API Value::GetRawBuffer()
        {
            return pSharedData ? pSharedData->RawBuffer : nullptr;
        }
        
        Color __MEGA_UI_API Value::GetColor() const
        {
            if (GetType() != ValueType::Color)
                throw Exception();

            return pSharedData->ColorValue;
        }

        Element* __MEGA_UI_API Value::GetElement() const
        {
            if (GetType() != ValueType::Element)
                throw Exception();

            return pSharedData->pEleValue;
        }

        uString __MEGA_UI_API Value::GetString() const
        {
            if (GetType() != ValueType::uString)
                throw Exception();
            return pSharedData->szValue;
        }

        StyleSheet* __MEGA_UI_API Value::GetStyleSheet() const
        {
            if (GetType() != ValueType::StyleSheet)
                throw Exception();
            return pSharedData->pStyleSheet;
        }

        Font& __MEGA_UI_API Value::GetFont() const
        {
            if (GetType() != ValueType::Font)
                throw Exception();
            return pSharedData->FontValue;
        }

        Rect& __MEGA_UI_API Value::GetRect() const
        {
            if (GetType() != ValueType::Rect)
                throw Exception();
            return pSharedData->rectVal;
        }

        bool __MEGA_UI_API Value::CmpValue(const Value& _Other, ValueCmpOperation _Operation, bool _bIgnoreDpi) const
        {
            if (pSharedData == _Other.pSharedData)
            {
                switch (GetType())
                {
                case ValueType::int32_t:
                case ValueType::float_t:
                    return _Operation == ValueCmpOperation::Equal || _Operation == ValueCmpOperation::GreaterThanOrEqual || _Operation == ValueCmpOperation::LessThanOrEqual;
                    break;
                default:
                    return _Operation == ValueCmpOperation::Equal;
                    break;
                }
            }

            // 类型不同时，只能比较 NotEqual
            if (GetType() != _Other.GetType())
                return _Operation == ValueCmpOperation::NotEqual;

            switch (GetType())
            {
            case ValueType::int32_t:
                switch (_Operation)
                {
                case ValueCmpOperation::Equal:
                    return GetInt32() == _Other.GetInt32();
                    break;
                case ValueCmpOperation::NotEqual:
                    return GetInt32() != _Other.GetInt32();
                    break;
                case ValueCmpOperation::GreaterThan:
                    return GetInt32() > _Other.GetInt32();
                    break;
                case ValueCmpOperation::GreaterThanOrEqual:
                    return GetInt32() >= _Other.GetInt32();
                    break;
                case ValueCmpOperation::LessThan:
                    return GetInt32() < _Other.GetInt32();
                    break;
                case ValueCmpOperation::LessThanOrEqual:
                    return GetInt32() <= _Other.GetInt32();
                    break;
                }
                break;
            case ValueType::float_t:
            {
                auto _iLeft = pSharedData->floatValue;
                auto _iRight = _Other.pSharedData->floatValue;
                if (_bIgnoreDpi == false && _Other.pSharedData->NeedCalculate())
                    _iRight = MegaUI::UpdateDpi(_iRight, _Other.pSharedData->GetDpi(), pSharedData->GetDpi(), _Other.pSharedData->SuffixType.Type1);

                switch (_Operation)
                {
                case ValueCmpOperation::Equal:
                    return _iLeft == _iRight;
                    break;
                case ValueCmpOperation::NotEqual:
                    return _iLeft != _iRight;
                    break;
                case ValueCmpOperation::GreaterThan:
                    return _iLeft > _iRight;
                    break;
                case ValueCmpOperation::GreaterThanOrEqual:
                    return _iLeft >= _iRight;
                    break;
                case ValueCmpOperation::LessThan:
                    return _iLeft < _iRight;
                    break;
                case ValueCmpOperation::LessThanOrEqual:
                    return _iLeft <= _iRight;
                    break;
                }
                break;
            }
            case ValueType::boolean:
                switch (_Operation)
                {
                case ValueCmpOperation::Equal:
                    return GetBool() == _Other.GetBool();
                    break;
                case ValueCmpOperation::NotEqual:
                    return GetBool() != _Other.GetBool();
                    break;
                }
                break;
            case ValueType::uString:
                switch (_Operation)
                {
                case ValueCmpOperation::Equal:
                    return pSharedData->szValue == _Other.pSharedData->szValue;
                    break;
                case ValueCmpOperation::NotEqual:
                    return pSharedData->szValue != _Other.pSharedData->szValue;
                    break;
                }
                break;
            case ValueType::Point:
                switch (_Operation)
                {
                case ValueCmpOperation::Equal:
                    return pSharedData->ptVal == _Other.pSharedData->ptVal;
                    break;
                case ValueCmpOperation::NotEqual:
                    return pSharedData->ptVal != _Other.pSharedData->ptVal;
                    break;
                }
                break;
            case ValueType::Size:
            {
                auto& _iLeft = pSharedData->sizeVal;
                auto _iRight = _Other.pSharedData->sizeVal;
                if (_bIgnoreDpi == false && _Other.pSharedData->NeedCalculate())
                {
                    auto _iOldDpi = _Other.pSharedData->GetDpi();
                    auto _iNewDpi = pSharedData->GetDpi();
                    if (_iOldDpi != _iNewDpi)
                    {
                        _iRight.Width = MegaUI::UpdateDpi(_iRight.Width, _iOldDpi, _iNewDpi, _Other.pSharedData->SuffixType.Type1);
                        _iRight.Height = MegaUI::UpdateDpi(_iRight.Height, _iOldDpi, _iNewDpi, _Other.pSharedData->SuffixType.Type2);
                    }
                }

                switch (_Operation)
                {
                case ValueCmpOperation::Equal:
                    return _iLeft == _iRight;
                    break;
                case ValueCmpOperation::NotEqual:
                    return _iLeft != _iRight;
                    break;
                }
                break;
            }
            case ValueType::Rect:
            {
                auto& _iLeft = pSharedData->rectVal;
                auto _iRight = _Other.pSharedData->rectVal;
                if (_bIgnoreDpi == false && _Other.pSharedData->NeedCalculate())
                {
                    auto _iOldDpi = _Other.pSharedData->GetDpi();
                    auto _iNewDpi = pSharedData->GetDpi();
                    if (_iOldDpi != _iNewDpi)
                    {
                        _iRight.Left = MegaUI::UpdateDpi(_iRight.Left, _iOldDpi, _iNewDpi, _Other.pSharedData->SuffixType.Type1);
                        _iRight.Top = MegaUI::UpdateDpi(_iRight.Top, _iOldDpi, _iNewDpi, _Other.pSharedData->SuffixType.Type2);
                        _iRight.Right = MegaUI::UpdateDpi(_iRight.Right, _iOldDpi, _iNewDpi, _Other.pSharedData->SuffixType.Type3);
                        _iRight.Bottom = MegaUI::UpdateDpi(_iRight.Bottom, _iOldDpi, _iNewDpi, _Other.pSharedData->SuffixType.Type4);
                    }
                }

                switch (_Operation)
                {
                case ValueCmpOperation::Equal:
                    return pSharedData->rectVal == _Other.pSharedData->rectVal;
                    break;
                case ValueCmpOperation::NotEqual:
                    return pSharedData->rectVal != _Other.pSharedData->rectVal;
                    break;
                }
                break;
            }
            case ValueType::Element:
                switch (_Operation)
                {
                case ValueCmpOperation::Equal:
                    return pSharedData->pEleValue == _Other.pSharedData->pEleValue;
                    break;
                case ValueCmpOperation::NotEqual:
                    return pSharedData->pEleValue != _Other.pSharedData->pEleValue;
                    break;
                }
                break;
            case ValueType::ElementList:
                if (_Operation == ValueCmpOperation::Equal || _Operation == ValueCmpOperation::NotEqual)
                {
                    auto _uSize = pSharedData->ListVal.GetSize();
                    auto _bSame = _uSize == _Other.pSharedData->ListVal.GetSize();
                    if (_bSame)
                    {
                        for (uint32_t _uIndex = 0; _uIndex != _uSize;++ _uIndex)
                        {
                            _bSame = pSharedData->ListVal[_uIndex] == _Other.pSharedData->ListVal[_uIndex];
                            if (!_bSame)
                                break;
                        }
                    }

                    return _Operation == ValueCmpOperation::Equal ? _bSame : _bSame == false;
                }
            case ValueType::ATOM:
                switch (_Operation)
                {
                case ValueCmpOperation::Equal:
                    return pSharedData->uAtomVal == _Other.pSharedData->uAtomVal;
                    break;
                case ValueCmpOperation::NotEqual:
                    return pSharedData->uAtomVal != _Other.pSharedData->uAtomVal;
                    break;
                }
                break;
            case ValueType::HCURSOR:
                switch (_Operation)
                {
                case ValueCmpOperation::Equal:
                    return pSharedData->hCursorVal == _Other.pSharedData->hCursorVal;
                    break;
                case ValueCmpOperation::NotEqual:
                    return pSharedData->hCursorVal != _Other.pSharedData->hCursorVal;
                    break;
                }
                break;
            case ValueType::Layout:
                switch (_Operation)
                {
                case ValueCmpOperation::Equal:
                    return pSharedData->pLayout == _Other.pSharedData->pLayout;
                    break;
                case ValueCmpOperation::NotEqual:
                    return pSharedData->pLayout != _Other.pSharedData->pLayout;
                    break;
                }
                break;
            case ValueType::Color:
                switch (_Operation)
                {
                case ValueCmpOperation::Equal:
                    return pSharedData->ColorValue.ColorRGBA == _Other.pSharedData->ColorValue.ColorRGBA;
                    break;
                case ValueCmpOperation::NotEqual:
                    return pSharedData->ColorValue.ColorRGBA != _Other.pSharedData->ColorValue.ColorRGBA;
                    break;
                }
                break;
            case ValueType::Font:
            {
                if (pSharedData->FontValue.uWeight == _Other.pSharedData->FontValue.uWeight
                    && pSharedData->FontValue.fStyle == _Other.pSharedData->FontValue.fStyle
                    && pSharedData->FontValue.Color == _Other.pSharedData->FontValue.Color
                    && pSharedData->FontValue.szFace.GetSize() == _Other.pSharedData->FontValue.szFace.GetSize()
                    && pSharedData->FontValue.szFace.CompareI(_Other.pSharedData->FontValue.szFace) == 0)
                {
                    auto _iLeft = pSharedData->FontValue.iSize;
                    auto _iRight = _Other.pSharedData->FontValue.iSize;
                    if (_bIgnoreDpi == false && _Other.pSharedData->NeedCalculate())
                    {
                        auto _iOldDpi = _Other.pSharedData->GetDpi();
                        auto _iNewDpi = pSharedData->GetDpi();

                        _iRight = MegaUI::UpdateDpi(_iRight, _iOldDpi, _iNewDpi, _Other.pSharedData->SuffixType.Type1);
                    }

                    if (_iLeft == _iRight)
                    {
                        if (_Operation == ValueCmpOperation::Equal)
                            return true;
                    }
                }

                return false;
                break;
            }
            default:
                break;
            }

            return false;
        }

        bool __MEGA_UI_API Value::IsSame(const Value& _Other) const
        {
            return pSharedData == _Other.pSharedData;
        }
        
        Value __MEGA_UI_API Value::UpdateDpi(_In_ int32_t _iNewDpi) const
        {
            if (pSharedData && pSharedData->NeedCalculate())
            {
                if (_iNewDpi == 0 || _iNewDpi > uint16_max)
                    throw Exception(_S("DPI错误！"));

                auto _pSuffixType = pSharedData->SuffixType;
                auto _iOldDpi = _pSuffixType.Dpi;
                _pSuffixType.Dpi = (uint16_t)_iNewDpi;

                if (_iOldDpi == _pSuffixType.Dpi)
                    return *this;

                switch (ValueType(pSharedData->eType))
                {
                case ValueType::float_t:
                {
                    const auto _iNewValue = YY::MegaUI::UpdateDpi(pSharedData->floatValue, _iOldDpi, _iNewDpi, _pSuffixType.Type1);
                    if (_iNewValue == pSharedData->floatValue)
                        break;
                    return Value::CreateFloat(_iNewValue, _pSuffixType);
                    break;
                }
                case ValueType::Size:
                {
                    const auto _cx = YY::MegaUI::UpdateDpi(pSharedData->sizeVal.Width, _iOldDpi, _iNewDpi, _pSuffixType.Type1);
                    const auto _cy = YY::MegaUI::UpdateDpi(pSharedData->sizeVal.Height, _iOldDpi, _iNewDpi, _pSuffixType.Type2);
                    if (_cx == pSharedData->sizeVal.Width && _cy == pSharedData->sizeVal.Height)
                        break;
                    return Value::CreateSize(_cx, _cy, _pSuffixType);
                    break;
                }
                case ValueType::Rect:
                {
                    const auto _left = YY::MegaUI::UpdateDpi(pSharedData->rectVal.Left, _iOldDpi, _iNewDpi, _pSuffixType.Type1);
                    const auto _top = YY::MegaUI::UpdateDpi(pSharedData->rectVal.Top, _iOldDpi, _iNewDpi, _pSuffixType.Type2);
                    const auto _right = YY::MegaUI::UpdateDpi(pSharedData->rectVal.Right, _iOldDpi, _iNewDpi, _pSuffixType.Type3);
                    const auto _bottom = YY::MegaUI::UpdateDpi(pSharedData->rectVal.Bottom, _iOldDpi, _iNewDpi, _pSuffixType.Type4);
                    if (_left == pSharedData->rectVal.Left && _top == pSharedData->rectVal.Top && _right == pSharedData->rectVal.Right && _bottom == pSharedData->rectVal.Bottom)
                        break;
                    return Value::CreateRect(_left, _top, _right, _bottom, _pSuffixType);
                    break;
                }
                case ValueType::Font:
                {
                    const auto _iSize = YY::MegaUI::UpdateDpi(pSharedData->FontValue.iSize, _iOldDpi, _iNewDpi, _pSuffixType.Type1);
                    if (_iSize == pSharedData->FontValue.iSize)
                        break;

                    return Value::CreateFont(
                        pSharedData->FontValue.szFace,
                        _iSize,
                        pSharedData->FontValue.uWeight,
                        pSharedData->FontValue.fStyle,
                        pSharedData->FontValue.Color,
                        _pSuffixType);
                    break;
                }
                }
            }

            return *this;
        }
        


    } // namespace MegaUI
}


