#include <Arduino.h>

#define tapper_amount 2
#define time_off 200
#define time_on 20

static uint8_t tapper_input[tapper_amount] = {2,3};
static uint8_t tapper_output[tapper_amount] = {14,16};

void setup() {
  for(int x = 0; x< tapper_amount; x++ ){
      pinMode(tapper_input[x], INPUT_PULLUP);
      pinMode(tapper_output[x], OUTPUT);
  }
}

void loop()
{
    unsigned long opening_times[tapper_amount] = {millis(),millis()};
    size_t pin_state[tapper_amount] = {0,0};      
    for(;;) {
      for(size_t x = 0; x < tapper_amount; x++) {
        int signal = digitalRead(tapper_input[x]);
        if(signal == 0) {
          pin_state[x] = 0;
        }
        else if ((signal == 1) && (pin_state[x] == 0)) {          
          pin_state[x] = 1;        
          if(opening_times[x] + time_off + time_on <= millis()) {        
            digitalWrite(tapper_output[x], HIGH);
            opening_times[x] = millis();
          }
        } 
        if (millis() >= (opening_times[x] + time_on)) {
          digitalWrite(tapper_output[x], LOW);
        }
      }
    }
}
