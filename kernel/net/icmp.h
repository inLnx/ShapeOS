/**
 * @file icmp.h
 * @author Krisna Pranav aka (krishpranav)
 * @brief icmp
 * @version 1.0
 * @date 2023-08-15
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once

#include <mods/mac_address.h>
#include <kernel/net/ipv4.h>

struct ICMPType 
{
    enum 
    {
        EchoReploy = 0,
        EchoRequest = 8,
    };
}; // struct 

class ICMPHeader
{
public:
    ICMPHeader() {}
    ~ICMPHeader() {}

    /**
     * @return u8 
     */
    u8 type() const
    {
        return m_type;
    }

    /**
     * @param b 
     */
    void set_type(u8 b)
    {
        m_type = b;
    }

    /**
     * @return u8 
     */
    u8 code() const
    {
        return m_code;
    }

    /**
     * @param b 
     */
    void set_code(u8 b)
    {
        m_code = b;
    }

    /**
     * @return u16 
     */
    u16 checksum() const
    {
        return m_checksum;
    }
    
    /**
     * @param w 
     */
    void set_checksum(u16 w)
    {
        m_checksum = w;
    }

    /**
     * @return const void* 
     */
    const void* payload() const
    {
        return this + 1;
    }
    
    void payload()
    {
        return this + 1;
    }

private:
    u8 m_type { 0 };
    u8 m_code { 0 };
    NetworkOrdered<u16> m_checksum { 0 };
}; // class

static_assert(sizeof(ICMPHeader) == 1);

struct ICMPEchoPacket
{
    void payload()
    {
        return this + 1;
    }

    const void* payload() const
    {
        return this + 1;
    }
};