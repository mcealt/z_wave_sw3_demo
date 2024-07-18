/**
 * @file
 * Platform abstraction for Switch On/Off application
 * 
 * @copyright 2021 Silicon Laboratories Inc.
 */
#include <app_hw.h>
#include <board.h>
#include <Assert.h>
#include <events.h>
#include <board.h>
#include <zaf_event_distributor_soc.h>
#include "CC_BinarySwitch.h"

//#define DEBUGPRINT
#include "DebugPrint.h"

/**
 * To run this feature in the application, please press and hold BTN0 on the EXP board.
 */
#ifndef HMI_CHECKER_ENABLE
#define HMI_CHECKER_ENABLE        0
#endif

#if (HMI_CHECKER_ENABLE)
#include "HMI_checker.h"
#endif

#define SWITCH_TOGGLE_BTN1   APP_BUTTON_A
#define SWITCH_STATUS_LED1   APP_LED_A

#define SWITCH_TOGGLE_BTN2   APP_BUTTON_B
#define SWITCH_STATUS_LED2   APP_LED_B

#define SWITCH_TOGGLE_BTN3   APP_BUTTON_C
#define SWITCH_STATUS_LED3   APP_LED_C

/* Ensure we did not allocate the same physical button or led to more than one function */
STATIC_ASSERT(APP_BUTTON_LEARN_RESET != SWITCH_TOGGLE_BTN1,
              STATIC_ASSERT_FAILED_button_overlap);
STATIC_ASSERT(APP_LED_INDICATOR != SWITCH_STATUS_LED1,
              STATIC_ASSERT_FAILED_led_overlap);


static void button_handler(BUTTON_EVENT event, bool is_called_from_isr)
{
#if (HMI_CHECKER_ENABLE)
  HMIChecker_buttonEventHandler(event);
#endif

  uint8_t app_event = EVENT_EMPTY;

  if (BTN_EVENT_LONG_PRESS(APP_BUTTON_LEARN_RESET) == event)
  {
    app_event = EVENT_SYSTEM_RESET;
  }
  else if (BTN_EVENT_SHORT_PRESS(APP_BUTTON_LEARN_RESET) == event)
  {
    app_event = EVENT_SYSTEM_LEARNMODE_TOGGLE;
  }
  else if (BTN_EVENT_SHORT_PRESS(SWITCH_TOGGLE_BTN1) == event)
  {
    app_event = EVENT_APP_TOGGLE_LED1;
  }
  else if (BTN_EVENT_SHORT_PRESS(SWITCH_TOGGLE_BTN2) == event)
  {
    app_event = EVENT_APP_TOGGLE_LED2;
  }
  else if (BTN_EVENT_SHORT_PRESS(SWITCH_TOGGLE_BTN3) == event)
  {
    app_event = EVENT_APP_TOGGLE_LED3;
  }
  else if (BTN_EVENT_HOLD(SWITCH_TOGGLE_BTN1) == event)
  {
    app_event = EVENT_APP_SEND_NIF;
  }

  if (app_event != EVENT_EMPTY)
  {
    if (is_called_from_isr)
    {
      zaf_event_distributor_enqueue_app_event_from_isr(app_event);
    }
    else
    {
      zaf_event_distributor_enqueue_app_event(app_event);
    }
  }
}

void app_hw_init(void)
{
  DPRINT("--------------------------------\n");
  DPRINTF("%s: Toggle switch on/off\n", Board_GetButtonLabel(SWITCH_TOGGLE_BTN1));
  DPRINT("      Hold 5 sec: TX INIF\n");
  DPRINTF("%s: Toggle learn mode\n", Board_GetButtonLabel(APP_BUTTON_LEARN_RESET));
  DPRINT("      Hold 5 sec: Reset\n");
  DPRINTF("%s: Learn mode + identify\n", Board_GetLedLabel(APP_LED_INDICATOR));
  DPRINTF("%s: Switch status on/off\n", Board_GetLedLabel(SWITCH_STATUS_LED1));
  DPRINT("--------------------------------\n\n");

  Board_SetButtonCallback(button_handler);
  Board_EnableButton(APP_BUTTON_LEARN_RESET);
  Board_EnableButton(SWITCH_TOGGLE_BTN1);
  Board_EnableButton(SWITCH_TOGGLE_BTN2);
  Board_EnableButton(SWITCH_TOGGLE_BTN3);

#if (HMI_CHECKER_ENABLE)
  HMIChecker_Init();  // Used to check the LEDs on the EXP board.
#endif
}

void cc_binary_switch_handler1(cc_binary_switch_t * p_switch)
{
  uint8_t value = ZAF_Actuator_GetCurrentValue(&p_switch->actuator);
  Board_SetLed(SWITCH_STATUS_LED1, (value > 0) ? LED_ON : LED_OFF);
}

void cc_binary_switch_handler2(cc_binary_switch_t * p_switch)
{
  uint8_t value = ZAF_Actuator_GetCurrentValue(&p_switch->actuator);
  Board_SetLed(SWITCH_STATUS_LED2, (value > 0) ? LED_ON : LED_OFF);
}

void cc_binary_switch_handler3(cc_binary_switch_t * p_switch)
{
  uint8_t value = ZAF_Actuator_GetCurrentValue(&p_switch->actuator);
  Board_SetLed(SWITCH_STATUS_LED3, (value > 0) ? LED_ON : LED_OFF);
}