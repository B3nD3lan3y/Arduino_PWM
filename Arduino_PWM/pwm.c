//------------------------------------------------------------------------------
//             __             __   ___  __
//     | |\ | /  ` |    |  | |  \ |__  /__`
//     | | \| \__, |___ \__/ |__/ |___ .__/
//
//------------------------------------------------------------------------------

#include "pwm.h"
#include <sam.h>
#include <component/pm.h>

//------------------------------------------------------------------------------
//      __   ___  ___         ___  __
//     |  \ |__  |__  | |\ | |__  /__`
//     |__/ |___ |    | | \| |___ .__/
//
//------------------------------------------------------------------------------

#define LED_R0 (PORT_PA15)
#define LED_R0_GROUP (0)
#define LED_R0_PIN (PIN_PA15%32)

#define LED_G0 (PORT_PA20)
#define LED_G0_GROUP (0)
#define LED_G0_PIN (PIN_PA20%32)

#define LED_B0 (PORT_PA07)
#define LED_B0_GROUP (0)
#define LED_B0_PIN (PIN_PA07%32)


// Constants for Clock multiplexers
#define GENERIC_CLOCK_MULTIPLEXER_TCC0_TCC1 (0x1A)

//------------------------------------------------------------------------------
//     ___      __   ___  __   ___  ___  __
//      |  \ / |__) |__  |  \ |__  |__  /__`
//      |   |  |    |___ |__/ |___ |    .__/
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                __          __        ___  __
//     \  /  /\  |__) |  /\  |__) |    |__  /__`
//      \/  /~~\ |  \ | /~~\ |__) |___ |___ .__/
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      __   __   __  ___  __  ___      __   ___  __
//     |__) |__) /  \  |  /  \  |  \ / |__) |__  /__`
//     |    |  \ \__/  |  \__/  |   |  |    |___ .__/
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      __        __          __
//     |__) |  | |__) |    | /  `
//     |    \__/ |__) |___ | \__,
//
//------------------------------------------------------------------------------

//==============================================================================
void pwm_init()
{
  // Enable Multiplexing
  PORT->Group[LED_R0_GROUP].PINCFG[LED_R0_PIN].bit.PMUXEN = 1;
  PORT->Group[LED_G0_GROUP].PINCFG[LED_G0_PIN].bit.PMUXEN = 1;
  PORT->Group[LED_B0_GROUP].PINCFG[LED_B0_PIN].bit.PMUXEN = 1;
  
  // Configure Multiplexing
  // R0 and G0 use Peripherial Function F (TCC0)
  // B0 uses Peripherial Function E 
  PORT->Group[LED_R0_GROUP].PMUX[LED_R0_PIN / 2].bit.PMUXO = 0x5;
  PORT->Group[LED_G0_GROUP].PMUX[LED_G0_PIN / 2].bit.PMUXE = 0x5;
  PORT->Group[LED_B0_GROUP].PMUX[LED_B0_PIN / 2].bit.PMUXO = 0x4;

  // Page 656:
  // Before the TCC is enabled, it must be configured as outlined by the following steps:
  //   Enable the TCC bus clock (CLK_TCCx_APB) first
  PM->APBCMASK.bit.TCC0_ = 1;
  PM->APBCMASK.bit.TCC1_ = 1;
  
  /* ----------------------------------------------------------------------------------------------
   * Put Generic Clock Generator 1 as source for Generic Clock Multiplexer 0x1A (TCC0 and TCC1)
   */
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID( GENERIC_CLOCK_MULTIPLEXER_TCC0_TCC1 ) | // Generic Clock Multiplexer 0
                      GCLK_CLKCTRL_GEN_GCLK1 | // Generic Clock Generator 1 is source
                      GCLK_CLKCTRL_CLKEN ;
  
  // Enable TCC0 for R0 and G0
  // Select PRESCALER setting in the Control A register (CTRLA.PRESCALER)
  TCC0->CTRLA.bit.PRESCALER   = 0;        
  
  //  Select Prescaler Synchronization setting in Control A register (CTRLA.PRESCSYNC)
  TCC0->CTRLA.bit.PRESCSYNC   = 0;
  
  TCC0->CTRLA.bit.RESOLUTION  = 0;        // No dithering, please!
  
  //  If down-counting operation must be enabled, write a one to the Counter Direction bit in the Control B Set register (CTRLBSET.DIR)
  TCC0->CTRLBSET.bit.DIR      = 0;
  while ( TCC0->SYNCBUSY.bit.CTRLB )
  {
    ;
  }
  
  //   Select the Waveform Generation operation in WAVE register (WAVE.WAVEGEN)
  TCC0->WAVE.bit.WAVEGEN      = 0x2;      // Normal PWM mode
  while ( TCC0->SYNCBUSY.bit.WAVE )
  {
    ;
  }
  
  //   Select the Waveform Output Polarity in the WAVE register (WAVE.POL)
  TCC0->WAVE.vec.POL          = 0;
  while ( TCC0->SYNCBUSY.bit.WAVE )
  {
    ;
  }
  
  //   The waveform output can be inverted for the individual channels using the Waveform Output Invert Enable bit group in the Driver register (DRVCTRL.INVEN)
  TCC0->DRVCTRL.vec.INVEN     = 0;        // No inversion
  TCC0->PER.reg               = LED_FULL_BRIGHTNESS; // TOP is 255
  while ( TCC0->SYNCBUSY.bit.PER )
  {
    ;
  }
  
  TCC0->WEXCTRL.bit.OTMX      = 0x1;      // Alternate CC0 and CC1 for all 8 outputs
  
  // G0 CCB
  TCC0->CCB[0].bit.CCB          = 0;
  while ( TCC0->SYNCBUSY.bit.CCB0 )
  {
    ;
  }
  // R0 CCB
  TCC0->CCB[1].bit.CCB          = 0;
  while ( TCC0->SYNCBUSY.bit.CCB1 )
  {
    ;
  }
  
  // The TCC is enabled by writing a one to the Enable bit in the Control A register (CTRLA.ENABLE).
  TCC0->CTRLA.bit.ENABLE      = 1;
  
  
   // Enable TCC1 for B0
  // Select PRESCALER setting in the Control A register (CTRLA.PRESCALER)
  TCC1->CTRLA.bit.PRESCALER   = 0;        
  
  //  Select Prescaler Synchronization setting in Control A register (CTRLA.PRESCSYNC)
  TCC1->CTRLA.bit.PRESCSYNC   = 0;
  
  TCC1->CTRLA.bit.RESOLUTION  = 0;        // No dithering, please!
  
  //  If down-counting operation must be enabled, write a one to the Counter Direction bit in the Control B Set register (CTRLBSET.DIR)
  TCC1->CTRLBSET.bit.DIR      = 0;
  while ( TCC1->SYNCBUSY.bit.CTRLB )
  {
    ;
  }
  
  //   Select the Waveform Generation operation in WAVE register (WAVE.WAVEGEN)
  TCC1->WAVE.bit.WAVEGEN      = 0x2;      // Normal PWM mode
  while ( TCC1->SYNCBUSY.bit.WAVE )
  {
    ;
  }
  
  //   Select the Waveform Output Polarity in the WAVE register (WAVE.POL)
  TCC1->WAVE.vec.POL          = 0;
  while ( TCC1->SYNCBUSY.bit.WAVE )
  {
    ;
  }
  
  //   The waveform output can be inverted for the individual channels using the Waveform Output Invert Enable bit group in the Driver register (DRVCTRL.INVEN)
  TCC1->DRVCTRL.vec.INVEN     = 0;        // No inversion
  TCC1->PER.reg               = LED_FULL_BRIGHTNESS; // TOP is 255
  while ( TCC1->SYNCBUSY.bit.PER )
  {
    ;
  }
  
  TCC1->WEXCTRL.bit.OTMX      = 0x1;      // Alternate CC0 and CC1
  // B0 CCB
  TCC1->CCB[1].bit.CCB          = 0;
  while ( TCC1->SYNCBUSY.bit.CCB1 )
  {
    ;
  }
  
  TCC1->CTRLA.bit.ENABLE      = 1;

  // Enable double buffering
  TCC0->CTRLBCLR.bit.LUPD = 1;
  TCC1->CTRLBCLR.bit.LUPD = 1;
}

void pwm_r0_set(uint8_t hue)
{
  TCC0->CCB[1].bit.CCB = hue;
  while ( TCC0->SYNCBUSY.bit.CC1 )
  {
    ;
  }
}

void pwm_g0_set(uint8_t hue)
{
  TCC0->CCB[0].bit.CCB = hue;
  while ( TCC0->SYNCBUSY.bit.CC0 )
  {
    ;
  }
}

void pwm_b0_set(uint8_t hue)
{
  TCC1->CCB[1].bit.CCB = hue;
  while ( TCC1->SYNCBUSY.bit.CC1 )
  {
    ;
  }
}

//------------------------------------------------------------------------------
//      __   __              ___  ___
//     |__) |__) | \  /  /\   |  |__
//     |    |  \ |  \/  /~~\  |  |___
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      __                  __        __        __
//     /  `  /\  |    |    |__)  /\  /  ` |__/ /__`
//     \__, /~~\ |___ |___ |__) /~~\ \__, |  \ .__/
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//        __   __  , __
//     | /__` |__)  /__`   
//     | .__/ |  \  .__/
//
//------------------------------------------------------------------------------
