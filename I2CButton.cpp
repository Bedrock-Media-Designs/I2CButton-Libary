#include "I2CButton.h"
#include "Arduino.h"



/**
* Constructor function - sets defaults ready for use
*/
I2CButton::I2CButton()
{
	// Defaults
	this->debounce = 20;
	this->dbl_press_delay = 250;
	this->long_press_delay = 750;

	// Internal states
	this->_state = true;
	this->_last_state = true;
	this->_dbl_press_waiting = false;
	this->_dblpress_on_next_up = false;
	this->_single_press_ok = false;
	this->_down_time = -1;
	this->_up_time = -1;
	this->_ignore_up = false;
	this->_wait_for_up = false;
	this->_hold_happened = false;
}



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
void I2CButton::configure(int pin, int pull_mode = PULL_DOWN, void (*press)(int) = NULL, void (*doub)(int) = NULL, void (*hold)(int) = NULL)
{
	this->_pin = pin;
	this->_pull_mode = pull_mode;

	onPress = press;
	onDblPress = doub;
	onHold = hold;
}



/**
* Check button state. To be used within your loop() function
*
* @return void
*/
void I2CButton::check(bool state)
{
	int result_event = 0;
	long mill = millis();

	this->_state = state == HIGH;

	// Button down
	if (this->_state != this->_pull_mode
		&& this->_last_state == this->_pull_mode
		&& (mill - this->_up_time) > this->debounce)
	{
		this->_down_time = mill;
		this->_ignore_up = false;
		this->_wait_for_up = false;
		this->_single_press_ok = true;
		this->_hold_happened = false;

		if ((mill - this->_up_time) < this->dbl_press_delay
			&& this->_dblpress_on_next_up == false
			&& this->_dbl_press_waiting == true)
		{
			this->_dblpress_on_next_up = true;
		}
		else
		{
			this->_dblpress_on_next_up = false;
			this->_dbl_press_waiting = false;
		}
	}

	// Button released
	else if (this->_state  == this->_pull_mode
		&& this->_last_state != this->_pull_mode
		&& (mill - this->_down_time) > this->debounce)
	{
		if (this->_ignore_up == false)
		{
			this->_up_time = mill;

			if (this->_dblpress_on_next_up == false)
			{
				this->_dbl_press_waiting = true;
			}
			else
			{
				result_event = 2;
				this->_dblpress_on_next_up = false;
				this->_dbl_press_waiting = false;
				this->_single_press_ok = false;
			}

		}
	}

	// Test for normal Press event; dbl_press_delay expired
	if (this->_state == this->_pull_mode
		&& (mill - this->_up_time) >= this->dbl_press_delay
		&& this->_dbl_press_waiting == true
		&& this->_dblpress_on_next_up == false
		&& this->_single_press_ok == true
		&& result_event != 2)
	{
		result_event = 1;
		this->_dbl_press_waiting = false;
	}

	// Test for hold
	if (this->_state != this->_pull_mode
		&& (mill - this->_down_time) >= this->long_press_delay)
	{
		// Trigger hold
		if (this->_hold_happened == false)
		{
			this->_wait_for_up = true;
			this->_ignore_up = true;
			this->_dblpress_on_next_up = false;
			this->_dbl_press_waiting = false;
			this->_hold_happened = true;
		}

		this->_down_time = mill;
		result_event = 3;
	}

	this->_last_state = this->_state;

	if (result_event == 1 && onPress) onPress(this->_pin);
	if (result_event == 2 && onDblPress) onDblPress(this->_pin);
	if (result_event == 3 && onHold) onHold(this->_pin);
}
