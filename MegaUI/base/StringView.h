﻿#pragma once
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <crtdbg.h>

#include "MegaUITypeInt.h"
#include "Encoding.h"

#pragma pack(push, __MEGA_UI_PACKING)

namespace YY
{
    namespace MegaUI
    {
        __forceinline constexpr uint_t __MEGA_UI_API GetStringLength(const achar_t* _szSrc)
        {
            return _szSrc ? strlen(_szSrc) : 0;
        }

        __forceinline constexpr uint_t __MEGA_UI_API GetStringLength(const u8char_t* _szSrc)
        {
            return _szSrc ? strlen((const char*)_szSrc) : 0;
        }

        __forceinline constexpr uint_t __MEGA_UI_API GetStringLength(const u16char_t* _szSrc)
        {
            return _szSrc ? wcslen(_szSrc) : 0;
        }

        __forceinline constexpr uint_t __MEGA_UI_API GetStringLength(const u32char_t* _szSrc)
        {
            if (!_szSrc)
                return 0;
            auto _pScan = _szSrc;
            for (; *_pScan; ++_pScan);

            return _pScan - _szSrc;
        }

        __forceinline constexpr bool __MEGA_UI_API IsEmptyString(const achar_t* _szSrc)
        {
            return _szSrc == nullptr || _szSrc[0] == achar_t('\0');
        }

        __forceinline constexpr bool __MEGA_UI_API IsEmptyString(const u8char_t* _szSrc)
        {
            return _szSrc == nullptr || _szSrc[0] == u8char_t('\0');
        }

        __forceinline constexpr bool __MEGA_UI_API IsEmptyString(const u16char_t* _szSrc)
        {
            return _szSrc == nullptr || _szSrc[0] == u16char_t('\0');
        }

        __forceinline constexpr bool __MEGA_UI_API IsEmptyString(const u32char_t* _szSrc)
        {
            return _szSrc == nullptr || _szSrc[0] == u32char_t('\0');
        }
        
        __forceinline constexpr achar_t __MEGA_UI_API CharUpperASCII(achar_t _ch)
        {
            if (_ch >= achar_t('a') && _ch <= achar_t('z'))
            {
                _ch = _ch - achar_t('a') + achar_t('A');
            }
            return _ch;
        }

        __forceinline constexpr u8char_t __MEGA_UI_API CharUpperASCII(u8char_t _ch)
        {
            if (_ch >= u8char_t('a') && _ch <= u8char_t('z'))
            {
                _ch = _ch - u8char_t('a') + u8char_t('A');
            }
            return _ch;
        }
        
        __forceinline constexpr u16char_t __MEGA_UI_API CharUpperASCII(u16char_t _ch)
        {
            if (_ch >= u16char_t('a') && _ch <= u16char_t('z'))
            {
                _ch = _ch - u16char_t('a') + u16char_t('A');
            }
            return _ch;
        }

        __forceinline constexpr u32char_t __MEGA_UI_API CharUpperASCII(u32char_t _ch)
        {
            if (_ch >= u32char_t('a') && _ch <= u32char_t('z'))
            {
                _ch = _ch - u32char_t('a') + u32char_t('A');
            }
            return _ch;
        }

        __forceinline auto __cdecl GetStringFormatLength(
            _In_z_ _Printf_format_string_ achar_t const* const _Format,
                                          va_list              _ArgList
            )
        {
            return _vscprintf(_Format, _ArgList);
        }

        __forceinline auto __cdecl GetStringFormatLength(
            _In_z_ _Printf_format_string_ u8char_t const* const _Format,
                                          va_list               _ArgList
            )
        {
            return _vscprintf((const achar_t*)_Format, _ArgList);
        }

        __forceinline auto __cdecl GetStringFormatLength(
            _In_z_ _Printf_format_string_ wchar_t const* const _Format,
                                          va_list              _ArgList
            )
        {
            return _vscwprintf(_Format, _ArgList);
        }

        _Success_(return >= 0)
        __forceinline auto __cdecl FormatStringV(
            _Out_writes_(_BufferCount) _Always_(_Post_z_) achar_t* const       _Buffer,
            _In_                                          size_t const         _BufferCount,
            _In_z_ _Printf_format_string_                 achar_t const* const _Format,
                                                          va_list              _ArgList
            )
        {
            return vsprintf_s(_Buffer, _BufferCount, _Format, _ArgList);
        }

        _Success_(return >= 0)
        __forceinline auto __cdecl FormatStringV(
            _Out_writes_(_BufferCount) _Always_(_Post_z_) u8char_t* const       _Buffer,
            _In_                                          size_t const          _BufferCount,
            _In_z_ _Printf_format_string_                 u8char_t const* const _Format,
                                                          va_list               _ArgList
            )
        {
            return vsprintf_s((achar_t*)_Buffer, _BufferCount, (achar_t*)_Format, _ArgList);
        }

        _Success_(return >= 0)
        __forceinline auto __cdecl FormatStringV(
            _Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t* const       _Buffer,
            _In_                                          size_t const         _BufferCount,
            _In_z_ _Printf_format_string_                 wchar_t const* const _Format,
                                                          va_list              _ArgList
            )
        {
            return vswprintf_s(_Buffer, _BufferCount, _Format, _ArgList);
        }

        template<typename _char_t, Encoding _eEncoding>
        class StringView
        {
        public:
            using char_t = _char_t;

        private:
            const char_t* szString;
            uint_t cchString;
            constexpr static Encoding eEncoding = _eEncoding;

        public:
            StringView()
                : szString(nullptr)
                , cchString(0)
            {
            }

            StringView(_In_reads_opt_(_cchSrc) const char_t* _szSrc, _In_ uint_t _cchSrc)
                : szString(_szSrc)
                , cchString(_szSrc ? _cchSrc : 0)
            {
            }

            StringView(_In_opt_z_ const char_t* _szSrc)
                : szString(_szSrc)
                , cchString(GetStringLength(_szSrc))
            {
            }

            template<uint_t _uArrayCount>
            StringView(const char_t (&_szSrc)[_uArrayCount])
                : szString(_szSrc)
                , cchString(_uArrayCount - 1)
            {
            }

            HRESULT __MEGA_UI_API SetString(_In_reads_opt_(_cchSrc) const char_t* _szSrc, _In_ uint_t _cchSrc)
            {
                if (_szSrc == nullptr && _cchSrc)
                    return E_INVALIDARG;

                szString = _szSrc;
                cchString = _cchSrc;
                return S_OK;
            }

            constexpr static Encoding __MEGA_UI_API GetEncoding()
            {
                return eEncoding;
            }

            __forceinline uint_t __MEGA_UI_API GetSize() const
            {
                return cchString;
            }

            __forceinline
            _Ret_notnull_ _Post_readable_size_(cchString)
            const char_t* __MEGA_UI_API GetConstString() const
            {
                return szString;
            }

            __forceinline char_t __MEGA_UI_API operator[](_In_ uint_t _uIndex) const
            {
                _ASSERTE(_uIndex < GetSize());

                return szString[_uIndex];
            }

            __forceinline const char_t* __MEGA_UI_API begin() const
            {
                return this->GetConstString();
            }

            __forceinline const char_t* __MEGA_UI_API end() const
            {
                return this->GetConstString() + this->GetSize();
            }

            bool __MEGA_UI_API operator==(StringView _Other)
            {
                if (cchString != _Other.cchString)
                    return false;

                return memcmp(szString, _Other.szString, cchString * sizeof(szString[0])) == 0;
            }

            bool __MEGA_UI_API operator==(const char_t* _Other)
            {
                if (_Other == nullptr && cchString == 0)
                    return true;
                uint_t _uIndex = 0;
                for (; _uIndex != GetSize(); ++_uIndex)
                {
                    if (szString[_uIndex] != _Other[_uIndex])
                        return false;
                }

                return _Other[_uIndex] == char_t('\0');
            }

            int __MEGA_UI_API Compare(const char_t* _Other)
            {
                if (_Other == nullptr && cchString == 0)
                    return 0;

                uint_t _uIndex = 0;
                for (; _uIndex != GetSize(); ++_uIndex)
                {
                    auto _result = szString[_uIndex] - _Other[_uIndex];
                    if (_result != 0)
                        return _result;
                }

                return char_t('\0') - _Other[_uIndex];
            }
            
            int __MEGA_UI_API CompareI(const char_t* _Other)
            {
                if (_Other == nullptr && cchString == 0)
                    return 0;

                uint_t _uIndex = 0;
                for (; _uIndex != GetSize(); ++_uIndex)
                {
                    auto _result = CharUpperASCII(szString[_uIndex]) - CharUpperASCII(_Other[_uIndex]);
                    if (_result != 0)
                        return _result;
                }

                return char_t('\0') - _Other[_uIndex];
            }
        };

        template<>
        class StringView<achar_t, Encoding::ANSI>
        {
        public:
            using char_t = achar_t;

        private:
            _Field_size_(cchString) const char_t* szString;
            uint_t cchString;
            Encoding eEncoding;

        public:
            StringView()
                : szString(nullptr)
                , cchString(0)
                , eEncoding(Encoding::ANSI)
            {
            }

            StringView(_In_reads_opt_(_cchSrc) const char_t* _szSrc, _In_ uint_t _cchSrc, _In_ Encoding _eEncoding = Encoding::ANSI)
                : szString(_szSrc)
                , cchString(_cchSrc)
                , eEncoding(_eEncoding)
            {
            }

            StringView(_In_opt_z_ const char_t* _szSrc, _In_ Encoding _eEncoding = Encoding::ANSI)
                : szString(_szSrc)
                , cchString(GetStringLength(_szSrc))
                , eEncoding(_eEncoding)
            {
            }

            template<uint_t _uArrayCount>
            StringView(const char_t (&_szSrc)[_uArrayCount], _In_ Encoding _eEncoding = Encoding::ANSI)
                : szString(_szSrc)
                , cchString(_uArrayCount - 1)
                , eEncoding(_eEncoding)
            {
            }

            __forceinline Encoding __MEGA_UI_API GetEncoding() const
            {
                return eEncoding;
            }

            __forceinline uint_t __MEGA_UI_API GetSize() const
            {
                return cchString;
            }

            __forceinline _Ret_notnull_ _Post_readable_size_(cchString)
            const char_t* __MEGA_UI_API GetConstString() const
            {
                return szString;
            }

            __forceinline char_t __MEGA_UI_API operator[](_In_ uint_t _uIndex) const
            {
                _ASSERTE(_uIndex < GetSize());

                return szString[_uIndex];
            }

            __forceinline const char_t* __MEGA_UI_API begin() const
            {
                return this->GetConstString();
            }

            __forceinline const char_t* __MEGA_UI_API end() const
            {
                return this->GetConstString() + this->GetSize();
            }
        };

        typedef StringView<achar_t, Encoding::ANSI> aStringView;
        typedef StringView<u8char_t, Encoding::UTF8> u8StringView;
        typedef StringView<u16char_t, Encoding::UTF16LE> u16StringLEView;
        typedef StringView<u16char_t, Encoding::UTF16BE> u16StringBEView;
        typedef StringView<u32char_t, Encoding::UTF32LE> u32StringLEView;
        typedef StringView<u32char_t, Encoding::UTF32BE> u32StringBEView;

        typedef StringView<u16char_t, Encoding::UTF16> u16StringView;
        typedef StringView<u32char_t, Encoding::UTF32> u32StringView;
    } // namespace MegaUI
} // namespace YY

#pragma pack(pop)
