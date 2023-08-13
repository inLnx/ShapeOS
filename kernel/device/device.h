/**
 * @file device.h
 * @author Krisna Pranav
 * @brief device
 * @version 1.0
 * @date 2023-08-13
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once 

#include <mods/function.h>
#include <mods/doublelinkedlist.h>
#include <mods/hashmap.h>
#include <kernel/lock.h>
#include <kernel/unixtypes.h>
#include <kernel/device/asyncdevicerequest.h>
#include <kernel/filesystem/file.h>

namespace Kernel
{
    class Device : public File 
    {

    public:
        virtual ~Device() override;

        /**
         * @return unsigned 
         */
        unsigned major() const
        {
            return m_major;
        }

        /**
         * @return unsigned 
         */
        unsigned minor() const
        {
            return m_minor;
        }

        /**
         * @return String 
         */
        virtual String absolute_path(const FileDescription&) const override;

        /**
         * @return String 
         */
        virtual String absolute_path() const;

        /**
         * @return uid_t 
         */
        uid_t uid() const
        {
            return m_uid;
        }

        /**
         * @return uid_t 
         */
        uid_t gid() const
        {
            return m_gid;
        }

        /**
         * @return true 
         * @return false 
         */
        virtual bool is_device() const override
        {
            return true;
        }

        /**
         * @return true 
         * @return false 
         */
        virtual bool is_disk_Device() const
        {
            return false;
        }

        /// @brief: for_each
        static void for_each(Function<void(Device&)>);

        /**
         * @param major 
         * @param minor 
         * @return Device* 
         */
        static Device* get_device(unsigned major, unsigned minor);

        void process_next_queued_request(Badge<AsyncDeviceRequest>, const AsyncDeviceRequest&);

    protected:
        /**
         * @param major 
         * @param minor 
         */
        Device(unsigned major, unsigned minor);

        /**
         * @param uid 
         */
        void set_uid(uid_t uid)
        {
            m_uid = uid;
        }

        /**
         * @param gid 
         */
        void set_gid(gid_t gid)
        {
            m_gid = gid;
        }

        static HashMap<u32, Device*>& all_devices();
    
    private:
        unsigned m_major { 0 };
        unsigned m_minor { 0 };

        uid_t m_uid { 0 };
        gid_t m_gid { 0 };

        SpinLock<u8> m_requests_lock;

        DoubleLinkedList<RefPtr<AsyncDeviceRequest>> m_request;

    }; // class Device

} // namespace Kernel