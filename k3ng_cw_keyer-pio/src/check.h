int paddle_pin_read(int pin_to_read)
{

  // Updated code provided by Fred, VK2EFL
  //
  // Note on OPTION_DIRECT_PADDLE_PIN_READS_MEGA, OPTION_DIRECT_PADDLE_PIN_READS_UNO, OPTION_SAVE_MEMORY_NANOKEYER
  // For Mega2560 and Uno/Nano speed up paddle pin reads by direct read of the register
  // it saves about 340 bytes of code too

#ifndef FEATURE_CAPACITIVE_PADDLE_PINS
#ifndef OPTION_INVERT_PADDLE_PIN_LOGIC
#ifdef OPTION_DIRECT_PADDLE_PIN_READS_MEGA // after April 2019, if this option is not defined then a direct read of the pins can never occur
  switch (pin_to_read)
  {
  case 2:
    return (bitRead(PINE, 4));
    break;
  case 5:
    return (bitRead(PINE, 3));
    break;
  } // end switch
#endif                                    // OPTION_DIRECT_PADDLE_READS_MEGA
#ifdef OPTION_DIRECT_PADDLE_PIN_READS_UNO // since with this verion, April 2019, this option is not defined then a direct read of the pins can never occur
  return (bitRead(PIND, pin_to_read));    // use this line on Unos and Nanos
#endif                                    // OPTION_DIRECT_PADDLE_PIN_READS_UNO
#ifdef OPTION_SAVE_MEMORY_NANOKEYER       //
  switch (pin_to_read)
  {
  case 2:
    return (bitRead(PIND, 2));
    break;
  case 5:
    return (bitRead(PIND, 5));
    break;
  case 8:
    return (bitRead(PINB, 0));
    break;
  } // end switch
#endif // OPTION_SAVE_MEMORY_NANOKEYER
#if !defined(OPTION_DIRECT_PADDLE_PIN_READS_UNO) && !defined(OPTION_DIRECT_PADDLE_PIN_READS_MEGA) && !defined(OPTION_SAVE_MEMORY_NANOKEYER)
  return digitalRead(pin_to_read); // code using digitalRead
#endif                             // !defined(OPTION_DIRECT_PADDLE_PIN_READS_UNO) && !defined(OPTION_DIRECT_PADDLE_PIN_READS_MEGA)
#else                              // !OPTION_INVERT_PADDLE_PIN_LOGIC
  return !digitalRead(pin_to_read); // we do the regular digitalRead() if none of the direct register read options are valid
#endif                             // !OPTION_INVERT_PADDLE_PIN_LOGIC
#else                              // !FEATURE_CAPACITIVE_PADDLE_PINS
  if (capactive_paddle_pin_inhibit_pin)
  {
    if (digitalRead(capactive_paddle_pin_inhibit_pin) == HIGH)
    {
      return digitalRead(pin_to_read);
    } // end if
  } // end if (capactive_paddle_pin_inhibit_pin)
  if (read_capacitive_pin(pin_to_read) > capacitance_threshold)
    return LOW;
  else
    return HIGH;
#endif                             // !FEATURE_CAPACITIVE_PADDLE_PINS

  // #ifndef FEATURE_CAPACITIVE_PADDLE_PINS
  //   #ifndef OPTION_INVERT_PADDLE_PIN_LOGIC
  //     #if defined(OPTION_DIRECT_PADDLE_PIN_READS_MEGA)
  //       switch(pin_to_read){
  //         case 2: return(bitRead(PINE,4));break;
  //         case 5: return(bitRead(PINE,3));break;
  //       }
  //     #else //OPTION_DIRECT_PADDLE_READS_MEGA
  //       return digitalRead(pin_to_read);
  //     #endif //OPTION_DIRECT_PADDLE_READS_MEGA
  //   #else
  //     return !digitalRead(pin_to_read);
  //   #endif
  // #else
  //     if (capactive_paddle_pin_inhibit_pin){
  //       if (digitalRead(capactive_paddle_pin_inhibit_pin) == HIGH){
  //         return digitalRead(pin_to_read);
  //       }

  //     }

  //     if (read_capacitive_pin(pin_to_read) > capacitance_threshold) {
  //       return LOW;
  //     } else {
  //       return HIGH;
  //     }

  // #endif //FEATURE_CAPACITIVE_PADDLE_PINS
}

//---------------------------------------------------------------------

#if defined(OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE) && defined(FEATURE_WINKEY_EMULATION)
void service_winkey_breakin()
{

  if (send_winkey_breakin_byte_flag)
  {
    winkey_port_write(0xc2 | winkey_sending | winkey_xoff, 0); // 0xc2 - BREAKIN bit set high
    winkey_interrupted = 1;
    send_winkey_breakin_byte_flag = 0;
#ifdef DEBUG_WINKEY
    debug_serial_port->println("service_winkey_breakin: winkey_interrupted = 1");
#endif
  }
}
#endif // defined(OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE) && defined(FEATURE_WINKEY_EMULATION)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL)
void check_serial()
{

#if defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE)
  if (check_serial_override)
  {
    return;
  }
#endif

#ifdef DEBUG_SERIAL_SEND_CW_CALLOUT
  byte debug_serial_send_cw[2];
  byte previous_tx = 0;
  byte previous_sidetone = 0;
#endif

#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering check_serial"));
#endif

#ifdef FEATURE_WINKEY_EMULATION
  if (primary_serial_port_mode == SERIAL_WINKEY_EMULATION)
  {
    service_winkey(WINKEY_HOUSEKEEPING);
  }
#endif

  while (primary_serial_port->available() > 0)
  {
    incoming_serial_byte = primary_serial_port->read();
#ifdef FEATURE_SLEEP
    last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
    last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM
#ifdef DEBUG_SERIAL_SEND_CW_CALLOUT
    debug_serial_send_cw[0] = (incoming_serial_byte & 0xf0) >> 4;
    debug_serial_send_cw[1] = incoming_serial_byte & 0x0f;
    for (byte x = 0; x < 2; x++)
    {
      if (debug_serial_send_cw[x] < 10)
      {
        debug_serial_send_cw[x] = debug_serial_send_cw[x] + 48;
      }
      else
      {
        debug_serial_send_cw[x] = debug_serial_send_cw[x] + 55;
      }
    }
    previous_tx = key_tx;
    key_tx = 0;
    previous_sidetone = configuration.sidetone_mode;
    configuration.sidetone_mode = SIDETONE_ON;
    send_char(debug_serial_send_cw[0], 0);
    send_char(debug_serial_send_cw[1], 0);
    key_tx = previous_tx;
    configuration.sidetone_mode = previous_sidetone;
#endif

#if !defined(FEATURE_WINKEY_EMULATION) && !defined(FEATURE_COMMAND_LINE_INTERFACE)
    primary_serial_port->println(F("No serial features enabled..."));
#endif

#if defined(FEATURE_WINKEY_EMULATION) && defined(FEATURE_COMMAND_LINE_INTERFACE)
    if (primary_serial_port_mode == SERIAL_WINKEY_EMULATION)
    {
      service_winkey(SERVICE_SERIAL_BYTE);
    }
    else
    {
      service_command_line_interface(primary_serial_port);
    }
#else // defined(FEATURE_WINKEY_EMULATION) && defined(FEATURE_COMMAND_LINE_INTERFACE)
#ifdef FEATURE_COMMAND_LINE_INTERFACE
    service_command_line_interface(primary_serial_port);
#endif // FEATURE_COMMAND_LINE_INTERFACE
#ifdef FEATURE_WINKEY_EMULATION
    service_winkey(SERVICE_SERIAL_BYTE);
#endif // FEATURE_WINKEY_EMULATION
#endif // defined(FEATURE_WINKEY_EMULATION) && defined(FEATURE_COMMAND_LINE_INTERFACE)

  } // while (primary_serial_port->available() > 0)

#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
  while (secondary_serial_port->available() > 0)
  {
    incoming_serial_byte = secondary_serial_port->read();
#ifdef FEATURE_SLEEP
    last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
    last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM
#ifdef DEBUG_SERIAL_SEND_CW_CALLOUT
    debug_serial_send_cw[0] = (incoming_serial_byte & 0xf0) >> 4;
    debug_serial_send_cw[1] = incoming_serial_byte & 0x0f;
    for (byte x = 0; x < 2; x++)
    {
      if (debug_serial_send_cw[x] < 10)
      {
        debug_serial_send_cw[x] = debug_serial_send_cw[x] + 48;
      }
      else
      {
        debug_serial_send_cw[x] = debug_serial_send_cw[x] + 55;
      }
    }
    previous_tx = key_tx;
    key_tx = 0;
    previous_sidetone = configuration.sidetone_mode;
    configuration.sidetone_mode = SIDETONE_ON;
    send_char(debug_serial_send_cw[0], 0);
    send_char(debug_serial_send_cw[1], 0);
    key_tx = previous_tx;
    configuration.sidetone_mode = previous_sidetone;
#endif // DEBUG_SERIAL_SEND_CW_CALLOUT
    service_command_line_interface(secondary_serial_port);
  } //  while (secondary_serial_port->available() > 0)
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
}
#endif // defined(FEATURE_SERIAL)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL_HELP) && defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_page_pause(PRIMARY_SERIAL_CLS *port_to_use, byte seconds_timeout)
{

  unsigned long pause_start_time = millis();

  port_to_use->println("\r\nPress enter...");
  while ((!port_to_use->available()) && (((millis() - pause_start_time) / 1000) < seconds_timeout))
  {
  }
  while (port_to_use->available())
  {
    port_to_use->read();
  }
}
#endif // defined(FEATURE_SERIAL_HELP) && defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)

//---------------------------------------------------------------------

void speed_set(int wpm_set)
{

  if ((wpm_set > 0) && (wpm_set < 1000))
  {
    configuration.wpm = wpm_set;
    config_dirty = 1;

#ifdef FEATURE_DYNAMIC_DAH_TO_DIT_RATIO
    if ((configuration.wpm >= DYNAMIC_DAH_TO_DIT_RATIO_LOWER_LIMIT_WPM) && (configuration.wpm <= DYNAMIC_DAH_TO_DIT_RATIO_UPPER_LIMIT_WPM))
    {
      int dynamicweightvalue = map(configuration.wpm, DYNAMIC_DAH_TO_DIT_RATIO_LOWER_LIMIT_WPM, DYNAMIC_DAH_TO_DIT_RATIO_UPPER_LIMIT_WPM, DYNAMIC_DAH_TO_DIT_RATIO_LOWER_LIMIT_RATIO, DYNAMIC_DAH_TO_DIT_RATIO_UPPER_LIMIT_RATIO);
      configuration.dah_to_dit_ratio = dynamicweightvalue;
    }
#endif // FEATURE_DYNAMIC_DAH_TO_DIT_RATIO

#ifdef FEATURE_LED_RING
    update_led_ring();
#endif // FEATURE_LED_RING

#ifdef FEATURE_MIDI
    midi_send_wpm_response();
#endif

#ifdef FEATURE_DISPLAY
    lcd_center_print_timed_wpm();
#endif
  }
}

//-------------------------------------------------------------------------------------------------------

void check_dit_paddle()
{

  byte pin_value = 0;
  byte dit_paddle = 0;
#ifdef OPTION_DIT_PADDLE_NO_SEND_ON_MEM_RPT
  static byte memory_rpt_interrupt_flag = 0;
#endif

  if (configuration.paddle_mode == PADDLE_NORMAL)
  {
    dit_paddle = paddle_left;
  }
  else
  {
    dit_paddle = paddle_right;
  }

  pin_value = paddle_pin_read(dit_paddle);

#if defined(FEATURE_USB_MOUSE) || defined(FEATURE_USB_KEYBOARD)
  if (usb_dit)
  {
    pin_value = 0;
  }
#endif

#ifdef OPTION_DIT_PADDLE_NO_SEND_ON_MEM_RPT
  if (pin_value && memory_rpt_interrupt_flag)
  {
    memory_rpt_interrupt_flag = 0;
    sending_mode = MANUAL_SENDING;
    // loop_element_lengths(3, 0, configuration.wpm);
    dit_buffer = 0;
  }
#endif

#ifdef OPTION_DIT_PADDLE_NO_SEND_ON_MEM_RPT
  if ((pin_value == 0) && (memory_rpt_interrupt_flag == 0))
  {
#else
  if (pin_value == 0)
  {
#endif
#ifdef FEATURE_DEAD_OP_WATCHDOG
    if (dit_buffer == 0)
    {
      dit_counter++;
      dah_counter = 0;
    }
#endif
    dit_buffer = 1;

#if defined(OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE) && defined(FEATURE_WINKEY_EMULATION)
    if (!winkey_interrupted && winkey_host_open && !winkey_breakin_status_byte_inhibit)
    {
      send_winkey_breakin_byte_flag = 1;
      // winkey_port_write(0xc2|winkey_sending|winkey_xoff); // 0xc2 - BREAKIN bit set high
      // winkey_interrupted = 1;

      // tone(sidetone_line,1000);
      // delay(500);
      // noTone(sidetone_line);

      dit_buffer = 0;
    }
#endif // defined(OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE) && defined(FEATURE_WINKEY_EMULATION)

#ifdef FEATURE_SLEEP
    last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
    last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM
    manual_ptt_invoke = 0;
#ifdef FEATURE_MEMORIES
    if (repeat_memory < 255)
    {
      repeat_memory = 255;
#ifdef OPTION_DIT_PADDLE_NO_SEND_ON_MEM_RPT
      dit_buffer = 0;
      while (!paddle_pin_read(dit_paddle))
      {
      };
      memory_rpt_interrupt_flag = 1;
#endif
    }
#endif
    clear_send_buffer();
  }
}

//-------------------------------------------------------------------------------------------------------

void check_dah_paddle()
{

  byte pin_value = 0;
  byte dah_paddle;

  if (configuration.paddle_mode == PADDLE_NORMAL)
  {
    dah_paddle = paddle_right;
  }
  else
  {
    dah_paddle = paddle_left;
  }

  pin_value = paddle_pin_read(dah_paddle);

#if defined(FEATURE_USB_MOUSE) || defined(FEATURE_USB_KEYBOARD)
  if (usb_dah)
  {
    pin_value = 0;
  }
#endif

  if (pin_value == 0)
  {
#ifdef FEATURE_DEAD_OP_WATCHDOG
    if (dah_buffer == 0)
    {
      dah_counter++;
      dit_counter = 0;
    }
#endif
    dah_buffer = 1;

#if defined(OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE) && defined(FEATURE_WINKEY_EMULATION)
    if (!winkey_interrupted && winkey_host_open && !winkey_breakin_status_byte_inhibit)
    {
      send_winkey_breakin_byte_flag = 1;
      dah_buffer = 0;
    }
#endif // defined(OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE) && defined(FEATURE_WINKEY_EMULATION)

#ifdef FEATURE_SLEEP
    last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
    last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM
#ifdef FEATURE_MEMORIES
    repeat_memory = 255;
#endif
    clear_send_buffer();
    manual_ptt_invoke = 0;
  }
}

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_MEMORIES) && defined(FEATURE_BUTTONS)
void check_the_memory_buttons()
{

  byte analogbuttontemp = button_array.Pressed();
  if ((analogbuttontemp > 0) && (analogbuttontemp < (number_of_memories + 1)) && ((millis() - button_last_add_to_send_buffer_time) > 400))
  {
    add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
    add_to_send_buffer(analogbuttontemp - 1);
    button_last_add_to_send_buffer_time = millis();
  }
}
#endif

//------------------------------------------------------------------

#ifdef FEATURE_BUTTONS
byte analogbuttonread(byte button_number)
{

  if (button_array.Pressed(button_number))
  {
    return 1;
  }
  return 0;
}
#endif

//------------------------------------------------------------------

void loop_element_lengths(float lengths, float additional_time_ms, int speed_wpm_in)
{

#if defined(FEATURE_SERIAL) && !defined(OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW)
  loop_element_lengths_breakout_flag = 1;
#endif // FEATURE_SERIAL

#if defined(DEBUG_LOOP_ELEMENT_LENGTHS)
  debug_serial_port->println("loop_element_lengths: enter");
#endif

  float element_length;

  if (lengths <= 0)
  {
    return;
  }

#if defined(FEATURE_FARNSWORTH)

  if ((lengths == 1) && (speed_wpm_in == 0))
  {
    element_length = additional_time_ms;
  }
  else
  {
    if (speed_mode == SPEED_NORMAL)
    {
      element_length = 1200 / speed_wpm_in;
    }
    else
    {
      element_length = qrss_dit_length * 1000;
    }
  }

#else  // FEATURE_FARNSWORTH
  if (speed_mode == SPEED_NORMAL)
  {
    element_length = 1200 / speed_wpm_in;
  }
  else
  {
    element_length = qrss_dit_length * 1000;
  }
#endif // FEATURE_FARNSWORTH

  unsigned long ticks;
  if ((long(element_length * lengths * 1000) + long(additional_time_ms * 1000)) < 0)
  {
    return;
  }
  else
  {
    ticks = long(element_length * lengths * 1000) + long(additional_time_ms * 1000); // improvement from Paul, K1XM
  }
  unsigned long start = micros();

#if defined(FEATURE_SERIAL) && !defined(OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW)
  while (((micros() - start) < ticks) && (service_tx_inhibit_and_pause() == 0) && loop_element_lengths_breakout_flag)
  {
#else
  while (((micros() - start) < ticks) && (service_tx_inhibit_and_pause() == 0))
  {
#endif

    check_ptt_tail();

#if defined(FEATURE_SERIAL) && !defined(OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW)
    if (((ticks - (micros() - start)) > (10 * 1000)) && (sending_mode == AUTOMATIC_SENDING))
    {
      check_serial();
      if (loop_element_lengths_breakout_flag == 0)
      {
        dump_current_character_flag = 1;
      }
    }
#endif // FEATURE_SERIAL

#if defined(FEATURE_INTERNET_LINK) /*&& !defined(OPTION_INTERNET_LINK_NO_UDP_SVC_DURING_KEY_DOWN)*/
    // if ((millis() > 1000)  && ((millis()-start) > FEATURE_INTERNET_LINK_SVC_DURING_LOOP_TIME_MS)){
    if ((ticks - (micros() - start)) > (FEATURE_INTERNET_LINK_SVC_DURING_LOOP_TIME_MS * 1000))
    {
      service_udp_send_buffer();
      service_udp_receive();
      service_internet_link_udp_receive_buffer();
    }
#endif // FEATURE_INTERNET_LINK

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
    wdt_reset();
#endif // OPTION_WATCHDOG_TIMER

#if defined(FEATURE_ROTARY_ENCODER)
    check_rotary_encoder();
#endif // FEATURE_ROTARY_ENCODER

#if defined(FEATURE_USB_KEYBOARD) || defined(FEATURE_USB_MOUSE)
    service_usb();
#endif // FEATURE_USB_KEYBOARD || FEATURE_USB_MOUSE

#if defined(FEATURE_PTT_INTERLOCK)
    service_ptt_interlock();
#endif // FEATURE_PTT_INTERLOCK

#if defined(FEATURE_4x4_KEYPAD) || defined(FEATURE_3x4_KEYPAD)
    service_keypad();
#endif

#if defined(FEATURE_DISPLAY)
    if ((ticks - (micros() - start)) > (10 * 1000))
    {
      service_display();
    }
#endif

    if ((configuration.keyer_mode != ULTIMATIC) && (configuration.keyer_mode != SINGLE_PADDLE))
    {
      if ((configuration.keyer_mode == IAMBIC_A) && (paddle_pin_read(paddle_left) == LOW) && (paddle_pin_read(paddle_right) == LOW))
      {
        iambic_flag = 1;
      }

#ifndef FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING
      if (being_sent == SENDING_DIT)
      {
        check_dah_paddle();
      }
      else
      {
        if (being_sent == SENDING_DAH)
        {
          check_dit_paddle();
        }
        else
        {
          check_dah_paddle();
          check_dit_paddle();
        }
      }
#else  ////FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING
      // if (configuration.cmos_super_keyer_iambic_b_timing_on){
      if ((configuration.cmos_super_keyer_iambic_b_timing_on) && (sending_mode == MANUAL_SENDING))
      {
        if ((float(float(micros() - start) / float(ticks)) * 100) >= configuration.cmos_super_keyer_iambic_b_timing_percent)
        {
          // if ((float(float(millis()-starttime)/float(starttime-ticks))*100) >= configuration.cmos_super_keyer_iambic_b_timing_percent) {
          if (being_sent == SENDING_DIT)
          {
            check_dah_paddle();
          }
          else
          {
            if (being_sent == SENDING_DAH)
            {
              check_dit_paddle();
            }
          }
        }
        else
        {
          if (((being_sent == SENDING_DIT) || (being_sent == SENDING_DAH)) && (paddle_pin_read(paddle_left) == LOW) && (paddle_pin_read(paddle_right) == LOW))
          {
            dah_buffer = 0;
            dit_buffer = 0;
          }
        }
      }
      else
      {
        if (being_sent == SENDING_DIT)
        {
          check_dah_paddle();
        }
        else
        {
          if (being_sent == SENDING_DAH)
          {
            check_dit_paddle();
          }
          else
          {
            check_dah_paddle();
            check_dit_paddle();
          }
        }
      }
#endif // FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING
    }
    else
    { //(configuration.keyer_mode != ULTIMATIC)
#ifndef OPTION_NO_ULTIMATIC
      if (being_sent == SENDING_DIT)
      {
        check_dah_paddle();
      }
      else
      {
        if (being_sent == SENDING_DAH)
        {
          check_dit_paddle();
        }
        else
        {
          check_dah_paddle();
          check_dit_paddle();
        }
      }
#endif // OPTION_NO_ULTIMATIC
    }

#if defined(FEATURE_MEMORIES) && defined(FEATURE_BUTTONS)
    check_the_memory_buttons();
#endif

// blow out prematurely if we're automatic sending and a paddle gets hit
#ifdef FEATURE_BUTTONS
    if (sending_mode == AUTOMATIC_SENDING && (paddle_pin_read(paddle_left) == LOW || paddle_pin_read(paddle_right) == LOW || analogbuttonread(0) || dit_buffer || dah_buffer))
    {
      if (keyer_machine_mode == KEYER_NORMAL)
      {
        sending_mode = AUTOMATIC_SENDING_INTERRUPTED;
        automatic_sending_interruption_time = millis();
        return;
      }
    }
#else
    if (sending_mode == AUTOMATIC_SENDING && (paddle_pin_read(paddle_left) == LOW || paddle_pin_read(paddle_right) == LOW || dit_buffer || dah_buffer))
    {
      if (keyer_machine_mode == KEYER_NORMAL)
      {
        sending_mode = AUTOMATIC_SENDING_INTERRUPTED;
        automatic_sending_interruption_time = millis();

#ifdef FEATURE_SO2R_BASE
        so2r_set_rx();
#endif

        return;
      }
    }
#endif

#ifdef FEATURE_STRAIGHT_KEY
    service_straight_key();
#endif // FEATURE_STRAIGHT_KEY

#if defined(FEATURE_WEB_SERVER)
    if (speed_mode == SPEED_QRSS)
    {
      service_web_server();
    }
#endif // FEATURE_WEB_SERVER

#ifdef FEATURE_SO2R_SWITCHES
    so2r_switches();
#endif

  } // while ((millis() < endtime) && (millis() > 200))

  if ((configuration.keyer_mode == IAMBIC_A) && (iambic_flag) && (paddle_pin_read(paddle_left) == HIGH) && (paddle_pin_read(paddle_right) == HIGH))
  {
    iambic_flag = 0;
    dit_buffer = 0;
    dah_buffer = 0;
  }

  if ((being_sent == SENDING_DIT) || (being_sent == SENDING_DAH))
  {
    if (configuration.dit_buffer_off)
    {
      dit_buffer = 0;
    }
    if (configuration.dah_buffer_off)
    {
      dah_buffer = 0;
    }
  }

#if defined(DEBUG_LOOP_ELEMENT_LENGTHS)
  debug_serial_port->println("loop_element_lengths: exit");
#endif

} // void loop_element_lengths

//-------------------------------------------------------------------------------------------------------

void check_paddles()
{

#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering check_paddles"));
#endif

#define NO_CLOSURE 0
#define DIT_CLOSURE_DAH_OFF 1
#define DAH_CLOSURE_DIT_OFF 2
#define DIT_CLOSURE_DAH_ON 3
#define DAH_CLOSURE_DIT_ON 4

  if (keyer_machine_mode == BEACON)
  {
    return;
  }

  static byte last_closure = NO_CLOSURE;

  check_dit_paddle();
  check_dah_paddle();

#ifdef FEATURE_WINKEY_EMULATION
  if (winkey_dit_invoke)
  {
    dit_buffer = 1;
  }
  if (winkey_dah_invoke)
  {
    dah_buffer = 1;
  }
#endif // FEATURE_WINKEY_EMULATION
#ifndef OPTION_NO_ULTIMATIC
  if (configuration.keyer_mode == ULTIMATIC)
  {
    if (ultimatic_mode == ULTIMATIC_NORMAL)
    {

      switch (last_closure)
      {
      case DIT_CLOSURE_DAH_OFF:
        if (dah_buffer)
        {
          if (dit_buffer)
          {
            last_closure = DAH_CLOSURE_DIT_ON;
            dit_buffer = 0;
          }
          else
          {
            last_closure = DAH_CLOSURE_DIT_OFF;
          }
        }
        else
        {
          if (!dit_buffer)
          {
            last_closure = NO_CLOSURE;
          }
        }
        break;
      case DIT_CLOSURE_DAH_ON:
        if (dit_buffer)
        {
          if (dah_buffer)
          {
            dah_buffer = 0;
          }
          else
          {
            last_closure = DIT_CLOSURE_DAH_OFF;
          }
        }
        else
        {
          if (dah_buffer)
          {
            last_closure = DAH_CLOSURE_DIT_OFF;
          }
          else
          {
            last_closure = NO_CLOSURE;
          }
        }
        break;

      case DAH_CLOSURE_DIT_OFF:
        if (dit_buffer)
        {
          if (dah_buffer)
          {
            last_closure = DIT_CLOSURE_DAH_ON;
            dah_buffer = 0;
          }
          else
          {
            last_closure = DIT_CLOSURE_DAH_OFF;
          }
        }
        else
        {
          if (!dah_buffer)
          {
            last_closure = NO_CLOSURE;
          }
        }
        break;

      case DAH_CLOSURE_DIT_ON:
        if (dah_buffer)
        {
          if (dit_buffer)
          {
            dit_buffer = 0;
          }
          else
          {
            last_closure = DAH_CLOSURE_DIT_OFF;
          }
        }
        else
        {
          if (dit_buffer)
          {
            last_closure = DIT_CLOSURE_DAH_OFF;
          }
          else
          {
            last_closure = NO_CLOSURE;
          }
        }
        break;

      case NO_CLOSURE:
        if ((dit_buffer) && (!dah_buffer))
        {
          last_closure = DIT_CLOSURE_DAH_OFF;
        }
        else
        {
          if ((dah_buffer) && (!dit_buffer))
          {
            last_closure = DAH_CLOSURE_DIT_OFF;
          }
          else
          {
            if ((dit_buffer) && (dah_buffer))
            {
              // need to handle dit/dah priority here
              last_closure = DIT_CLOSURE_DAH_ON;
              dah_buffer = 0;
            }
          }
        }
        break;
      }
    }
    else
    { // if (ultimatic_mode == ULTIMATIC_NORMAL)
      if ((dit_buffer) && (dah_buffer))
      { // dit or dah priority mode
        if (ultimatic_mode == ULTIMATIC_DIT_PRIORITY)
        {
          dah_buffer = 0;
        }
        else
        {
          dit_buffer = 0;
        }
      }
    } // if (ultimatic_mode == ULTIMATIC_NORMAL)
  } // if (configuration.keyer_mode == ULTIMATIC)
#endif // OPTION_NO_ULTIMATIC

  if (configuration.keyer_mode == SINGLE_PADDLE)
  {
    switch (last_closure)
    {
    case DIT_CLOSURE_DAH_OFF:
      if (dit_buffer)
      {
        if (dah_buffer)
        {
          dah_buffer = 0;
        }
        else
        {
          last_closure = DIT_CLOSURE_DAH_OFF;
        }
      }
      else
      {
        if (dah_buffer)
        {
          last_closure = DAH_CLOSURE_DIT_OFF;
        }
        else
        {
          last_closure = NO_CLOSURE;
        }
      }
      break;

    case DIT_CLOSURE_DAH_ON:

      if (dah_buffer)
      {
        if (dit_buffer)
        {
          last_closure = DAH_CLOSURE_DIT_ON;
          dit_buffer = 0;
        }
        else
        {
          last_closure = DAH_CLOSURE_DIT_OFF;
        }
      }
      else
      {
        if (!dit_buffer)
        {
          last_closure = NO_CLOSURE;
        }
      }
      break;

    case DAH_CLOSURE_DIT_OFF:
      if (dah_buffer)
      {
        if (dit_buffer)
        {
          dit_buffer = 0;
        }
        else
        {
          last_closure = DAH_CLOSURE_DIT_OFF;
        }
      }
      else
      {
        if (dit_buffer)
        {
          last_closure = DIT_CLOSURE_DAH_OFF;
        }
        else
        {
          last_closure = NO_CLOSURE;
        }
      }
      break;

    case DAH_CLOSURE_DIT_ON:
      if (dit_buffer)
      {
        if (dah_buffer)
        {
          last_closure = DIT_CLOSURE_DAH_ON;
          dah_buffer = 0;
        }
        else
        {
          last_closure = DIT_CLOSURE_DAH_OFF;
        }
      }
      else
      {
        if (!dah_buffer)
        {
          last_closure = NO_CLOSURE;
        }
      }
      break;

    case NO_CLOSURE:
      if ((dit_buffer) && (!dah_buffer))
      {
        last_closure = DIT_CLOSURE_DAH_OFF;
      }
      else
      {
        if ((dah_buffer) && (!dit_buffer))
        {
          last_closure = DAH_CLOSURE_DIT_OFF;
        }
        else
        {
          if ((dit_buffer) && (dah_buffer))
          {
            // need to handle dit/dah priority here
            last_closure = DIT_CLOSURE_DAH_ON;
            dah_buffer = 0;
          }
        }
      }
      break;
    }
  } // if (configuration.keyer_mode == SINGLE_PADDLE)

  service_tx_inhibit_and_pause();
}

//-------------------------------------------------------------------------------------------------------

void send_dit()
{

  // notes: key_compensation is a straight x mS lengthening or shortening of the key down time
  //        weighting is

  unsigned int character_wpm = configuration.wpm;

#ifdef FEATURE_FARNSWORTH
  if ((sending_mode == AUTOMATIC_SENDING) && (configuration.wpm_farnsworth > configuration.wpm))
  {
    character_wpm = configuration.wpm_farnsworth;
#if defined(DEBUG_FARNSWORTH)
    debug_serial_port->println(F("send_dit: farns act"));
#endif
  }
#if defined(DEBUG_FARNSWORTH)

  else
  {
    debug_serial_port->println(F("send_dit: farns inact"));
  }
#endif
#endif // FEATURE_FARNSWORTH

  if (keyer_machine_mode == KEYER_COMMAND_MODE)
  {
    character_wpm = configuration.wpm_command_mode;
  }

  being_sent = SENDING_DIT;
  tx_and_sidetone_key(1);
#ifdef DEBUG_VARIABLE_DUMP
  dit_start_time = millis();
#endif
  if ((tx_key_dit) && (key_tx))
  {
    digitalWrite(tx_key_dit, tx_key_dit_and_dah_pins_active_state);
  }

#ifdef FEATURE_QLF
  if (qlf_active)
  {
    loop_element_lengths((1.0 * (float(configuration.weighting) / 50) * (random(qlf_dit_min, qlf_dit_max) / 100.0)), configuration.keying_compensation, character_wpm);
  }
  else
  {
    loop_element_lengths((1.0 * (float(configuration.weighting) / 50)), configuration.keying_compensation, character_wpm);
  }
#else  // FEATURE_QLF
  loop_element_lengths((1.0 * (float(configuration.weighting) / 50)), configuration.keying_compensation, character_wpm);
#endif // FEATURE_QLF

  if ((tx_key_dit) && (key_tx))
  {
    digitalWrite(tx_key_dit, tx_key_dit_and_dah_pins_inactive_state);
  }
#ifdef DEBUG_VARIABLE_DUMP
  dit_end_time = millis();
#endif
  tx_and_sidetone_key(0);

  loop_element_lengths((2.0 - (float(configuration.weighting) / 50)), (-1.0 * configuration.keying_compensation), character_wpm);

#ifdef FEATURE_AUTOSPACE

  byte autospace_end_of_character_flag = 0;

  if ((sending_mode == MANUAL_SENDING) && (configuration.autospace_active))
  {
    check_paddles();
  }
  if ((sending_mode == MANUAL_SENDING) && (configuration.autospace_active) && (dit_buffer == 0) && (dah_buffer == 0))
  {
    loop_element_lengths((float)configuration.autospace_timing_factor / (float)100, 0, configuration.wpm);
    autospace_end_of_character_flag = 1;
  }
#endif

#ifdef FEATURE_WINKEY_EMULATION
  if ((winkey_host_open) && (winkey_paddle_echo_activated) && (sending_mode == MANUAL_SENDING))
  {
    winkey_paddle_echo_buffer = (winkey_paddle_echo_buffer * 10) + 1;
    // winkey_paddle_echo_buffer_decode_time = millis() + (float(winkey_paddle_echo_buffer_decode_time_factor/float(configuration.wpm))*length_letterspace);
    winkey_paddle_echo_buffer_decode_time = millis() + (float(winkey_paddle_echo_buffer_decode_timing_factor * 1200.0 / float(configuration.wpm)) * length_letterspace);

#ifdef FEATURE_AUTOSPACE
    if (autospace_end_of_character_flag)
    {
      winkey_paddle_echo_buffer_decode_time = 0;
    }
#endif // FEATURE_AUTOSPACE
  }
#endif

#ifdef FEATURE_PADDLE_ECHO
  if (sending_mode == MANUAL_SENDING)
  {
    paddle_echo_buffer = (paddle_echo_buffer * 10) + 1;
    paddle_echo_buffer_decode_time = millis() + (((float)1200.0 / (float)configuration.wpm) * ((float)configuration.cw_echo_timing_factor / (float)100));

#ifdef FEATURE_AUTOSPACE
    if (autospace_end_of_character_flag)
    {
      paddle_echo_buffer_decode_time = 0;
    }
#endif // FEATURE_AUTOSPACE
  }
#endif // FEATURE_PADDLE_ECHO

#ifdef FEATURE_AUTOSPACE
  autospace_end_of_character_flag = 0;
#endif // FEATURE_AUTOSPACE

  being_sent = SENDING_NOTHING;
  last_sending_mode = sending_mode;

  check_paddles();
}

//-------------------------------------------------------------------------------------------------------

void send_dah()
{

  unsigned int character_wpm = configuration.wpm;

#ifdef FEATURE_FARNSWORTH
  if ((sending_mode == AUTOMATIC_SENDING) && (configuration.wpm_farnsworth > configuration.wpm))
  {
    character_wpm = configuration.wpm_farnsworth;
  }
#endif // FEATURE_FARNSWORTH

  if (keyer_machine_mode == KEYER_COMMAND_MODE)
  {
    character_wpm = configuration.wpm_command_mode;
  }

  being_sent = SENDING_DAH;
  tx_and_sidetone_key(1);
#ifdef DEBUG_VARIABLE_DUMP
  dah_start_time = millis();
#endif
  if ((tx_key_dah) && (key_tx))
  {
    digitalWrite(tx_key_dah, tx_key_dit_and_dah_pins_active_state);
  }

#ifdef FEATURE_QLF
  if (qlf_active)
  {
    loop_element_lengths((float(configuration.dah_to_dit_ratio / 100.0) * (float(configuration.weighting) / 50) * (random(qlf_dah_min, qlf_dah_max) / 100.0)), configuration.keying_compensation, character_wpm);
  }
  else
  {
    loop_element_lengths((float(configuration.dah_to_dit_ratio / 100.0) * (float(configuration.weighting) / 50)), configuration.keying_compensation, character_wpm);
  }
#else  // FEATURE_QLF
  loop_element_lengths((float(configuration.dah_to_dit_ratio / 100.0) * (float(configuration.weighting) / 50)), configuration.keying_compensation, character_wpm);
#endif // FEATURE_QLF

  if ((tx_key_dah) && (key_tx))
  {
    digitalWrite(tx_key_dah, tx_key_dit_and_dah_pins_inactive_state);
  }

#ifdef DEBUG_VARIABLE_DUMP
  dah_end_time = millis();
#endif

  tx_and_sidetone_key(0);

  loop_element_lengths((4.0 - (3.0 * (float(configuration.weighting) / 50))), (-1.0 * configuration.keying_compensation), character_wpm);

#ifdef FEATURE_AUTOSPACE

  byte autospace_end_of_character_flag = 0;

  if ((sending_mode == MANUAL_SENDING) && (configuration.autospace_active))
  {
    check_paddles();
  }
  if ((sending_mode == MANUAL_SENDING) && (configuration.autospace_active) && (dit_buffer == 0) && (dah_buffer == 0))
  {
    loop_element_lengths(2, 0, configuration.wpm);
    autospace_end_of_character_flag = 1;
  }
#endif

#ifdef FEATURE_WINKEY_EMULATION
  if ((winkey_host_open) && (winkey_paddle_echo_activated) && (sending_mode == MANUAL_SENDING))
  {
    winkey_paddle_echo_buffer = (winkey_paddle_echo_buffer * 10) + 2;
    // winkey_paddle_echo_buffer_decode_time = millis() + (float(winkey_paddle_echo_buffer_decode_time_factor/float(configuration.wpm))*length_letterspace);
    winkey_paddle_echo_buffer_decode_time = millis() + (float(winkey_paddle_echo_buffer_decode_timing_factor * 1200.0 / float(configuration.wpm)) * length_letterspace);
#ifdef FEATURE_AUTOSPACE
    if (autospace_end_of_character_flag)
    {
      winkey_paddle_echo_buffer_decode_time = 0;
    }
#endif // FEATURE_AUTOSPACE
  }
#endif

#ifdef FEATURE_PADDLE_ECHO
  if (sending_mode == MANUAL_SENDING)
  {
    paddle_echo_buffer = (paddle_echo_buffer * 10) + 2;
    paddle_echo_buffer_decode_time = millis() + (((float)1200.0 / (float)configuration.wpm) * ((float)configuration.cw_echo_timing_factor / (float)100));

#ifdef FEATURE_AUTOSPACE
    if (autospace_end_of_character_flag)
    {
      paddle_echo_buffer_decode_time = 0;
    }
#endif // FEATURE_AUTOSPACE
  }
#endif // FEATURE_PADDLE_ECHO

#ifdef FEATURE_AUTOSPACE
  autospace_end_of_character_flag = 0;
#endif // FEATURE_AUTOSPACE

  check_paddles();

  being_sent = SENDING_NOTHING;
  last_sending_mode = sending_mode;
}

/*

    The Dash

    by Linda Ellis


    I read of a man who stood to speak at a funeral of a friend.  He referred to the dates on the tombstone from the beginning...to the end.

    He noted that first came the date of birth and spoke of the following date with tears, but said what mattered most of all was the dash
     between those years.

    For that dash represents all the time they spent alive on earth and now only those who loved them know what that little line is worth.

    For it matters not, how much we own, the cars..the house...the cash.

      What matters is how we lived and loved and how we spend our dash.

    So think about this long and hard; are there things you'd like to change?

      For you never know how much time is left that still can be rearranged.

    To be less quick to anger and show appreciation more and love the people in our lives like we've never loved before.

    If we treat each other with respect and more often wear a smile...remembering that this special dash might only last a little while.

    So when your eulogy is being read, with your life's actions to rehash,


     would you be proud of the things they say about how you lived your dash?



*/

//-------------------------------------------------------------------------------------------------------

void send_the_dits_and_dahs(char const *cw_to_send)
{

  /* American Morse - Special Symbols

    ~  long dah (4 units)

    =  very long dah (5 units)

    &  an extra space (1 unit)

  */

  // debug_serial_port->println(F("send_the_dits_and_dahs()"));

  sending_mode = AUTOMATIC_SENDING;

#if defined(FEATURE_SERIAL) && !defined(OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW)
  dump_current_character_flag = 0;
#endif

#if defined(FEATURE_FARNSWORTH)
  float additional_intercharacter_time_ms;
#endif

  for (int x = 0; x < 12; x++)
  {
    switch (cw_to_send[x])
    {
    case '.':
      send_dit();
      break;
    case '-':
      send_dah();
      break;
#if defined(FEATURE_AMERICAN_MORSE) // this is a bit of a hack, but who cares!  :-)
    case '~':

      being_sent = SENDING_DAH;
      tx_and_sidetone_key(1);
      if ((tx_key_dah) && (key_tx))
      {
        digitalWrite(tx_key_dah, tx_key_dit_and_dah_pins_active_state);
      }
      loop_element_lengths((float(4.0) * (float(configuration.weighting) / 50)), configuration.keying_compensation, configuration.wpm);
      if ((tx_key_dah) && (key_tx))
      {
        digitalWrite(tx_key_dah, tx_key_dit_and_dah_pins_inactive_state);
      }
      tx_and_sidetone_key(0);
      loop_element_lengths((4.0 - (3.0 * (float(configuration.weighting) / 50))), (-1.0 * configuration.keying_compensation), configuration.wpm);
      break;

    case '=':
      being_sent = SENDING_DAH;
      tx_and_sidetone_key(1);
      if ((tx_key_dah) && (key_tx))
      {
        digitalWrite(tx_key_dah, tx_key_dit_and_dah_pins_active_state);
      }
      loop_element_lengths((float(5.0) * (float(configuration.weighting) / 50)), configuration.keying_compensation, configuration.wpm);
      if ((tx_key_dah) && (key_tx))
      {
        digitalWrite(tx_key_dah, tx_key_dit_and_dah_pins_inactive_state);
      }
      tx_and_sidetone_key(0);
      loop_element_lengths((4.0 - (3.0 * (float(configuration.weighting) / 50))), (-1.0 * configuration.keying_compensation), configuration.wpm);
      break;

    case '&':
      loop_element_lengths((4.0 - (3.0 * (float(configuration.weighting) / 50))), (-1.0 * configuration.keying_compensation), configuration.wpm);
      break;
#endif // FEATURE_AMERICAN_MORSE
    default:
      // return;
      x = 12;
      break;
    }

    if ((dit_buffer || dah_buffer || sending_mode == AUTOMATIC_SENDING_INTERRUPTED) && (keyer_machine_mode != BEACON))
    {
      dit_buffer = 0;
      dah_buffer = 0;
      // debug_serial_port->println(F("send_the_dits_and_dahs: AUTOMATIC_SENDING_INTERRUPTED"));
      // return;
      x = 12;
    }
#if defined(FEATURE_SERIAL)
    check_serial();
#if !defined(OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW)
    if (dump_current_character_flag)
    {
      x = 12;
    }
#endif
#endif

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
    wdt_reset();
#endif // OPTION_WATCHDOG_TIMER

  } // for (int x = 0;x < 12;x++)
}

//-------------------------------------------------------------------------------------------------------

void send_char(byte cw_char, byte omit_letterspace)
{
#ifdef DEBUG_SEND_CHAR
  debug_serial_port->print(F("send_char: called with cw_char:"));
  debug_serial_port->print((byte)cw_char);
  if (omit_letterspace)
  {
    debug_serial_port->print(F(" OMIT_LETTERSPACE"));
  }
  debug_serial_port->println();
#endif

#ifdef FEATURE_SLEEP
  last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
  last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM

  if ((cw_char == 10) || (cw_char == 13))
  {
    return;
  } // don't attempt to send carriage return or line feed

  sending_mode = AUTOMATIC_SENDING;

  if (char_send_mode == CW)
  {
    switch (cw_char)
    {
    case 'A':
      send_the_dits_and_dahs(".-");
      break;
    case 'B':
      send_the_dits_and_dahs("-...");
      break;
    case 'C':
      send_the_dits_and_dahs("-.-.");
      break;
    case 'D':
      send_the_dits_and_dahs("-..");
      break;
    case 'E':
      send_the_dits_and_dahs(".");
      break;
    case 'F':
      send_the_dits_and_dahs("..-.");
      break;
    case 'G':
      send_the_dits_and_dahs("--.");
      break;
    case 'H':
      send_the_dits_and_dahs("....");
      break;
    case 'I':
      send_the_dits_and_dahs("..");
      break;
    case 'J':
      send_the_dits_and_dahs(".---");
      break;
    case 'K':
      send_the_dits_and_dahs("-.-");
      break;
    case 'L':
      send_the_dits_and_dahs(".-..");
      break;
    case 'M':
      send_the_dits_and_dahs("--");
      break;
    case 'N':
      send_the_dits_and_dahs("-.");
      break;
    case 'O':
      send_the_dits_and_dahs("---");
      break;
    case 'P':
      send_the_dits_and_dahs(".--.");
      break;
    case 'Q':
      send_the_dits_and_dahs("--.-");
      break;
    case 'R':
      send_the_dits_and_dahs(".-.");
      break;
    case 'S':
      send_the_dits_and_dahs("...");
      break;
    case 'T':
      send_the_dits_and_dahs("-");
      break;
    case 'U':
      send_the_dits_and_dahs("..-");
      break;
    case 'V':
      send_the_dits_and_dahs("...-");
      break;
    case 'W':
      send_the_dits_and_dahs(".--");
      break;
    case 'X':
      send_the_dits_and_dahs("-..-");
      break;
    case 'Y':
      send_the_dits_and_dahs("-.--");
      break;
    case 'Z':
      send_the_dits_and_dahs("--..");
      break;

    case '0':
      send_the_dits_and_dahs("-----");
      break;
    case '1':
      send_the_dits_and_dahs(".----");
      break;
    case '2':
      send_the_dits_and_dahs("..---");
      break;
    case '3':
      send_the_dits_and_dahs("...--");
      break;
    case '4':
      send_the_dits_and_dahs("....-");
      break;
    case '5':
      send_the_dits_and_dahs(".....");
      break;
    case '6':
      send_the_dits_and_dahs("-....");
      break;
    case '7':
      send_the_dits_and_dahs("--...");
      break;
    case '8':
      send_the_dits_and_dahs("---..");
      break;
    case '9':
      send_the_dits_and_dahs("----.");
      break;

    case '=':
      send_the_dits_and_dahs("-...-");
      break;
    case '/':
      send_the_dits_and_dahs("-..-.");
      break;
    case '*':
      send_the_dits_and_dahs("-...-.-");
      break;
    case '.':
      send_the_dits_and_dahs(".-.-.-");
      break;
    case ',':
      send_the_dits_and_dahs("--..--");
      break;
    case '!':
      send_the_dits_and_dahs("--..--");
      break; // sp5iou 20180328
    // case '!': send_the_dits_and_dahs("-.-.--");break;//sp5iou 20180328
    case '\'':
      send_the_dits_and_dahs(".----.");
      break; // apostrophe
    case '(':
      send_the_dits_and_dahs("-.--.");
      break;
    case ')':
      send_the_dits_and_dahs("-.--.-");
      break;
    case '&':
      send_the_dits_and_dahs(".-...");
      break;
      // case '&': send_dit(); loop_element_lengths(3); send_dits(3); break;

    case '+':
      send_the_dits_and_dahs(".-.-.");
      break;
    case '-':
      send_the_dits_and_dahs("-....-");
      break;
    case '_':
      send_the_dits_and_dahs("..--.-");
      break;
    case '"':
      send_the_dits_and_dahs(".-..-.");
      break;
    case '$':
      send_the_dits_and_dahs("...-..-");
      break;
    case '@':
      send_the_dits_and_dahs(".--.-.");
      break;
    case '<':
      send_the_dits_and_dahs(".-.-.");
      break; // AR
    case '>':
      send_the_dits_and_dahs("...-.-");
      break; // SK

#if defined(OPTION_WINKEY_PROSIGN_COMPATIBILITY)
    case 0x5C:
      send_the_dits_and_dahs("-..-.");
      break; // Backslash
    case '[':
      send_the_dits_and_dahs(".-...");
      break;
    case ':':
      send_the_dits_and_dahs("-.--.");
      break;
    case ';':
      send_the_dits_and_dahs(".-.-");
      break;
    case ']':
      send_the_dits_and_dahs("-.--.");
      break;
#else
    case ':':
      send_the_dits_and_dahs("---...");
      break;
    case ';':
      send_the_dits_and_dahs("-.-.-.");
      break;

#endif

    case ' ':
      loop_element_lengths((configuration.length_wordspace - length_letterspace - 2), 0, configuration.wpm);
      break;

#ifdef OPTION_RUSSIAN_LANGUAGE_SEND_CLI // Contributed by Павел Бирюков, UA1AQC
    case 192:
      send_the_dits_and_dahs(".-");
      break; // А
    case 193:
      send_the_dits_and_dahs("-...");
      break; // Б
    case 194:
      send_the_dits_and_dahs(".--");
      break; // В
    case 195:
      send_the_dits_and_dahs("--.");
      break; // Г
    case 196:
      send_the_dits_and_dahs("-..");
      break; // Д
    case 197:
      send_the_dits_and_dahs(".");
      break; // Е
    case 168:
      send_the_dits_and_dahs(".");
      break; // Ё
    case 184:
      send_the_dits_and_dahs(".");
      break; // ё
    case 198:
      send_the_dits_and_dahs("...-");
      break; // Ж
    case 199:
      send_the_dits_and_dahs("--..");
      break; // З
    case 200:
      send_the_dits_and_dahs("..");
      break; // И
    case 201:
      send_the_dits_and_dahs(".---");
      break; // Й
    case 202:
      send_the_dits_and_dahs("-.-");
      break; // К
    case 203:
      send_the_dits_and_dahs(".-..");
      break; // Л
    case 204:
      send_the_dits_and_dahs("--");
      break; // М
    case 205:
      send_the_dits_and_dahs("-.");
      break; // Н
    case 206:
      send_the_dits_and_dahs("---");
      break; // О
    case 207:
      send_the_dits_and_dahs(".--.");
      break; // П
    case 208:
      send_the_dits_and_dahs(".-.");
      break; // Р
    case 209:
      send_the_dits_and_dahs("...");
      break; // С
    case 210:
      send_the_dits_and_dahs("-");
      break; // Т
    case 211:
      send_the_dits_and_dahs("..-");
      break; // У
    case 212:
      send_the_dits_and_dahs("..-.");
      break; // Ф
    case 213:
      send_the_dits_and_dahs("....");
      break; // Х
    case 214:
      send_the_dits_and_dahs("-.-.");
      break; // Ц
    case 215:
      send_the_dits_and_dahs("---.");
      break; // Ч
    case 216:
      send_the_dits_and_dahs("----");
      break; // Ш
    case 217:
      send_the_dits_and_dahs("--.-");
      break; // Щ
    case 218:
      send_the_dits_and_dahs("--.--");
      break; // Ъ
    case 219:
      send_the_dits_and_dahs("-.--");
      break; // Ы
    case 220:
      send_the_dits_and_dahs("-..-");
      break; // Ь
    case 221:
      send_the_dits_and_dahs("..-..");
      break; // Э
    case 222:
      send_the_dits_and_dahs("..--");
      break; // Ю
    case 223:
      send_the_dits_and_dahs(".-.-");
      break; // Я
    case 255:
      send_the_dits_and_dahs(".-.-");
      break; // я
#endif       // OPTION_RUSSIAN_LANGUAGE_SEND_CLI

    case '\n':
      break;
    case '\r':
      break;

#if defined(OPTION_PROSIGN_SUPPORT)
    case PROSIGN_AA:
      send_the_dits_and_dahs(".-.-");
      break;
    case PROSIGN_AS:
      send_the_dits_and_dahs(".-...");
      break;
    case PROSIGN_BK:
      send_the_dits_and_dahs("-...-.-");
      break;
    case PROSIGN_CL:
      send_the_dits_and_dahs("-.-..-..");
      break;
    case PROSIGN_CT:
      send_the_dits_and_dahs("-.-.-");
      break;
    case PROSIGN_KN:
      send_the_dits_and_dahs("-.--.");
      break;
    case PROSIGN_NJ:
      send_the_dits_and_dahs("-..---");
      break;
    case PROSIGN_SK:
      send_the_dits_and_dahs("...-.-");
      break;
    case PROSIGN_SN:
      send_the_dits_and_dahs("...-.");
      break;
    case PROSIGN_HH:
      send_the_dits_and_dahs("........");
      break; // iz0rus
    case PROSIGN_SOS:
      send_the_dits_and_dahs("...---...");
      break;
    case PROSIGN_SO:
      send_the_dits_and_dahs("...---");
      break;
#if !defined(OPTION_CW_KEYBOARD_GERMAN) && !defined(OPTION_CW_KEYBOARD_ITALIAN) && !defined(OPTION_PS2_NON_ENGLISH_CHAR_LCD_DISPLAY_SUPPORT)
    case PROSIGN_OS:
      send_the_dits_and_dahs("---...");
      break;
#endif // !defined(OPTION_CW_KEYBOARD_GERMAN) ....
#endif

#ifdef OPTION_NON_ENGLISH_EXTENSIONS
    case 192:
      send_the_dits_and_dahs(".--.-");
      break; // 'À'
    case 194:
      send_the_dits_and_dahs(".-.-");
      break; // 'Â'
    case 197:
      send_the_dits_and_dahs(".--.-");
      break; // 'Å'
    case 196:
      send_the_dits_and_dahs(".-.-");
      break; // 'Ä'
    case 198:
      send_the_dits_and_dahs(".-.-");
      break; // 'Æ'
    case 199:
      send_the_dits_and_dahs("-.-..");
      break; // 'Ç'
    case 208:
      send_the_dits_and_dahs("..--.");
      break; // 'Ð'
    case 138:
      send_the_dits_and_dahs("----");
      break; // 'Š'
    case 200:
      send_the_dits_and_dahs(".-..-");
      break; // 'È'
    case 201:
      send_the_dits_and_dahs("..-..");
      break; // 'É'
    case 142:
      send_the_dits_and_dahs("--..-.");
      break; // 'Ž'
    case 209:
      send_the_dits_and_dahs("--.--");
      break; // 'Ñ'
    case 214:
      send_the_dits_and_dahs("---.");
      break; // 'Ö'
    case 216:
      send_the_dits_and_dahs("---.");
      break; // 'Ø'
    case 211:
      send_the_dits_and_dahs("---.");
      break; // 'Ó'
    case 220:
      send_the_dits_and_dahs("..--");
      break; // 'Ü'
    case 223:
      send_the_dits_and_dahs("------");
      break; // 'ß'

    // for English/Japanese font LCD controller which has a few European characters also (HD44780UA00) (LA3ZA code)
    case 225:
      send_the_dits_and_dahs(".-.-");
      break; // 'ä' LA3ZA
    case 239:
      send_the_dits_and_dahs("---.");
      break; // 'ö' LA3ZA
    case 242:
      send_the_dits_and_dahs("---.");
      break; // 'ø' LA3ZA
    case 245:
      send_the_dits_and_dahs("..--");
      break; // 'ü' LA3ZA
    case 246:
      send_the_dits_and_dahs("----");
      break; // almost '' or rather sigma LA3ZA
    case 252:
      send_the_dits_and_dahs(".--.-");
      break; // å (sort of) LA3ZA
    case 238:
      send_the_dits_and_dahs("--.--");
      break; // 'ñ' LA3ZA
    case 226:
      send_the_dits_and_dahs("------");
      break; // 'ß' LA3ZA
#endif       // OPTION_NON_ENGLISH_EXTENSIONS

    case '|':
#if !defined(OPTION_WINKEY_DO_NOT_SEND_7C_BYTE_HALF_SPACE)
      loop_element_lengths(0.5, 0, configuration.wpm);
#endif
      return;
      break;

#if defined(OPTION_DO_NOT_SEND_UNKNOWN_CHAR_QUESTION)
    case '?':
      send_the_dits_and_dahs("..--..");
      break;
#endif

    default:
#if !defined(OPTION_DO_NOT_SEND_UNKNOWN_CHAR_QUESTION)
      send_the_dits_and_dahs("..--..");
#endif
      break;
    }
    if (omit_letterspace != OMIT_LETTERSPACE)
    {

      loop_element_lengths((length_letterspace - 1), 0, configuration.wpm); // this is minus one because send_dit and send_dah have a trailing element space
    }

#ifdef FEATURE_FARNSWORTH
    // Farnsworth Timing : http://www.arrl.org/files/file/Technology/x9004008.pdf
    if (configuration.wpm_farnsworth > configuration.wpm)
    {
      float additional_intercharacter_time_ms = ((((1.0 * farnsworth_timing_calibration) * ((60.0 * float(configuration.wpm_farnsworth)) - (37.2 * float(configuration.wpm))) / (float(configuration.wpm) * float(configuration.wpm_farnsworth))) / 19.0) * 1000.0) - (1200.0 / float(configuration.wpm_farnsworth));
      loop_element_lengths(1, additional_intercharacter_time_ms, 0);
    }
#endif
  }
  else
  {
    if (char_send_mode == HELL)
    {
#ifdef FEATURE_HELL
      transmit_hell_char(cw_char);
#endif
    }
    else
    {
      if (char_send_mode == AMERICAN_MORSE)
      {
#ifdef FEATURE_AMERICAN_MORSE

        /*

          ~  long dah (4 units)

          =  very long dah (5 units)

          &  an extra space (1 unit)

        */

        switch (cw_char)
        { // THIS SECTION IS AMERICAN MORSE CODE - DO NOT TOUCH IT !

        case 'A':
          send_the_dits_and_dahs(".-");
          break;
        case 'B':
          send_the_dits_and_dahs("-...");
          break;
        case 'C':
          send_the_dits_and_dahs("..&.");
          break;
        case 'D':
          send_the_dits_and_dahs("-..");
          break;
        case 'E':
          send_the_dits_and_dahs(".");
          break;
        case 'F':
          send_the_dits_and_dahs(".-.");
          break;
        case 'G':
          send_the_dits_and_dahs("--.");
          break;
        case 'H':
          send_the_dits_and_dahs("....");
          break;
        case 'I':
          send_the_dits_and_dahs("..");
          break;
        case 'J':
          send_the_dits_and_dahs("-.-.");
          break;
        case 'K':
          send_the_dits_and_dahs("-.-");
          break;
        case 'L':
          send_the_dits_and_dahs("~");
          break;
        case 'M':
          send_the_dits_and_dahs("--");
          break;
        case 'N':
          send_the_dits_and_dahs("-.");
          break;
        case 'O':
          send_the_dits_and_dahs(".&.");
          break;
        case 'P':
          send_the_dits_and_dahs(".....");
          break;
        case 'Q':
          send_the_dits_and_dahs("..-.");
          break;
        case 'R':
          send_the_dits_and_dahs(".&..");
          break;
        case 'S':
          send_the_dits_and_dahs("...");
          break;
        case 'T':
          send_the_dits_and_dahs("-");
          break;
        case 'U':
          send_the_dits_and_dahs("..-");
          break;
        case 'V':
          send_the_dits_and_dahs("...-");
          break;
        case 'W':
          send_the_dits_and_dahs(".--");
          break;
        case 'X':
          send_the_dits_and_dahs(".-..");
          break;
        case 'Y':
          send_the_dits_and_dahs("..&..");
          break;
        case 'Z':
          send_the_dits_and_dahs("...&.");
          break;

          // THIS SECTION IS AMERICAN MORSE CODE - DO NOT TOUCH IT !

        case '&':
          send_the_dits_and_dahs(".&...");
          break;

        case '0':
          send_the_dits_and_dahs("=");
          break;
        case '1':
          send_the_dits_and_dahs(".---.");
          break;
        case '2':
          send_the_dits_and_dahs("..--..");
          break;
        case '3':
          send_the_dits_and_dahs("...-.");
          break;
        case '4':
          send_the_dits_and_dahs("....-");
          break;
        case '5':
          send_the_dits_and_dahs("---");
          break;
        case '6':
          send_the_dits_and_dahs("......");
          break;
        case '7':
          send_the_dits_and_dahs("--..");
          break;
        case '8':
          send_the_dits_and_dahs("-....");
          break;
        case '9':
          send_the_dits_and_dahs("-..-");
          break;

          // THIS SECTION IS AMERICAN MORSE CODE - DO NOT TOUCH IT !

        case ',':
          send_the_dits_and_dahs(".-.-");
          break;
        case '.':
          send_the_dits_and_dahs("..--..");
          break;
        case '?':
          send_the_dits_and_dahs("-..-.");
          break;
        case '!':
          send_the_dits_and_dahs("---.");
          break;
        case ':':
          send_the_dits_and_dahs("-.-&.&.");
          break;
        case ';':
          send_the_dits_and_dahs("...&..");
          break;
        case '-':
          send_the_dits_and_dahs("....&.-..");
          break;

        } // switch (cw_char)

#endif
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------------

void speed_change(int change)
{
  if (((configuration.wpm + change) > wpm_limit_low) && ((configuration.wpm + change) < wpm_limit_high))
  {
    speed_set(configuration.wpm + change);
  }

#ifdef FEATURE_DISPLAY
  lcd_center_print_timed_wpm();
#endif
}

//-------------------------------------------------------------------------------------------------------
