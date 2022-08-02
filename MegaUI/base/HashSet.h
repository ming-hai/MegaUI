﻿#pragma once

#include "MegaUITypeInt.h"
#include "alloc.h"

#pragma pack(push, __MEGA_UI_PACKING)

namespace YY
{
    namespace MegaUI
    {
        // 暂时仅适用于指针
        template<typename _Key, uint32_t _uBuckets = 128>
        class HashSet
        {
        private:
            struct HashSetEntry
            {
                HashSetEntry* pNext;
                _Key Data;
            };

            HashSetEntry* pBuckets[_uBuckets];

        public:
            HashSet()
                : pBuckets {}
            {
            }

            ~HashSet()
            {
                for (auto _pBucket : pBuckets)
                {
                    for (;;)
                    {
                        if (!_pBucket)
                            break;

                        auto _pTmp = _pBucket;
                        _pBucket = _pBucket->pNext;
                        HFree(_pTmp);
                    }
                }
            }

            _Key Pop()
            {
                for (auto& _pBucket : pBuckets)
                {
                    if (_pBucket)
                    {
                        auto _Tmp = _pBucket;
                        _pBucket = _Tmp->pNext;
                        return _Tmp->Data;
                    }
                }

                return nullptr;
            }

            HRESULT __fastcall Insert(_Key _pKey)
            {
                const auto _uIndex = reinterpret_cast<uint_t>(_pKey) % _uBuckets;

                for (auto _pEntry = pBuckets[_uIndex]; _pEntry; _pEntry = _pEntry->pNext)
                {
                    if (_pEntry->Data == _pKey)
                        return S_FALSE;
                }

                auto pNewEntry = (HashSetEntry*) HAlloc(sizeof(HashSetEntry));
                if (!pNewEntry)
                    return E_OUTOFMEMORY;

                auto& _FirstEntry = pBuckets[_uIndex];

                pNewEntry->Data = _pKey;
                pNewEntry->pNext = _FirstEntry;
                _FirstEntry = pNewEntry;

                return S_OK;
            }
        };
    }
} // namespace YY