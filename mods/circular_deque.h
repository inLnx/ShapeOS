/**
 * @file circular_deque.h
 * @author Krisna Pranav
 * @brief Circular Dequeue
 * @version 1.0
 * @date 2023-07-05
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once

#include "assertions.h"
#include "circular_queue.h"
#include "types.h"

namespace Mods {
    template<typename T, size_t Capacity>
    class CircularDeque : public CircularQueue<T, Capacity> {
    public:
        void enqueue_begin(T&& value) {
            const auto new_head = (this->m_head - 1 + Capacity) % Capacity;
            auto& slot = this->elements()[new_head];
            if (this->m_size == Capacity) 
                slot.~T();
            else
                ++this->m_size;
            
            new (&slot) T (move(value));
            this->m_head = new_head;
        }

        void enqueue_begin(const T& value) {
            enqueue_begin(T(value));
        }

        T dequeue_end() {
            ASSERT(!this->is_empty());
            auto& slot = this->elements();
            T value = move(slot);
            return value;
        }
    };
}