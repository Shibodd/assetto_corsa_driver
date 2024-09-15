#pragma once

#include <stdexcept>
#include <string>

#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

#include "vjoy/public.h"
#include "vjoy/vjoyinterface.h"

class VJoyDeviceWrapper {
    BYTE m_idx;
    JOYSTICK_POSITION_V2 m_data;

public:
    VJoyDeviceWrapper(BYTE idx) : m_idx(idx) {
        if (!vJoyEnabled())
            throw std::runtime_error("Is VJoy installed?");

        WORD VerDll, VerDrv;
        if (!DriverMatch(&VerDll, &VerDrv))
            throw std::runtime_error("VJoy version mismatch (dll: " + std::to_string(VerDll) + ", driver: " + std::to_string(VerDrv) + ")");

        auto state = GetVJDStatus(idx);
        if (state != VJD_STAT_FREE) {
            std::string state_str;
            switch (state) {
            case VJD_STAT_FREE: state_str = "device is free (FREE)"; break;
            case VJD_STAT_BUSY: state_str = "device is owned by another application (BUSY)"; break;
            case VJD_STAT_MISS: state_str = "device is missing (MISS)"; break;
            case VJD_STAT_OWN: state_str = "device is already owned by this application (OWN)"; break;
            case VJD_STAT_UNKN: state_str = "device state is unknown (UNKN)"; break;
            default: state_str = std::to_string(state); break;
            }
            throw std::runtime_error("VJoy device " + std::to_string(idx) + " is not free (" + state_str + ")");
        }

        if (!AcquireVJD(idx))
            throw std::runtime_error("Failed to acquire VJoy device " + std::to_string(idx));

        memset(&m_data, 0, sizeof(decltype(m_data)));
    }
    ~VJoyDeviceWrapper() { RelinquishVJD(m_idx); }

    bool update() {
        m_data.bDevice = m_idx;
        return UpdateVJD(m_idx, static_cast<void*>(&m_data));
    }
    JOYSTICK_POSITION_V2& data() { return m_data; }
};