#include <iostream>
#include <string>
#include <chrono>
#include <unordered_map>
#include <thread>

#include "shared_file_out.h"
#include "shm_struct_wrapper.hpp"
#include "ac_controls.hpp"

using namespace std::chrono_literals;

int main()
{
    try {
        SHMStructWrapper<ac_shm::SPageFilePhysics> physics(TEXT("Local\\acpmf_physics"));

        ACControls ctrl(1);

        for (int i = 0; i <= 20; ++i) {
            float u = -1.0 + i * (2.0 / 20.0);
            ctrl.set_steer(u);
            ctrl.update();
            std::this_thread::sleep_for(100ms);

            float x = physics->steerAngle;

            std::cout << "u: " << u << ", x: " << x << std::endl;
        }

        
        

        /*

        bool is_shifting_up;
        ctrl.set_steer(0);
        ctrl.set_throttle(1);
        ctrl.update();

        int gear = 0;
        while (gear < 5) {
            std::cout << "Gear: " << (physics->gear - 1) << " @ " << physics->rpms << "RPM" << std::endl;

            if (physics->rpms > 5000) {
                ctrl.shift_up(true);
                ctrl.update();
                ctrl.shift_up(false);
                ctrl.update();
            }
        }
        */
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        throw;
    }

    // SHMStructWrapper<SPageFileStatic> statics(TEXT("Local\\acpmf_static"));
    // SHMStructWrapper<SPageFileGraphic> graphics(TEXT("Local\\acpmf_graphics"));

    return 0;
}