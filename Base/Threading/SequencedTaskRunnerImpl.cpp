#include "pch.h"
#include "SequencedTaskRunnerImpl.h"

#include <MegaUI/Base/ErrorCode.h>
#include <Base/Sync/Sync.h>
#include <Base/Threading/ProcessThreads.h>
#include <Base/Memory/WeakPtr.h>

__YY_IGNORE_INCONSISTENT_ANNOTATION_FOR_FUNCTION()

namespace YY::Base::Threading
{
    SequencedTaskRunnerImpl::SequencedTaskRunnerImpl()
        : uTaskRunnerId(GenerateNewTaskRunnerId())
        , uThreadId(0u)
        , uWeakupCountAndPushLock(0u)
    {
    }

    SequencedTaskRunnerImpl::~SequencedTaskRunnerImpl()
    {
        CleanupTaskQueue();
    }

    uint32_t __YYAPI SequencedTaskRunnerImpl::GetId()
    {
        return uTaskRunnerId;
    }

    TaskRunnerStyle __YYAPI SequencedTaskRunnerImpl::GetStyle()
    {
        return TaskRunnerStyle::None;
    }

#ifdef _WIN32
    HRESULT __YYAPI SequencedTaskRunnerImpl::PostTaskInternal(RefPtr<TaskEntry> _pTask)
    {
        _pTask->hr = E_PENDING;

        if (bStopWeakup)
        {
            _pTask->Wakeup(YY::Base::HRESULT_From_LSTATUS(ERROR_CANCELLED));
            return E_FAIL;
        }

        for (;;)
        {
            if (!Sync::BitSet(&uWeakupCountAndPushLock, LockedQueuePushBitIndex))
            {
                oTaskQueue.Push(_pTask.Detach());
                break;
            }
        }

        // 我们 解除锁定，uPushLock = 0 并且将 uWeakCount += 1
        // 因为刚才 uWeakupCountAndPushLock 已经将第一个标记位设置位 1
        // 所以我们再 uWeakupCountAndPushLock += 1即可。
        // uWeakCount + 1 <==> uWeakupCountAndPushLock + 2 <==> (uWeakupCountAndPushLock | 1) + 1
        if (Sync::Add(&uWeakupCountAndPushLock, UnlockQueuePushLockBitAndWeakupOnceRaw) < WeakupOnceRaw * 2u)
        {
            WeakPtr<SequencedTaskRunnerImpl> _pWeak = this;
            auto _bRet = TrySubmitThreadpoolCallback(
                [](_Inout_ PTP_CALLBACK_INSTANCE Instance, _Inout_opt_ PVOID Context) -> void
                {
                    WeakPtr<SequencedTaskRunnerImpl> _pWeak;
                    _pWeak.Attach(reinterpret_cast<WeakPtrRef<SequencedTaskRunnerImpl>*>(Context));

                    auto pSequencedTaskRunner = _pWeak.Get();
                    if (pSequencedTaskRunner)
                    {
                        pSequencedTaskRunner->ExecuteTaskRunner();
                    }
                },
                _pWeak.Detach(),
                nullptr);

            if (!_bRet)
            {
                // 阻止后续再唤醒线程
                Sync::BitSet(&uWeakupCountAndPushLock, StopWeakupBitIndex);
                auto _hr = YY::Base::HRESULT_From_LSTATUS(GetLastError());
                CleanupTaskQueue();
                ReleaseWeak();
                return _hr;
            }
        }

        return S_OK;
    }
#endif

    void __YYAPI SequencedTaskRunnerImpl::ExecuteTaskRunner()
    {
        uThreadId = GetCurrentThreadId();
        g_pTaskRunner = this;

        for (;;)
        {
            // 理论上 ExecuteTaskRunner 执行时引用计数 = 2，因为执行器拥有一次引用计数
            // 如果为 1 (IsShared() == false)，那么说明用户已经释放了这个 TaskRunner
            // 这时我们需要及时的退出，随后会将队列里的任务全部取消释放内存。
            if (!IsShared())
                break;

            auto _pTask = RefPtr<TaskEntry>::FromPtr(oTaskQueue.Pop());
            if (!_pTask)
                break;
                    
            _pTask->operator()();
            _pTask->Wakeup(S_OK);

            if (Sync::Subtract(&uWeakupCountAndPushLock, WeakupOnceRaw) < WeakupOnceRaw)
                break;
        }
        g_pTaskRunner = nullptr;
        uThreadId = 0;
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

            if (Sync::Subtract(&uWeakupCountAndPushLock, WeakupOnceRaw) < WeakupOnceRaw)
                break;
        }
        return;
    }
} // namespace YY
