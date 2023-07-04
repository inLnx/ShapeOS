/**
 * @file base64.h
 * @author Krisna Pranav
 * @brief Base64
 * @version 1.0
 * @date 2023-07-04
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once

#include "string.h"
#include "string_view.h"
#include "byte_buffer.h"
#include "span.h"

namespace Mods {
    /**
     * @return size_t 
     */
    size_t calcualte_base64_decoded_length(const StringView&);

    /**
     * @return size_t 
     */
    size_t calculate_base64_encoded_length(ReadonlyBytes);

    /**
     * @return String 
     */
    String encode_base64(ReadonlyBytes);
};

using Mods::encode_base64;