#include <iostream>
#include <string>
#include <chrono>

#include "shared_file_out.h"
#include "shm_struct_wrapper.hpp"

#include "vjoy/public.h"
#include "vjoy/vjoyinterface.h"


using namespace std::chrono_literals;

void check_vjoy_lib() {
    if (!vJoyEnabled())
        throw std::runtime_error("Is VJoy installed?");

    WORD VerDll, VerDrv;
    if (!DriverMatch(&VerDll, &VerDrv))
        throw std::runtime_error("VJoy version mismatch (dll: " + std::to_string(VerDll) + ", driver: " + std::to_string(VerDrv) + ")");
}

class VJoyDeviceWrapper {
    BYTE m_idx;

    JOYSTICK_POSITION_V2 m_data;
public:
    VJoyDeviceWrapper(BYTE idx) : m_idx(idx) {
        auto state = GetVJDStatus(idx);
        if (state != VJD_STAT_FREE)
            throw std::runtime_error("VJoy device " + std::to_string(idx) + " is not free (is " + std::to_string(state) + ")");

        if (!AcquireVJD(idx))
            throw std::runtime_error("Failed to acquire VJoy device " + std::to_string(idx));

        memset(&this->data(), 0, sizeof(JOYSTICK_POSITION_V2));
    }
    ~VJoyDeviceWrapper() { RelinquishVJD(m_idx); }

    bool update() {
        m_data.bDevice = m_idx;
        return UpdateVJD(m_idx, static_cast<void*>(&m_data));
    }
    JOYSTICK_POSITION_V2& data() { return m_data; }
};



int main()
{
    try {
        check_vjoy_lib();
        VJoyDeviceWrapper wrapper(1);

        SHMStructWrapper<SPageFilePhysics> physics(TEXT("Local\\acpmf_physics"));

        auto start = std::chrono::steady_clock::now();

        std::cout << "Zeroing gas" << std::endl;
        while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start) < 1000ms) {
            wrapper.data().wAxisY = 0x00;
            wrapper.update();
        }
        std::cout << "Gas zeroed" << std::endl;

        
        wrapper.data().wAxisY = 0x7FFF;
        wrapper.update();

        start = std::chrono::steady_clock::now();
        std::chrono::microseconds dt;
        
        do {
            dt = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start);
        } while (physics->gas == 0 && dt < 1000ms);

        std::cout << "Gas feedback after " << dt.count() << "us" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        throw;
    }

    // SHMStructWrapper<SPageFileStatic> statics(TEXT("Local\\acpmf_static"));
    // SHMStructWrapper<SPageFileGraphic> graphics(TEXT("Local\\acpmf_graphics"));

    return 0;
}