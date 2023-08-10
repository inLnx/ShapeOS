/**
 * @file doublebuffer.h
 * @author Krisna Pranav
 * @brief double buffer
 * @version 1.0
 * @date 2023-08-10
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once 

#include <mods/types.h>
#include <kernel/lock.h>
#include <kernel/thread.h>
#include <kernel/userorkernelbuffer.h>
#include <kernel/kbuffer.h>

namespace Kernel
{
    class DoubleBuffer
    {   
    public:
        explicit DoubleBuffer(size_t capacity = 65536);

        /**
         * @return ssize_t 
         */
        [[nodiscard]] ssize_t write(const UserOrKernelBuffer&, size_t);

        /**
         * @param data 
         * @param size 
         * @return ssize_t 
         */
        [[nodiscard]] ssize_t write(const u8* data, size_t size)
        {
            return write(UserOrKernelBuffer::for_kernel_buffer(const_cast<u8>*(data)));
        }

        /**
         * @return ssize_t 
         */
        [[nodiscard]] ssize_t read(UserOrKernelBuffer&, size_t);

        /**
         * @param data 
         * @param size 
         * @return ssize_t 
         */
        [[nodiscard]] ssize_t read(u8* data, size_t size)
        {
            auto buffer = UserOrKernelBuffer::for_kernel_buffer(data);

            return read(buffer, size);
        }
        

    private:
        void flip();
        void compute_lockfree_metadata();

        struct InnerBuffer
        {
            u8* data { nullptr };
            size_t size;
        }; 

        InnerBuffer* m_write_buffer { nullptr };
        InnerBuffer* m_read_buffer { nullptr };
        InnerBuffer m_buffer1;
        InnerBuffer m_buffer2;
        
        KBuffer m_storage;

        Function<void()> m_unlock_callback;
        
        size_t m_capacity { 0 };
        size_t m_read_buffer_index { 0 };
        size_t m_space_for_writing { 0 };

        bool m_empty { true };

        mutable Lock m_lock { "DoubleBuffer" };
    }; // class DoubleBuffer
}