
void ptt_key()
{

  unsigned long ptt_activation_time = millis();
  byte all_delays_satisfied = 0;

#ifdef FEATURE_SEQUENCER
  byte sequencer_1_ok = 0;
  byte sequencer_2_ok = 0;
  byte sequencer_3_ok = 0;
  byte sequencer_4_ok = 0;
  byte sequencer_5_ok = 0;
#endif

  if (configuration.ptt_disabled)
  {
    return;
  }

  if (ptt_line_activated == 0)
  { // if PTT is currently deactivated, bring it up and insert PTT lead time delay
#ifdef FEATURE_SO2R_BASE
    if (current_tx_ptt_line && (configuration.ptt_buffer_hold_active || manual_ptt_invoke_ptt_input_pin))
    {

#if defined(FEATURE_WINKEY_EMULATION) && defined(OPTION_WINKEY_PINCONFIG_PTT_CONTROLS_PTT_LINE)
      if (winkey_pinconfig_ptt_bit)
      {
        digitalWrite(configuration.current_ptt_line, ptt_line_active_state);
      }
#else
      digitalWrite(configuration.current_ptt_line, ptt_line_active_state);
#endif // defined(FEATURE_WINKEY_EMULATION) && !defined(OPTION_WINKEY_PINCONFIG_PTT_CONTROLS_PTT_HOLD)

// digitalWrite (current_tx_ptt_line, ptt_line_active_state);

// delay(configuration.ptt_lead_time[configuration.current_tx-1]);
#ifdef FEATURE_SEQUENCER
      sequencer_ptt_inactive_time = 0;
#endif
    }
#else
    if (configuration.current_ptt_line)
    {

#if defined(FEATURE_WINKEY_EMULATION) && defined(OPTION_WINKEY_PINCONFIG_PTT_CONTROLS_PTT_LINE)
      if (winkey_pinconfig_ptt_bit)
      {
        digitalWrite(configuration.current_ptt_line, ptt_line_active_state);
      }
#else
      digitalWrite(configuration.current_ptt_line, ptt_line_active_state);
#endif // defined(FEATURE_WINKEY_EMULATION) && !defined(OPTION_WINKEY_PINCONFIG_PTT_CONTROLS_PTT_HOLD)

      // digitalWrite (configuration.current_ptt_line, ptt_line_active_state);

#if defined(OPTION_WINKEY_2_SUPPORT) && defined(FEATURE_WINKEY_EMULATION)
      if ((wk2_both_tx_activated) && (ptt_tx_2))
      {
        digitalWrite(ptt_tx_2, ptt_line_active_state);
      }
#endif
      // delay(configuration.ptt_lead_time[configuration.current_tx-1]);
#ifdef FEATURE_SEQUENCER
      sequencer_ptt_inactive_time = 0;
#endif
    }
#endif // FEATURE_SO2R_BASE

#ifdef FEATURE_MIDI
    midi_key_ptt(1);
#endif

    ptt_line_activated = 1;

#ifdef FEATURE_SO2R_BASE
    so2r_set_rx();
#endif

    while (!all_delays_satisfied)
    {
#ifdef FEATURE_SEQUENCER
      if (sequencer_1_pin)
      {
        if (((millis() - ptt_activation_time) >= configuration.ptt_active_to_sequencer_active_time[0]) || sequencer_1_active)
        {
          digitalWrite(sequencer_1_pin, sequencer_pins_active_state);
          sequencer_1_ok = 1;
          sequencer_1_active = 1;
        }
      }
      else
      {
        sequencer_1_ok = 1;
      }
      if (sequencer_2_pin)
      {
        if (((millis() - ptt_activation_time) >= configuration.ptt_active_to_sequencer_active_time[1]) || sequencer_2_active)
        {
          digitalWrite(sequencer_2_pin, sequencer_pins_active_state);
          sequencer_2_ok = 1;
          sequencer_2_active = 1;
        }
      }
      else
      {
        sequencer_2_ok = 1;
      }
      if (sequencer_3_pin)
      {
        if (((millis() - ptt_activation_time) >= configuration.ptt_active_to_sequencer_active_time[2]) || sequencer_3_active)
        {
          digitalWrite(sequencer_3_pin, sequencer_pins_active_state);
          sequencer_3_ok = 1;
          sequencer_3_active = 1;
        }
      }
      else
      {
        sequencer_3_ok = 1;
      }
      if (sequencer_4_pin)
      {
        if (((millis() - ptt_activation_time) >= configuration.ptt_active_to_sequencer_active_time[3]) || sequencer_4_active)
        {
          digitalWrite(sequencer_4_pin, sequencer_pins_active_state);
          sequencer_4_ok = 1;
          sequencer_4_active = 1;
        }
      }
      else
      {
        sequencer_4_ok = 1;
      }
      if (sequencer_5_pin)
      {
        if (((millis() - ptt_activation_time) >= configuration.ptt_active_to_sequencer_active_time[4]) || sequencer_5_active)
        {
          digitalWrite(sequencer_5_pin, sequencer_pins_active_state);
          sequencer_5_ok = 1;
          sequencer_5_active = 1;
        }
      }
      else
      {
        sequencer_5_ok = 1;
      }

      if (((millis() - ptt_activation_time) >= configuration.ptt_lead_time[configuration.current_tx - 1]) && sequencer_1_ok && sequencer_2_ok && sequencer_3_ok && sequencer_4_ok && sequencer_5_ok)
      {
        all_delays_satisfied = 1;
      }

#else // FEATURE_SEQUENCER
#if defined(FEATURE_WINKEY_EMULATION) && !defined(OPTION_WINKEY_PINCONFIG_PTT_CONTROLS_PTT_HOLD)
      if (((millis() - ptt_activation_time) >= configuration.ptt_lead_time[configuration.current_tx - 1]) || (winkey_host_open && !winkey_pinconfig_ptt_bit))
      {
        all_delays_satisfied = 1;
      }
#else
      if ((millis() - ptt_activation_time) >= configuration.ptt_lead_time[configuration.current_tx - 1])
      {
        all_delays_satisfied = 1;
      }
#endif
#endif // FEATURE_SEQUENCER

    } // while (!all_delays_satisfied)
  }
  ptt_time = millis();
}

//-------------------------------------------------------------------------------------------------------

void ptt_unkey()
{

  if (ptt_line_activated)
  {

#ifdef FEATURE_SO2R_BASE
    if (current_tx_ptt_line)
    {
      digitalWrite(current_tx_ptt_line, ptt_line_inactive_state);
#else
    if (configuration.current_ptt_line)
    {
      digitalWrite(configuration.current_ptt_line, ptt_line_inactive_state);
#if defined(OPTION_WINKEY_2_SUPPORT) && defined(FEATURE_WINKEY_EMULATION)
      if ((wk2_both_tx_activated) && (ptt_tx_2))
      {
        digitalWrite(ptt_tx_2, ptt_line_inactive_state);
      }
#endif
#endif // FEATURE_SO2R_BASE
    }

#ifdef FEATURE_MIDI
    midi_key_ptt(0);
#endif

    ptt_line_activated = 0;
#ifdef FEATURE_SEQUENCER
    sequencer_ptt_inactive_time = millis();
#endif

#ifdef FEATURE_SO2R_BASE
    if (so2r_pending_tx)
    {
      so2r_tx = so2r_pending_tx;
      so2r_pending_tx = 0;
      so2r_set_tx();
    }

    so2r_set_rx();
#endif // FEATURE_SO2R_BASE
  }
}

//-------------------------------------------------------------------------------------------------------
