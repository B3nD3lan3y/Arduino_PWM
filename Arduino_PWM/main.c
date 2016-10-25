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

//-----------------------------------------------------------------------------
//      __   ___  ___         ___  __
//     |  \ |__  |__  | |\ | |__  /__`
//     |__/ |___ |    | | \| |___ .__/
//
//-----------------------------------------------------------------------------

/*
infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0484c/CHDCICDF.html
//void DelayCycles(volatile U32 n){while(n--);}
00000320 <DelayCycles>: //uses 8 cycles on each loop iteration (on ARM Cortex M0+)
320: b082 sub sp, #8
322: 9001 str r0, [sp, #4]
324: 9b01 ldr r3, [sp, #4] 2 cycles (loop start)
326: 1e5a subs r2, r3, #1 1 cycle
328: 9201 str r2, [sp, #4] 2 cycles
32a: 2b00 cmp r3, #0 1 cycles
32c: d1fa bne.n 324 <DelayCycles+0x4> 2 cycles (branch to start)
32e: b002 add sp, #8
330: 4770 bx lr
*/
#define CYCLES_IN_DLYTICKS_FUNC (8)
#define F_CPU (48000000UL)
#define MS_TO_DLYTICKS(ms) (uint32_t)(F_CPU / 1000 * ms / CYCLES_IN_DLYTICKS_FUNC)
// ((float)(F_CPU)) / 1000.0
#define DelayTicks(ticks) {volatile uint32_t n=ticks; while(n--);}//takes 8 cycles
#define DelayMs(ms) DelayTicks(MS_TO_DLYTICKS(ms))//uses 20bytes


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

    uint8_t red = 0x00;
    uint8_t green = (0xff / 3);
    uint8_t blue = (0xff * 2 / 3);

    // The speed at which we rotate
    uint16_t delay = 50;
    uint16_t elapsed = 0;
    

    /* Replace with your application code */
    while (1) 
    {
        // Delay before going again.
        DelayMs(1);
        elapsed++;

        // If the full delay has elapsed, rotate the LEDs
        if (elapsed >= adc_pot_get() / 8)
        {
            elapsed = 0;
            // Rotate the leds
            pwm_r0_set(red);
            pwm_g0_set(green);
            pwm_b0_set(blue);

            red = (red + 1) % 0x80;
            green = (green + 1) % 0x80;
            blue = (blue + 1) % 0x80;
        }
    }
}

//-----------------------------------------------------------------------------
//      __   __              ___  ___
//     |__) |__) | \  /  /\   |  |__
//     |    |  \ |  \/  /~~\  |  |___
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//        __   __   __
//     | /__` |__) /__`
//     | .__/ |  \ .__/
//
//-----------------------------------------------------------------------------
