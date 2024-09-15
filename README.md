A driver for Assetto Corsa for use in co-simulation, using the Shared Memory feature for reading real-time telemetry (at the full simulation frequency, 333Hz) and VJoy to control the vehicle.

# Reading state:
The UDP telemetry feature sends a reduced set of data at low frequency (~20Hz), so it is not used here.
Instead, the most information can be read via the Shared Memory feature (Reference: https://www.assettocorsa.net/forum/index.php?threads/shared-memory-reference-25-05-2017.3352/).
Three structs:
SPageFilePhysics - variable data related to the physics simulation, e.g. vehicle heading, velocity, rpm, angular velocity, etc... Updated at a fixed frequency of 333Hz, independent of the graphical settings. 
SPageFileGraphics - variable data related to the session, e.g. laptimes, race position, simulation status (e.g. paused), etc... Seems to be updated at twice the graphical frame-rate.
SPageFileStatic - constant data that doesn't change during the session, e.g. track name, max RPM, tyre radius... I would expect this is not updated at all during a session.

# Controlling the vehicle:
We control the vehicle using VJoy. Delay was tested thanks to the fact that the current inputs can be read from SPageFilePhysics, and the control takes effect the next physical tick (i.e. there's no delay).

Gear shifts are handled by simulating sequential paddles, because the H-Shifter feature only works on certain vehicles and requires the usage of the clutch pedal even if "Automatic Clutch" is enabled.

As for the steering, the range [-1,1] is used as input. The actual steering wheel angle also depends on the "degrees" setting (which you find in the game control settings) and the vehicle steering lock. Whenever "degrees" is less than the steering lock of the vehicle, it is automatically remapped to the full steering lock of the vehicle. Instead, when it is larger, the input is saturated to the steering lock. For this reason, i suggest to set "degrees" to the minimum possible (i.e. 40), so that the full range [0,1] is mapped to the actual vehicle steering range.
