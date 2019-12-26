# JoyCon Bridge

**Easy, simple, object-oriented access to JoyCons.**

JoyCon Bridge allows you to access the state of any connected JoyCon, including buttons, analog sticks, accelerometer and gyroscope data - all in a simple and straightforward manner.


## C++ Example
```cpp
#include "JoyCon.h"
#include "connect.h"
#include "exceptions.h"

using namespace joy_con_bridge;

int main()
{
	try {
		JoyCon left = connect::getLeftJoyCon();
		while (true) {
			left.poll();

			ButtonsState buttons = left.getButtonsState();
			AnalogStick stick = left.getLeftStick();
			ThreeAxesSensor acc = left.getAccelerometer();
			ThreeAxesSensor gyro = left.getGyroscope();

			// ...
		}
	} catch (const JoyConError& e) {
		std::cout << "JoyCon error! " << e.what() << std::endl;
	} catch (const HidError& e) {
		std::cout << "Device error! " << e.what() << std::endl;
	}

	return 0;
}
```


## Python interface

The sub-project `pyjoyconbridge` provides the JoyConBridge library as a Python module (`pyd`).

### Example

```py
import pyjoyconbridge


def main():
    try:
        left = pyjoyconbridge.get_left_joy_con()
        while True:
            left.poll()

            buttons = left.buttons_state
            stick = left.left_stick
            acc = left.accelerometer
            gyro = left.gyroscope

            # ...
    except RuntimeError as e:
        print("Error! " + str(e))
```


## Build dependencies

* Boost.Python
	* Note: This is required only if you intend to build `pyjoyconbridge`.


## Limitations

* Only Bluetooth communication is supported.
* No support for sending rumble.
* Windows only (can be resolved by replacing the `hidapi` version used).


## Special thanks

* [dekuNukem/Nintendo_Switch_Reverse_Engineering](https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering):  Detailed information about the JoyCon's protocol

* [CTCaer/jc_toolkit](https://github.com/CTCaer/jc_toolkit): Sensor calibration implementation
