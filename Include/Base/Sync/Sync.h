﻿#pragma once
#include <Base/YY.h>
#include <Base/Time/TimeSpan.h>

namespace YY
{
    namespace Base
    {
        namespace Sync
        {
#ifdef _WIN32
            using ::WaitOnAddress;
            using ::WakeByAddressSingle;
            using ::WakeByAddressAll;

#pragma comment(lib, "Synchronization.lib")

#else // !_WIN32
            bool
            __YYAPI
            WaitOnAddress(
                _In_reads_bytes_(AddressSize) volatile void* Address,
                _In_reads_bytes_(AddressSize) void* CompareAddress,
                _In_ size_t AddressSize,
                _In_opt_ uint32_t dwMilliseconds
                );

            void
            __YYAPI
            WakeByAddressSingle(
                    _In_ void* Address
                );

            void
            __YYAPI
            WakeByAddressAll(
                _In_ void* Address
                );
#endif // _WIN32

            /// <summary>
            /// 等待 Address的值等于 CompareAddress
            /// </summary>
            /// <param name="Address"></param>
            /// <param name="CompareAddress"></param>
            /// <param name="AddressSize"></param>
            /// <param name="dwMilliseconds"></param>
            /// <returns></returns>
            bool __YYAPI WaitEqualOnAddress(
                _In_reads_bytes_(AddressSize) volatile void* Address,
                _In_reads_bytes_(AddressSize) void* CompareAddress,
                _In_ size_t AddressSize,
                _In_ TimeSpan<TimePrecise::Millisecond> _uMilliseconds = TimeSpan<TimePrecise::Millisecond>::GetMax()) noexcept;
        }
    } // namespace Base

    using namespace YY::Base::Sync;
} // namespace YY
