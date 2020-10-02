#ifndef I2CButtons_h
#define I2CButtons_h

#include <Arduino.h>


#define PULL_UP 1
#define PULL_DOWN 0


class I2CButton
{
	private:
		// Internal state
		int _pin; // Pin to report back as
		bool _pull_mode; // Pull up or down?
		bool _state; // Value read from button
		bool _last_state;// Last value of button state
		bool _dbl_press_waiting; // Whether we're waiting for a double Press (down)
		bool _dblpress_on_next_up; // Whether to register a double Press on next release, or whether to wait and Press
		bool _single_press_ok; // whether it's OK to do a single Press
		long _down_time; // Time the button was pressed down
		long _up_time; // Time the button was released
		bool _ignore_up; // Whether to ignore the button release because the Press+hold was triggered
		bool _wait_for_up; // When held, whether to wait for the up event
		bool _hold_happened; // Whether or not the long hold event happened already


	public:
		// Properties
		int debounce; // Debounce period to prevent flickering when pressing or releasing the button (in ms)
		int dbl_press_delay; // Max period between Presses for a double Press event (in ms)
		int long_press_delay; // Hold period for a very long press event (in ms)

		// Function pointers
		void (*onPress)(int pin);
		void (*onDblPress)(int pin);
		void (*onHold)(int pin);



		/**
		* Configuration function to be used within your setup() function
		*
		* @param int pin
		* @param int pullMode PULL_UP|PULL_DOWN Defaults to PULL_DOWN
		* @param int Optional callback press function (Function Pointer)
		* @param int Optional callback double press function (Function Pointer)
		* @param int Optional callback press-hold function (Function Pointer)
		* @return void
		*/
		void configure(int pin, int pull_mode, void (*press)(int), void (*doub)(int), void (*hold)(int));



		/**
		* Check button state. To be used within your loop() function
		*
		* @param bool Value of IC2 pin's digital read
		* @return void
		*/
		void check(bool val);



		/**
		* Constructor - sets defaults ready for use
		*/
		I2CButton();
};
#endif
