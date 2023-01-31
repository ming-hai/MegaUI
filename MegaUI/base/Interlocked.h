﻿#pragma once

#include <intrin.h>

#include "MegaUITypeInt.h"

namespace YY
{
    namespace MegaUI
    {
        namespace Interlocked
        {
            __forceinline uint32_t __MEGA_UI_API Increment(uint32_t* _Addend)
            {
                return _InterlockedIncrement(reinterpret_cast<long volatile*>(_Addend));
            }

            __forceinline int32_t __MEGA_UI_API Increment(int32_t* _Addend)
            {
                return _InterlockedIncrement(reinterpret_cast<long volatile*>(_Addend));
            }

            __forceinline uint32_t __MEGA_UI_API Decrement(uint32_t* _Addend)
            {
                return _InterlockedDecrement(reinterpret_cast<long volatile*>(_Addend));
            }
            
            __forceinline int32_t __MEGA_UI_API Decrement(int32_t* _Addend)
            {
                return _InterlockedDecrement(reinterpret_cast<long volatile*>(_Addend));
            }

            __forceinline bool __MEGA_UI_API BitTestAndSet(uint32_t* _Base, uint32_t _Offset)
            {
                return _interlockedbittestandset(reinterpret_cast<long volatile*>(_Base), _Offset);
            }

            __forceinline bool __MEGA_UI_API BitTestAndReset(uint32_t* _Base, uint32_t _Offset)
            {
                return _interlockedbittestandreset(reinterpret_cast<long volatile*>(_Base), _Offset);
            }

            __forceinline int32_t __MEGA_UI_API CompareExchange(int32_t* _Destination, int32_t _Exchange, int32_t _Comparand)
            {
                return _InterlockedCompareExchange(reinterpret_cast<long volatile*>(_Destination), _Exchange, _Comparand);
            }

            __forceinline int64_t __MEGA_UI_API CompareExchange(int64_t* _Destination, int64_t _Exchange, int64_t _Comparand)
            {
                return _InterlockedCompareExchange64(reinterpret_cast<long long volatile*>(_Destination), _Exchange, _Comparand);
            }

            template<typename _Type>
            __forceinline _Type* __MEGA_UI_API CompareExchangePoint(_Type** _Destination, _Type* _Exchange, _Type* _Comparand)
            {
                return CompareExchange((intptr_t*)_Destination, (intptr_t)_Exchange, (intptr_t)_Comparand);
            }

            __forceinline int32_t __MEGA_UI_API Exchange(int32_t* _Destination, int32_t _Exchange)
            {
                return _InterlockedExchange(reinterpret_cast<long volatile*>(_Destination), _Exchange);
            }

            __forceinline int64_t __MEGA_UI_API Exchange(int64_t* _Destination, int64_t _Exchange)
            {
                return _InterlockedExchange64(reinterpret_cast<long long volatile*>(_Destination), _Exchange);
            }

            template<typename _Type>
            __forceinline _Type* __MEGA_UI_API ExchangePoint(_Type** _Destination, _Type* _Exchange)
            {
                return Exchange((intptr_t*)_Destination, (intptr_t)_Exchange);
            }

        } // namespace Interlocked
    }     // namespace MegaUI
} // namespace YY