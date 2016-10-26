//------------------------------------------------------------------------------
//             __             __   ___  __
//     | |\ | /  ` |    |  | |  \ |__  /__`
//     | | \| \__, |___ \__/ |__/ |___ .__/
//
//------------------------------------------------------------------------------

#include "sam.h"
#include "pwm.h"
#include "timer.h"
#include "adc.h"
#include "timer.h"

//-----------------------------------------------------------------------------
//      __   ___  ___         ___  __
//     |  \ |__  |__  | |\ | |__  /__`
//     |__/ |___ |    | | \| |___ .__/
//
//-----------------------------------------------------------------------------

// Value, passed to led_set, can be in the range 0 to 255
#define LED_VAL_MIN       (0x00)
#define LED_VAL_MAX       (0xff)
// When the value rolls over, the LED will be red
#define LED_VAL_RED       (LED_VAL_MIN)
// One third of the way through its cycle, the LED will be green
#define LED_VAL_GREEN     (LED_VAL_MAX / 3)
// Two thirds of the way through its cycle, the LED will be blue
#define LED_VAL_BLUE      ((LED_VAL_MAX * 2) / 3)

//-----------------------------------------------------------------------------
//     ___      __   ___  __   ___  ___  __
//      |  \ / |__) |__  |  \ |__  |__  /__`
//      |   |  |    |___ |__/ |___ |    .__/
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                __          __        ___  __
//     \  /  /\  |__) |  /\  |__) |    |__  /__`
//      \/  /~~\ |  \ | /~~\ |__) |___ |___ .__/
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      __   __   __  ___  __  ___      __   ___  __
//     |__) |__) /  \  |  /  \  |  \ / |__) |__  /__`
//     |    |  \ \__/  |  \__/  |   |  |    |___ .__/
//
//-----------------------------------------------------------------------------
static void led_set(uint8_t value);

//-----------------------------------------------------------------------------
//      __        __          __
//     |__) |  | |__) |    | /  `
//     |    \__/ |__) |___ | \__,
//
//-----------------------------------------------------------------------------

//=============================================================================
int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
    pwm_init();
	adc_init();
	timer_init();

	// Maintain a counter of how far through the 256 step cycle we are.
    uint8_t led_value = LED_VAL_MIN;

    /* Replace with your application code */
    while (1) 
    {
        // If the full delay has elapsed, rotate the LEDs
        if (timer_get() >= adc_pot_get() / 64)
        {
            timer_set(0);
            
			led_value++;
			if (led_value > LED_VAL_MAX)
			{
				led_value = LED_VAL_MIN;
			}

			led_set(led_value);
        }
    }
}

//-----------------------------------------------------------------------------
//      __   __              ___  ___
//     |__) |__) | \  /  /\   |  |__
//     |    |  \ |  \/  /~~\  |  |___
//
//-----------------------------------------------------------------------------


//==============================================================================
// Set how far through the red -> green -> blue cycle the LED is.
// LED_VAL_[COLOR] constants in led.h can set individual colors, and consistent
// incrementing of value cycles smoothly through the spectrum.
//==============================================================================
static void led_set(uint8_t value)
{
  // NOTE: LED_VAL_RED = 0, LED_VAL_GREEN = 1/3 LED_VAL_MAX, and LED_VAL_BLUE
  //  = 2/3 LED_VAL_MAX. Consequently, those three values divide the domain of
  // this function into three (approximately, because ints) equal parts.

  // 0 <= value <= LED_VAL_GREEN is the first third of the cycle. Red is
  // ramping down and green is ramping up.
  if (value <= LED_VAL_GREEN)
  {
    // Red is ramping down
    pwm_r0_set(LED_VAL_GREEN - value);
    // Green is ramping up
    pwm_g0_set(value);
    // Blue is off
    pwm_b0_set(0);
  }

  // LED_VAL_GREEN < value <= LED_VAL_BLUE is the second third of the cycle.
  // Green is ramping down and blue is ramping up.
  else if (value <= LED_VAL_BLUE)
  {
    // Red is off
    pwm_r0_set(0);
    // Green is ramping down
    pwm_g0_set(LED_VAL_BLUE - value);
    // Blue is ramping up
    pwm_b0_set(value - LED_VAL_GREEN);
  }

  // LED_VAL_BLUE < value <= LED_VAL_MAX is the final third of the cycle.
  // Blue is ramping down and red is ramping up.
  else
  {
    // Red is ramping up
    pwm_r0_set(value - LED_VAL_BLUE);
    // Green is off
    pwm_g0_set(0);
    // Blue is ramping down
    pwm_b0_set(LED_VAL_MAX - value);
  }
}
//-----------------------------------------------------------------------------
//        __   __   __
//     | /__` |__) /__`
//     | .__/ |  \ .__/
//
//-----------------------------------------------------------------------------
