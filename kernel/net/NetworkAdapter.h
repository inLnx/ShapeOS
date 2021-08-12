/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

// includes
#include <base/ByteBuffer.h>
#include <base/Function.h>
#include <base/IntrusiveList.h>
#include <base/MACAddress.h>
#include <baase/Types.h>
#include <base/WeakPtr.h>
#include <base/Weakable.h>
#include <kernel/bus/pci/Definitions.h>
#include <kernel/KBuffer.h>
#include <kernel/net/ARP.h>
#include <kernel/net/EthernetFrameHeader.h>
#include <kernel/net/ICMP.h>
#include <kernel/net/IPv4.h>
#include <kernel/UserOrKernelBuffer.h>

namespace Kernel {

class NetworkAdapter;

using NetworkByteBuffer = AK::Detail::ByteBuffer<1500>;

struct PacketWithTimestamp : public RefCounted<PacketWithTimestamp> {
    PacketWithTimestamp(NonnullOwnPtr<KBuffer> buffer, Time timestamp)
        : buffer(move(buffer))
        , timestamp(timestamp)
    {
    }

    ReadonlyBytes bytes() { return { buffer->data(), buffer->size() }; };

    NonnullOwnPtr<KBuffer> buffer;
    Time timestamp;
    IntrusiveListNode<PacketWithTimestamp, RefPtr<PacketWithTimestamp>> packet_node;
};

class NetworkAdapter : public RefCounted<NetworkAdapter>
    , public Weakable<NetworkAdapter> {
public:
    static constexpr i32 LINKSPEED_INVALID = -1;

    virtual ~NetworkAdapter();

    virtual StringView class_name() const = 0;

    const String& name() const { return m_name; }
    MACAddress mac_address() { return m_mac_address; }
    IPv4Address ipv4_address() const { return m_ipv4_address; }
    IPv4Address ipv4_netmask() const { return m_ipv4_netmask; }
    IPv4Address ipv4_broadcast() const { return IPv4Address { (m_ipv4_address.to_u32() & m_ipv4_netmask.to_u32()) | ~m_ipv4_netmask.to_u32() }; }
    IPv4Address ipv4_gateway() const { return m_ipv4_gateway; }
    virtual bool link_up() { return false; }
    virtual i32 link_speed()
    {

        return LINKSPEED_INVALID;
    }
    virtual bool link_full_duplex() { return false; }

    void set_ipv4_address(const IPv4Address&);
    void set_ipv4_netmask(const IPv4Address&);
    void set_ipv4_gateway(const IPv4Address&);

    void send(const MACAddress&, const ARPPacket&);
    void fill_in_ipv4_header(PacketWithTimestamp&, IPv4Address const&, MACAddress const&, IPv4Address const&, IPv4Protocol, size_t, u8);

    size_t dequeue_packet(u8* buffer, size_t buffer_size, Time& packet_timestamp);

    bool has_queued_packets() const { return !m_packet_queue.is_empty(); }

    u32 mtu() const { return m_mtu; }
    void set_mtu(u32 mtu) { m_mtu = mtu; }

    u32 packets_in() const { return m_packets_in; }
    u32 bytes_in() const { return m_bytes_in; }
    u32 packets_out() const { return m_packets_out; }
    u32 bytes_out() const { return m_bytes_out; }

    RefPtr<PacketWithTimestamp> acquire_packet_buffer(size_t);
    void release_packet_buffer(PacketWithTimestamp&);

    constexpr size_t layer3_payload_offset() const { return sizeof(EthernetFrameHeader); }
    constexpr size_t ipv4_payload_offset() const { return layer3_payload_offset() + sizeof(IPv4Packet); }

    Function<void()> on_receive;

    void send_packet(ReadonlyBytes);

protected:
    NetworkAdapter();
    void set_interface_name(const PCI::Address&);
    void set_mac_address(const MACAddress& mac_address) { m_mac_address = mac_address; }
    void did_receive(ReadonlyBytes);
    virtual void send_raw(ReadonlyBytes) = 0;

    void set_loopback_name();

private:
    MACAddress m_mac_address;
    IPv4Address m_ipv4_address;
    IPv4Address m_ipv4_netmask;
    IPv4Address m_ipv4_gateway;

    static constexpr size_t max_packet_buffers = 1024;

    using PacketList = IntrusiveList<PacketWithTimestamp, RefPtr<PacketWithTimestamp>, &PacketWithTimestamp::packet_node>;

    PacketList m_packet_queue;
    size_t m_packet_queue_size { 0 };
    PacketList m_unused_packets;
    String m_name;
    u32 m_packets_in { 0 };
    u32 m_bytes_in { 0 };
    u32 m_packets_out { 0 };
    u32 m_bytes_out { 0 };
    u32 m_mtu { 1500 };
};

}
