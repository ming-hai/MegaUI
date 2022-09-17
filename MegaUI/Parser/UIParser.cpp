﻿#include "pch.h"
#include "UIParser.h"

#include "../base/alloc.h"
#include "../base/StringTransform.h"
#include "ValueParser.h"

#define RAPIDXML_STATIC_POOL_SIZE 1024

#include "../ThirdParty/rapidxml/rapidxml.hpp"


#define _RAPAIDXML_STAATIC_STRING(_STR) _STR, (std::size(_STR) - 1)

#pragma warning(disable : 28251)

namespace YY
{
    namespace MegaUI
    {
#pragma pack(push, 1)
        struct ByteCodeBase
        {
            // 字节码大小
            uint8_t cbData;
            // 字节码类型
            uint8_t Type;

            // uint8_t Data[0];

            enum ByteCodeType
            {
                // 字节码开始
                ByteCodeBegin = 0,
                // 字节码结束
                ByteCodeEnd,
                // 开始解析子控件
                ChildElementBegin,
                // 子控件解析结束，这时会把子控件提交到父控件中
                ChildElementEnd,

                // 创建控件
                CreateElement,
                // 设置控件属性
                SetProperty,
            };
        };
        
        struct ByteCodeBegin
        {
            // 字节码大小
            uint8_t cbData = sizeof(ByteCodeBegin);
            // 字节码类型
            uint8_t Type = ByteCodeBase::ByteCodeBegin;
        };

        struct ByteCodeEnd
        {
            // 字节码大小
            uint8_t cbData = sizeof(ByteCodeEnd);
            // 字节码类型
            uint8_t Type = ByteCodeBase::ByteCodeEnd;
        };

        struct ChildElementBegin
        {
            // 字节码大小
            uint8_t cbData = sizeof(ChildElementBegin);
            // 字节码类型
            uint8_t Type = ByteCodeBase::ChildElementBegin;
        };
        
        struct ChildElementEnd
        {
            // 字节码大小
            uint8_t cbData = sizeof(ChildElementEnd);
            // 字节码类型
            uint8_t Type = ByteCodeBase::ChildElementEnd;
        };

        struct CreateElement
        {
            // 字节码大小
            uint8_t cbData = sizeof(CreateElement);
            // 字节码类型
            uint8_t Type = ByteCodeBase::CreateElement;
            uint16_t uIndexOfElementName;
        };

        struct SetProperty
        {
            // 字节码大小
            uint8_t cbData = sizeof(SetProperty);
            // 字节码类型
            uint8_t Type = ByteCodeBase::SetProperty;
            uint16_t uIndexOfPropertyValue;
            const PropertyInfo* pProp;
        };
#pragma pack(pop)

        static void* rapidxml_alloc(std::size_t _uByteSize)
        {
            return HAlloc(_uByteSize);
        }

        static void rapidxml_free(void* _pAddress)
        {
            HFree(_pAddress);
        }

        void __MEGA_UI_API UIParser::Clear()
        {
            LocalValueCache.Clear();
            RecorderArray.Clear();
        }

        HRESULT __MEGA_UI_API UIParser::ParserByXmlString(u8String&& _szXmlString)
        {
            Clear();
            if (_szXmlString.GetSize() == 0)
                return E_INVALIDARG;

            auto _pStringBuffer = (char*) _szXmlString.LockBuffer();
            if (!_pStringBuffer)
                return E_OUTOFMEMORY;
            _szXmlString.UnlockBuffer();

            // 为了从兼容性考虑，这里直接用 char 代表 u8char_t，注意上下文。
            rapidxml::xml_document<char> _XmlDocument;
            _XmlDocument.set_allocator(rapidxml_alloc, rapidxml_free);
            try
            {
                _XmlDocument.parse<rapidxml::parse_default>(_pStringBuffer);
            } catch (const rapidxml::parse_error& _error)
            {
                return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
            }

            auto _pRoot = _XmlDocument.first_node(_RAPAIDXML_STAATIC_STRING("MegaUI"), false);
            if (!_pRoot)
                return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

            HRESULT _hr = S_OK;

            for (auto _pNote = _pRoot->first_node(); _pNote; _pNote = _pNote->next_sibling())
            {
                if (_pNote->type() != rapidxml::node_element)
                    continue;

                auto _pResID = _pNote->first_attribute(_RAPAIDXML_STAATIC_STRING("ResID"), false);
                if (!_pResID)
                {
                    _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                    break;
                }
                
                UIParserRecorder _Recorder;
                _hr = _Recorder.szResourceID.SetString((u8char_t*)_pResID->value(), _pResID->value_size());
                if (FAILED(_hr))
                    return _hr;
                _pNote->remove_attribute(_pResID);

                ByteCodeBegin _ByteCodeBegin;
                _hr = _Recorder.ByteCode.Add((uint8_t*)&_ByteCodeBegin, sizeof(_ByteCodeBegin));
                if (FAILED(_hr))
                    return _hr;

                _hr = ParserElementNode(_pNote, &_Recorder);
                if (FAILED(_hr))
                    break;

                ByteCodeEnd _ByteCodeEnd;
                _hr = _Recorder.ByteCode.Add((uint8_t*)&_ByteCodeEnd, sizeof(_ByteCodeEnd));
                if (FAILED(_hr))
                    return _hr;

                RecorderArray.EmplacePtr(std::move(_Recorder));
            }

            if (FAILED(_hr))
                Clear();

            return _hr;
        }

        HRESULT __MEGA_UI_API UIParser::Play(u8StringView _szResID, Element* _pTopElement, intptr_t* _pCooike, Element** _ppElement)
        {
            if (!_ppElement)
                return E_INVALIDARG;

            *_ppElement = nullptr;

            for (auto& Recorder : RecorderArray)
            {
                if (Recorder.szResourceID.GetSize() != _szResID.GetSize() || _szResID.CompareI(Recorder.szResourceID.GetConstString()) != 0)
                    continue;
                ArrayView<uint8_t> _ByteCode(Recorder.ByteCode.GetData(), Recorder.ByteCode.GetSize());

                if (_ByteCode.GetSize() <= sizeof(ByteCodeBegin) + sizeof(ByteCodeEnd))
                {
                    // 一个有效的格式总应该包含 ByteCodeBegin + ByteCodeEnd；
                    return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                }

                auto _pByteCodeBegin = (ByteCodeBegin*)_ByteCode.GetData();
                if (_pByteCodeBegin->Type != ByteCodeBase::ByteCodeBegin || _pByteCodeBegin->cbData < sizeof(ByteCodeBegin))
                    return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

                _ByteCode.Slice(_pByteCodeBegin->cbData);

                DynamicArray<Element*, false, false> _ElementList;
                auto _hr = Play(_ByteCode, _pTopElement, _pCooike, & _ElementList);
                if (SUCCEEDED(_hr) && _ElementList.GetSize() == 1)
                {
                    *_ppElement = *_ElementList.GetData();
                }
                else
                {
                    for (auto _pItem : _ElementList)
                    {
                        _pItem->Destroy(false);
                    }
                }

                return _hr;
            }

            return E_NOT_SET;
        }
        
        IClassInfo* __MEGA_UI_API UIParser::FindClassInfo(raw_const_astring_t _szClassName, uint32_t* _pIndex)
        {
            if (_pIndex)
                *_pIndex = uint32_max;

            if (_szClassName == nullptr || *_szClassName == '\0')
                return nullptr;

            auto _uSize = ClassArray.GetSize();
            auto _pData = ClassArray.GetData();
            for (uint32_t _uIndex = 0; _uIndex != _uSize; ++_uIndex)
            {
                auto _pClassInfo = _pData[_uIndex];

                if (_strcmpi(_pClassInfo->GetName(), _szClassName) == 0)
                {
                    if (_pIndex)
                        *_pIndex = _uIndex;
                    return _pClassInfo;
                }
            }

            auto _pClassInfo = GetRegisterControlClassInfo(_szClassName);
            if (!_pClassInfo)
                return nullptr;

            uint_t _uIndex;
            auto _ppBuffer = ClassArray.AddAndGetPtr(&_uIndex);
            if (!_ppBuffer)
                return nullptr;

            *_ppBuffer = _pClassInfo;
            if (_pIndex)
                *_pIndex = _uIndex;
            return _pClassInfo;
        }
        
        HRESULT __MEGA_UI_API UIParser::ParserElementNode(rapidxml::xml_node<char>* _pNote, UIParserRecorder* _pRecorder)
        {
            CreateElement _CreateElement;
            uint32_t _uIndex;
            auto _pClassInfo = FindClassInfo(_pNote->name(), &_uIndex);
            if (!_pClassInfo)
                return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
            _CreateElement.uIndexOfElementName = _uIndex;

            auto _hr = _pRecorder->ByteCode.Add((uint8_t*)&_CreateElement, sizeof(_CreateElement));
            if (FAILED(_hr))
                return _hr;

            for (auto _pAttr = _pNote->first_attribute(); _pAttr; _pAttr = _pAttr->next_attribute())
            {
                _hr = ParserElementProperty(_pAttr, _pClassInfo, _pRecorder);
                if (FAILED(_hr))
                    return _hr;
            }

            bool _bHasChild = false;
            
            for (auto _pChildNote = _pNote->first_node(); _pChildNote; _pChildNote = _pChildNote->next_sibling())
            {
                if (_pChildNote->type() != rapidxml::node_element)
                    continue;

                if (!_bHasChild)
                {
                    _bHasChild = true;
                    ChildElementBegin _ChildElementBegin;
                    _hr = _pRecorder->ByteCode.Add((uint8_t*)&_ChildElementBegin, sizeof(_ChildElementBegin));
                    if (FAILED(_hr))
                        return _hr;
                }

                _hr = ParserElementNode(_pChildNote, _pRecorder);
                if (FAILED(_hr))
                    return _hr;
            }

            if (_bHasChild)
            {
                ChildElementEnd _ChildElementEnd;
                _hr = _pRecorder->ByteCode.Add((uint8_t*)&_ChildElementEnd, sizeof(_ChildElementEnd));
                if (FAILED(_hr))
                    return _hr;
            }

            return S_OK;
        }
        
        HRESULT __MEGA_UI_API UIParser::ParserElementProperty(rapidxml::xml_attribute<char>* _pAttribute, IClassInfo* _pClassInfo, UIParserRecorder* _pRecorder)
        {
            const PropertyInfo* _pProp = nullptr;
            // 搜索 PropertyInfo
            for (uint32_t _uIndex = 0; _pProp = _pClassInfo->EnumPropertyInfo(_uIndex); ++_uIndex)
            {
                if (_strcmpi(_pProp->pszName, _pAttribute->name()) == 0)
                {
                    break;
                }
            }

            if (!_pProp)
                return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

            if (!_pProp->ppValidValues)
                return E_UNEXPECTED;
            
            ValueParser _ValueParser;
            ExprNode _ExprNode;

            auto _hr = _ValueParser.Parse(
                u8StringView((u8char_t*)_pAttribute->value(), _pAttribute->value_size()),
                &_ExprNode);

            if (FAILED(_hr))
                return _hr;
            
            Value _Value;

            for (auto pValidValue = _pProp->ppValidValues; *pValidValue != ValueType::Null; ++pValidValue)
            {
                HRESULT _hr = E_NOTIMPL;

                switch (*pValidValue)
                {
                case ValueType::int32_t:
                    _hr = ParserInt32Value(_pProp, &_ExprNode, &_Value);
                    break;
                case ValueType::boolean:
                    _hr = ParserBoolValue(_pProp, &_ExprNode, &_Value);
                    break;
                case ValueType::uString:
                    _hr = ParserStringValue(_pProp, &_ExprNode, &_Value);
                    break;
                case ValueType::POINT:
                    _hr = ParserPointValue(_pProp, &_ExprNode, &_Value);
                    break;
                case ValueType::SIZE:
                    _hr = ParserSizeValue(_pProp, &_ExprNode, &_Value);
                    break;
                case ValueType::Rect:
                    _hr = ParserRectValue(_pProp, &_ExprNode, &_Value);
                    break;
                case ValueType::Color:
                    _hr = ParserColorValue(_pProp, &_ExprNode, &_Value);
                    break;
                default:
                    break;
                }

                // 格式不支持，
                if (_hr == __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT))
                    continue;

                if (FAILED(_hr))
                    return _hr;

                uint_t _uIndex;
                auto _ppBufer = LocalValueCache.AddAndGetPtr(&_uIndex);
                if (!_ppBufer)
                    return E_OUTOFMEMORY;

                *_ppBufer = std::move(_Value);

                SetProperty SetPropertyByteCode;
                SetPropertyByteCode.uIndexOfPropertyValue = _uIndex;
                SetPropertyByteCode.pProp = _pProp;
                _hr = _pRecorder->ByteCode.Add((uint8_t*)&SetPropertyByteCode, sizeof(SetPropertyByteCode));
                if (FAILED(_hr))
                    return _hr;

                return S_OK;
            }

            return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
        }

        HRESULT __MEGA_UI_API ParserInt32Value(const u8StringView& _szValue, int32_t* _pValue)
        {
            int32_t _iValue = 0;

            // 16进制 0x???
            if (_szValue.GetSize() >= 3 && _szValue[0] == '0' && (_szValue[1] == 'x' || _szValue[1] == 'X'))
            {
                for (uint_t _uIndex = 2; _uIndex != _szValue.GetSize(); ++_uIndex)
                {
                    auto _ch = _szValue[_uIndex];

                    if (_ch >= '0' && _ch <= '9')
                    {
                        _iValue <<= 4;
                        _iValue |= _ch - '0';
                    }
                    else if (_ch >= 'A' && _ch <= 'F')
                    {
                        _iValue <<= 4;
                        _iValue |= _ch - 'A' + 10;
                    }
                    else if (_ch >= 'a' && _ch <= 'f')
                    {
                        _iValue <<= 4;
                        _iValue |= _ch - 'a' + 10;
                    }
                    else
                    {
                        return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                    }
                }
            }
            else
            {
                // 10进制
                uint_t _uIndex = 0;
                bool _bNegative = _szValue[0] == '-';
                if (_bNegative)
                    ++_uIndex;

                for (; _uIndex != _szValue.GetSize(); ++_uIndex)
                {
                    auto _ch = _szValue[_uIndex];

                    if (_ch >= '0' && _ch <= '9')
                    {
                        _iValue *= 10;
                        _iValue += _ch - '0';
                    }
                    else
                    {
                        return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                    }
                }

                if (_bNegative)
                    _iValue *= -1;
            }

            *_pValue = _iValue;
            return S_OK;
        }

        HRESULT __MEGA_UI_API ParserInt32Value(const PropertyInfo* _pProp, ExprNode* _pExprNode, int32_t* _pValue)
        {
            if (_pExprNode->Type == ExprNodeType::Or)
            {
                if (_pExprNode->ChildExprNode.GetSize() == 0)
                    return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

                for (auto& Expr : _pExprNode->ChildExprNode)
                {
                    int32_t _iValue = 0;
                    auto _hr = ParserInt32Value(_pProp, &Expr, &_iValue);
                    if (FAILED(_hr))
                        return _hr;

                    *_pValue |= _iValue;
                }

                return S_OK;
            }
            else if (_pExprNode->Type == ExprNodeType::BaseIdentifier)
            {
                auto szValue = _pExprNode->szValue;
                if (szValue.GetSize() == 0)
                    return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                
                if (SUCCEEDED(ParserInt32Value(_pExprNode->szValue, _pValue)))
                    return S_OK;

                // 转换失败，但是Int32 可能映射到枚举值
                if (_pProp && _pProp->pEnumMaps)
                {
                    for (auto _pEnum = _pProp->pEnumMaps; _pEnum->pszEnum; ++_pEnum)
                    {
                        if (_pExprNode->szValue.CompareI((u8char_t*)_pEnum->pszEnum) == 0)
                        {
                            *_pValue = _pEnum->nEnum;
                            return S_OK;
                        }
                    }
                }
            }
            
            return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
        }
        
        HRESULT __MEGA_UI_API ParserStringValue(const PropertyInfo* _pProp, ExprNode* _pExprNode, u8StringView* _pValue)
        {
            if (_pExprNode->Type == ExprNodeType::Root)
            {
                if (_pExprNode->ChildExprNode.GetSize() == 0)
                    return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

                auto _hr = ParserStringValue(_pProp, &_pExprNode->ChildExprNode[0], _pValue);
                if (SUCCEEDED(_hr) || _hr != __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT))
                    return _hr;

                *_pValue = _pExprNode->szValue;
                return S_OK;
            }
            else if (_pExprNode->Type == ExprNodeType::BaseIdentifier)
            {
                *_pValue = _pExprNode->szValue;
                return S_OK;
            }
            else
            {
                return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
            }
        }

        HRESULT __MEGA_UI_API UIParser::ParserInt32Value(const PropertyInfo* _pProp, ExprNode* _pExprNode, Value* _pValue)
        {
            if (_pExprNode->Type == ExprNodeType::Root)
            {
                if (_pExprNode->ChildExprNode.GetSize() == 0)
                    return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                
                _pExprNode = &_pExprNode->ChildExprNode[0];
            }

            int32_t _iValue = 0;
            auto _hr = YY::MegaUI::ParserInt32Value(_pProp, _pExprNode, &_iValue);
            if(FAILED(_hr))
                return _hr;

            auto _Value = Value::CreateInt32(_iValue);
            if (_Value == nullptr)
                return E_OUTOFMEMORY;

            *_pValue = _Value;
            return S_OK;
        }
        
        HRESULT __MEGA_UI_API UIParser::ParserBoolValue(const PropertyInfo* _pProp, ExprNode* _pExprNode, Value* _pValue)
        {
            if (_pExprNode->Type == ExprNodeType::Root)
            {
                if (_pExprNode->ChildExprNode.GetSize() == 0)
                    return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

                _pExprNode = &_pExprNode->ChildExprNode[0];
            }

            if (_pExprNode->Type == ExprNodeType::BaseIdentifier)
            {
                if (_pExprNode->szValue.CompareI((u8char_t*)"true") == 0)
                {
                    *_pValue = Value::CreateBool(true);
                    return S_OK;
                }
                else if (_pExprNode->szValue.CompareI((u8char_t*)"false") == 0)
                {
                    *_pValue = Value::CreateBool(false);
                    return S_OK;
                }
            }

            return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
        }
        
        HRESULT __MEGA_UI_API UIParser::ParserStringValue(const PropertyInfo* _pProp, ExprNode* _pExprNode, Value* _pValue)
        {
            u8StringView _ValueView;
            auto _hr = YY::MegaUI::ParserStringValue(_pProp, _pExprNode, &_ValueView);
            if (FAILED(_hr))
                return _hr;

            uString _szTmp;
            _hr = Transform(_ValueView, &_szTmp);
            if (FAILED(_hr))
                return _hr;

            auto _Value = Value::CreateString(_szTmp);
            if (_Value == nullptr)
                return E_OUTOFMEMORY;
            *_pValue = _Value;
            return S_OK;
        }

        HRESULT __MEGA_UI_API UIParser::ParserFunction(aStringView _szFunctionName, ExprNode* _pExprNode, aStringView _szFormat, ParsedArg* _pArg, uint_t _uArgCount)
        {
            if (_pExprNode->Type != ExprNodeType::Funcall)
                return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

            if (_pExprNode->szValue.GetSize() != _szFunctionName.GetSize() || _strnicmp((char*)_pExprNode->szValue.GetConstString(), _szFunctionName.GetConstString(), _szFunctionName.GetSize()) != 0)
                return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

            if (_pExprNode->ChildExprNode.GetSize() != _szFormat.GetSize())
                return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

            HRESULT _hr = S_OK;

            uint_t _uArgIndex = 0;

            for (auto _ch : _szFormat)
            {
                // 跳过
                if (_ch == ',')
                    continue;

                if (_uArgIndex == _uArgCount)
                    return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

                switch (_ch)
                {
                case '*':
                    // 纯占位
                    ++_uArgIndex;
                    break;
                case 'I':
                    // int32_t
                    _hr = YY::MegaUI::ParserInt32Value(nullptr, &_pExprNode->ChildExprNode[_uArgIndex], &_pArg[_uArgIndex].iNumber);
                    ++_uArgIndex;
                    break;
                case 'S':
                    _hr = YY::MegaUI::ParserStringValue(nullptr, &_pExprNode->ChildExprNode[_uArgIndex], &_pArg[_uArgIndex].szString);
                    ++_uArgIndex;
                    break;
                default:
                    _hr = E_NOTIMPL;
                    break;
                }
                
                if (FAILED(_hr))
                    return _hr;
            }

            return S_OK;
        }

        HRESULT __MEGA_UI_API UIParser::ParserPointValue(const PropertyInfo* _pProp, ExprNode* _pExprNode, Value* _pValue)
        {
            if (_pExprNode->Type == ExprNodeType::Root)
            {
                if (_pExprNode->ChildExprNode.GetSize() == 0)
                    return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

                _pExprNode = &_pExprNode->ChildExprNode[0];
            }

            ParsedArg _Arg[2];

            auto _hr = ParserFunction("Point", _pExprNode, "II", _Arg, std::size(_Arg));
            if (FAILED(_hr))
                return _hr;

            auto _Value = Value::CreatePoint(_Arg[0].iNumber, _Arg[1].iNumber);
            if (_Value == nullptr)
                return E_OUTOFMEMORY;
            *_pValue = std::move(_Value);

            return S_OK;
        }
        
        HRESULT __MEGA_UI_API UIParser::ParserSizeValue(const PropertyInfo* _pProp, ExprNode* _pExprNode, Value* _pValue)
        {
            if (_pExprNode->Type == ExprNodeType::Root)
            {
                if (_pExprNode->ChildExprNode.GetSize() == 0)
                    return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

                _pExprNode = &_pExprNode->ChildExprNode[0];
            }

            ParsedArg _Arg[2];

            auto _hr = ParserFunction("Size", _pExprNode, "II", _Arg);
            if (FAILED(_hr))
                return _hr;

            auto _Value = Value::CreateSize(_Arg[0].iNumber, _Arg[1].iNumber);
            if (_Value == nullptr)
                return E_OUTOFMEMORY;
            *_pValue = std::move(_Value);

            return S_OK;
        }
        
        HRESULT __MEGA_UI_API UIParser::ParserRectValue(const PropertyInfo* _pProp, ExprNode* _pExprNode, Value* _pValue)
        {
            if (_pExprNode->Type == ExprNodeType::Root)
            {
                if (_pExprNode->ChildExprNode.GetSize() == 0)
                    return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

                _pExprNode = &_pExprNode->ChildExprNode[0];
            }

            ParsedArg _Arg[4];

            auto _hr = ParserFunction("Rect", _pExprNode, "IIII", _Arg);
            if (FAILED(_hr))
                return _hr;

            auto _Value = Value::CreateRect(_Arg[0].iNumber, _Arg[1].iNumber, _Arg[2].iNumber, _Arg[3].iNumber);
            if (_Value == nullptr)
                return E_OUTOFMEMORY;
            *_pValue = std::move(_Value);

            return S_OK;
        }
        
        HRESULT __MEGA_UI_API UIParser::ParserColorValue(const PropertyInfo* _pProp, ExprNode* _pExprNode, Value* _pValue)
        {
            if (_pExprNode->Type == ExprNodeType::Root)
            {
                if (_pExprNode->ChildExprNode.GetSize() == 0)
                    return __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

                _pExprNode = &_pExprNode->ChildExprNode[0];
            }

            Color _Color;

            ParsedArg _Arg[4];
            auto _hr = ParserFunction("ARGB", _pExprNode, "IIII", _Arg);
            if (SUCCEEDED(_hr))
            {
                _Color = Color::MakeARGB(_Arg[0].iNumber, _Arg[1].iNumber, _Arg[2].iNumber, _Arg[3].iNumber);
            }
            else if (_hr == __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT))
            {
                _hr = ParserFunction("RGB", _pExprNode, "III", _Arg);
                if (SUCCEEDED(_hr))
                    _Color = Color::MakeRGB(_Arg[0].iNumber, _Arg[1].iNumber, _Arg[2].iNumber);
            }

            if (FAILED(_hr))
                return _hr;

            auto _Value = Value::CreateColor(_Color);
            if (_Value == nullptr)
                return E_OUTOFMEMORY;
            *_pValue = std::move(_Value);
            return S_OK;
        }
        
        HRESULT __MEGA_UI_API UIParser::Play(ArrayView<uint8_t>& _ByteCode, Element* _pTopElement, intptr_t* _pCooike, DynamicArray<Element*, false, false>* _pElementArray)
        {
            Element* _pCurrentElement = nullptr;
            HRESULT _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);

            for (;;)
            {
                if (_ByteCode.GetSize() == 0)
                {
                    _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                    break;
                }

                auto _pByteCodeBase = (ByteCodeBase*)_ByteCode.GetData();

                if (_ByteCode.GetSize() < sizeof(ByteCodeBase) || _pByteCodeBase->cbData > _ByteCode.GetSize() || _pByteCodeBase->cbData < sizeof(ByteCodeBase))
                {
                    _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                    break;
                }

                if (_pByteCodeBase->Type == ByteCodeBase::ChildElementBegin)
                {
                    if (_pCurrentElement == nullptr || _pByteCodeBase->cbData < sizeof(ChildElementBegin))
                    {
                        _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                        break;
                    }
                    _ByteCode.Slice(_pByteCodeBase->cbData);

                    DynamicArray<Element*, false, false> _Child;

                    _hr = Play(_ByteCode, _pTopElement, _pCooike, & _Child);
                    if (FAILED(_hr))
                        break;

                    _hr = _pCurrentElement->Add(_Child.GetData(), _Child.GetSize());
                    if (FAILED(_hr))
                        break;
                }
                else if (_pByteCodeBase->Type == ByteCodeBase::ChildElementEnd)
                {
                    if (_pByteCodeBase->cbData < sizeof(ChildElementEnd))
                    {
                        _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                        break;
                    }
                    _ByteCode.Slice(_pByteCodeBase->cbData);

                    if (_ByteCode.GetSize() == 0)
                    {
                        _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                        break;
                    }

                    return S_OK;
                }
                else if (_pByteCodeBase->Type == ByteCodeBase::ByteCodeEnd)
                {
                    if (_pByteCodeBase->cbData < sizeof(ByteCodeEnd))
                    {
                        _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                        break;
                    }
                    _ByteCode.Slice(_pByteCodeBase->cbData);

                    if (_ByteCode.GetSize() != 0)
                    {
                        _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                        break;
                    }

                    return S_OK;
                }
                else if (_pByteCodeBase->Type == ByteCodeBase::CreateElement)
                {
                    if (_pByteCodeBase->cbData < sizeof(CreateElement))
                    {
                        _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                        break;
                    }
                    _ByteCode.Slice(_pByteCodeBase->cbData);

                    auto _pCreateElement = (CreateElement*)_pByteCodeBase;
                    auto _ppClass = ClassArray.GetItemPtr(_pCreateElement->uIndexOfElementName);

                    if (!_ppClass)
                    {
                        _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                        break;
                    }
                    auto _pClass = *_ppClass;

                    _hr = _pClass->CreateInstance(_pTopElement, _pCooike, &_pCurrentElement);
                    if (FAILED(_hr))
                        break;

                    if (!_pElementArray->EmplacePtr(_pCurrentElement))
                    {
                        HDelete(_pCurrentElement);
                        _pCurrentElement = nullptr;
                        _hr = E_OUTOFMEMORY;
                        break;
                    }
                }
                else if (_pByteCodeBase->Type == ByteCodeBase::SetProperty)
                {
                    if (_pCurrentElement == nullptr || _pByteCodeBase->cbData < sizeof(SetProperty))
                    {
                        _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                        break;
                    }
                    _ByteCode.Slice(_pByteCodeBase->cbData);

                    auto _pSetProperty = (SetProperty*)_pByteCodeBase;

                    auto _ppValue = LocalValueCache.GetItemPtr(_pSetProperty->uIndexOfPropertyValue);
                    if (!_ppValue)
                    {
                        _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                        break;
                    }

                    // 忽略错误
                    _pCurrentElement->SetValue(*_pSetProperty->pProp, PropertyIndicies::PI_Local, *_ppValue);
                }
                else
                {
                    _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                    break;
                }
            }

            for (auto _pElement : *_pElementArray)
            {
                _pElement->Destroy(false);
            }

            _pElementArray->Clear();

            return _hr;
        }
    } // namespace MegaUI
} // namespace YY