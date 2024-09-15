#pragma once

#include <algorithm>

#include "vjoy_device_wrapper.hpp"

class ACControls {
	VJoyDeviceWrapper m_device;

	inline static LONG float_to_axis(float x) {
		constexpr static LONG AXIS_MAX = 0x8000;

		// Remap x from [0,1] to [0, AXIS_MAX]
		x = x * (float)AXIS_MAX;

		// Ensure the cast is safe
		x = std::clamp<float>(
			x,
			static_cast<float>(std::numeric_limits<LONG>::min()),
			static_cast<float>(std::numeric_limits<LONG>::max())
		);

		// Cast, then saturate to range
		return std::clamp<LONG>(static_cast<LONG>(x), 0, AXIS_MAX);
	}

	inline void set_button(int idx, bool value) {
		LONG mask = 1 << idx;
		if (value)
			m_device.data().lButtons |= mask;
		else
			m_device.data().lButtons &= ~mask;
	}

public:
	ACControls(int device_idx) : m_device(device_idx) {
		set_steer(0);
	}

	void set_steer(float degrees) { m_device.data().wAxisX = float_to_axis(0.5f + (0.5f * degrees)); }
	void set_throttle(float throttle) { m_device.data().wAxisY = float_to_axis(throttle); }
	void set_brake(float brake) { m_device.data().wAxisZ = float_to_axis(brake); }
	void shift_up(bool state) { set_button(0, state); }
	void shift_down(bool state) { set_button(1, state); }

	bool update() {
		bool ans = m_device.update();
		return ans;
	}
};

