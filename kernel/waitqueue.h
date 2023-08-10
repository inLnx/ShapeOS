/**
 * @file waitqueue.h
 * @author Krisna Pranav
 * @brief wait queue
 * @version 1.0
 * @date 2023-08-10
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once 

#include <mods/atomic.h>
#include <kernel/spinlock.h>
#include <kernel/thread.h>

namespace Kernel
{

    class WaitQueue : public Thread::BlockCondition
    {
    public:
        
        /**
         * @tparam Args 
         * @param timeout 
         * @param args 
         * @return Thread::BlockResult 
         */
        template<class... Args>
        Thread::BlockResult wait_on(const Thread::BlockTimeout& timeout, Args&&... args)
        {
            return Thread::current()->block<Thread::QueueBlocker>(timeout, *this, forward<Args>(args)...);
        }

    protected:
        /**
         * @param b 
         * @param data 
         * @return true 
         * @return false 
         */
        virtual bool should_add_blocker(Thread::Blocker& b, void* data) override;

    private:
        bool m_wake_requested { false };
    }; // class WaitQueue

} // namespace Kernel