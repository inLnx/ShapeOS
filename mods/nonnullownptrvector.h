/**
 * @file nonnullownptrvector.h
 * @author Krisna Pranav
 * @brief NonNull_OwnPtr_Vector
 * @version 6.0
 * @date 2023-08-11
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once 

#include <mods/nonnullownptr.h>
#include <mods/nonnullptrvector.h>

namespace Mods 
{

    /**
     * @tparam T 
     * @tparam inline_capacity 
     */
    template<typename T, int inline_capacity>
    class NonnullOwnPtrVector : public NonnullPtrVector<NonnullOwnPtr<T>, inline_capacity> 
    {
    };

} // namespace Mods

// using the mods.
using Mods::NonnullOwnPtrVector;
