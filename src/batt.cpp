#include <Arduino.h>

#define BATT_HISTORY_COUNT 3
#define BATT_SENSE_PIN 36

static int32_t batt_voltage_array[BATT_HISTORY_COUNT];
static int32_t batt_voltage_index = 0;
static bool initialized = false;

static void init_empty_slots(int32_t voltage) {
  for(int i = 0; i < BATT_HISTORY_COUNT; i++) {
    batt_voltage_array[i] = voltage;
  }
}

static void append_to_history(int32_t voltage) {
    batt_voltage_array[batt_voltage_index] = voltage;
    batt_voltage_index++;
    if(batt_voltage_index >= BATT_HISTORY_COUNT) {
        batt_voltage_index = 0;
    }
    if(!initialized) {
      initialized = true;
      init_empty_slots(voltage);
    }
}

void sample_batt_level() {
    int adcResult = analogRead(BATT_SENSE_PIN); //read pin A0 value
    //Serial.printf("ADC: %d\n", adcResult);
    float battVoltage = adcResult * (3.3 / 4095.0)*2;
    //Serial.printf("battVoltage: %f\n", battVoltage);
    append_to_history((int32_t)(battVoltage*1000));
}

int32_t get_avg_batt_voltage() {
  int32_t res = 0;
  for(int i = 0; i < BATT_HISTORY_COUNT; i++) {
    res += batt_voltage_array[i];
  }
  return res/BATT_HISTORY_COUNT;
}

