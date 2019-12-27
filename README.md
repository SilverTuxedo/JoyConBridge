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


### Building `pyjoyconbridge`

This project uses Boost.Python.

* Install [Python](https://www.python.org/downloads/).
* Download and configure [Boost](https://www.boost.org/).
* Set the following environment variables:
	* `BOOST_ROOT`: The directory where Boost is located.
	* `PYTHON_ROOT`: The directory where Python is installed.
* Build using Visual Studio.

If you don't want to change your environment variables, you can manually replace `$(...)` occurences in `Boost.Python.props`.


## Limitations

* Only Bluetooth communication is supported.
* No support for sending rumble.
* Windows only (can be resolved by replacing the `hidapi` version used).


## Special thanks

* [dekuNukem/Nintendo_Switch_Reverse_Engineering](https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering):  Detailed information about the JoyCon's protocol

* [CTCaer/jc_toolkit](https://github.com/CTCaer/jc_toolkit): Sensor calibration implementation
