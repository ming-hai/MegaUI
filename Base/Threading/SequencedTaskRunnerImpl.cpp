﻿#include "pch.h"
#include "SequencedTaskRunnerImpl.h"

#include <Base/ErrorCode.h>
#include <Base/Sync/Sync.h>
#include <Base/Threading/ProcessThreads.h>
#include <Base/Memory/WeakPtr.h>

__YY_IGNORE_INCONSISTENT_ANNOTATION_FOR_FUNCTION()

namespace YY::Base::Threading
{
    SequencedTaskRunnerImpl::SequencedTaskRunnerImpl()
        : uWakeupCountAndPushLock(0u)
    {
    }

    SequencedTaskRunnerImpl::~SequencedTaskRunnerImpl()
    {
        CleanupTaskQueue();
    }

    TaskRunnerStyle __YYAPI SequencedTaskRunnerImpl::GetStyle() const noexcept
    {
        return TaskRunnerStyle::Sequenced;
    }

    HRESULT __YYAPI SequencedTaskRunnerImpl::PostTaskInternal(RefPtr<TaskEntry> _pTask)
    {
        _pTask->hr = E_PENDING;

        auto _fFlags = uWakeupCountAndPushLock;
        for (;;)
        {
            if (_fFlags & (1 << StopWakeupBitIndex))
            {
                return YY::Base::HRESULT_From_LSTATUS(ERROR_CANCELLED);
            }
            else if (_fFlags & (1 << LockedQueuePushBitIndex))
            {
                YieldProcessor();
                _fFlags = uWakeupCountAndPushLock;
                continue;
            }

            const auto _uLast = Sync::CompareExchange(&uWakeupCountAndPushLock, _fFlags + LockQueuePushLockAndWakeupOnceRaw, _fFlags);
            if (_uLast == _fFlags)
            {
                oTaskQueue.Push(_pTask.Detach());
                Sync::BitReset(&uWakeupCountAndPushLock, LockedQueuePushBitIndex);
                break;
            }
            _fFlags = _uLast;
        }

        // 小于 WakeupOnceRaw说明之前是没有线程了
        if (_fFlags < WakeupOnceRaw)
        {
            auto _hr = ThreadPool::PostTaskInternal(this);
            if (FAILED(_hr))
            {
                // 阻止后续再唤醒线程
                Sync::BitSet(&uWakeupCountAndPushLock, StopWakeupBitIndex);
                CleanupTaskQueue();
                return _hr;
            }
        }
        return S_OK;
    }

    void __YYAPI SequencedTaskRunnerImpl::operator()()
    {
        ExecuteTaskRunner();
    }

    void __YYAPI SequencedTaskRunnerImpl::ExecuteTaskRunner()
    {
        g_pTaskRunnerWeak = this;

        for (;;)
        {
            // 理论上 ExecuteTaskRunner 执行时引用计数 = 2，因为执行器拥有一次引用计数
            // 如果为 1 (IsShared() == false)，那么说明用户已经释放了这个 TaskRunner
            // 这时我们需要及时的退出，随后会将队列里的任务全部取消释放内存。
            if (!IsShared())
                break;

            auto _pTask = oTaskQueue.Pop();
            if (!_pTask)
                break;
            _pTask->operator()();
            _pTask->Release();
            if (Sync::Subtract(&uWakeupCountAndPushLock, WakeupOnceRaw) < WakeupOnceRaw)
                break;
        }
        g_pTaskRunnerWeak = nullptr;
        return;
    }
            
    void __YYAPI SequencedTaskRunnerImpl::CleanupTaskQueue() noexcept
    {
        for (;;)
        {
            auto _pTask = RefPtr<TaskEntry>::FromPtr(oTaskQueue.Pop());
            if (!_pTask)
                break;

            _pTask->Wakeup(YY::Base::HRESULT_From_LSTATUS(ERROR_CANCELLED));

            if (Sync::Subtract(&uWakeupCountAndPushLock, WakeupOnceRaw) < WakeupOnceRaw)
                break;
        }
        return;
    }
} // namespace YY
