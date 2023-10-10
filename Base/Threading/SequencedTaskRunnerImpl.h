﻿#pragma once
#include <Base/Threading/TaskRunnerImpl.h>
#include <Base/Sync/InterlockedQueue.h>

#pragma pack(push, __YY_PACKING)

namespace YY::Base::Threading
{
    class SequencedTaskRunnerImpl : public SequencedTaskRunner
    {
    private:
        InterlockedQueue<TaskEntry> oTaskQueue;

        // |uWakeupCount| bStopWakeup | bPushLock |
        // | 31   ~   2 |     1       |    0      |
        union
        {
            uint32_t uWakeupCountAndPushLock;
            struct
            {
                uint32_t bPushLock : 1;
                uint32_t bStopWakeup : 1;
                uint32_t uWakeupCount : 30;
            };
        };
        enum : uint32_t
        {
            LockedQueuePushBitIndex = 0,
            StopWakeupBitIndex,
            WakeupCountStartBitIndex,
            WakeupOnceRaw = 1 << WakeupCountStartBitIndex,
            UnlockQueuePushLockBitAndWakeupOnceRaw = WakeupOnceRaw - (1u << LockedQueuePushBitIndex),
        };

    public:
        SequencedTaskRunnerImpl();

        ~SequencedTaskRunnerImpl() override;

        SequencedTaskRunnerImpl(const SequencedTaskRunnerImpl&) = delete;

        SequencedTaskRunnerImpl& operator=(const SequencedTaskRunnerImpl&) = delete;

        /////////////////////////////////////////////////////
        // TaskRunner
        virtual TaskRunnerStyle __YYAPI GetStyle() const noexcept override;

    private:
        HRESULT __YYAPI PostTaskInternal(RefPtr<TaskEntry> _pWorkEntry) override;

        void __YYAPI CleanupTaskQueue() noexcept;

        void __YYAPI ExecuteTaskRunner();
    };
} // namespace YY

#pragma pack(pop)
