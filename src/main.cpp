#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

SemaphoreHandle_t interruptSemaphore[2];

int tapperout[2] = {16, 14};

int tapper0in = 2; //int1
int tapper1in = 3; //int0

void taptask(void * pvParameters) {
  int tapper = (int)pvParameters;  
  for (;;) {
    if (xSemaphoreTake(interruptSemaphore[tapper], portMAX_DELAY) == pdPASS) {      
      digitalWrite(tapperout[tapper], HIGH);
      Serial.println("open");
      Serial.println(tapper);
      vTaskDelay(20 / portTICK_PERIOD_MS);  
      digitalWrite(tapperout[tapper], LOW);
      vTaskDelay(500 / portTICK_PERIOD_MS); 
      Serial.println("cool");
      Serial.println(tapper);      
    }
    vTaskDelay(10);
  }
}

void tap1interrupt() {  
  xSemaphoreGiveFromISR(interruptSemaphore[1], NULL);
}

void tap0interrupt() {
  xSemaphoreGiveFromISR(interruptSemaphore[0], NULL);
}

void setup()
{
  for(int x =0; x< 2; x++) {
    pinMode(tapperout[x], OUTPUT);     
    xTaskCreate(taptask, "tap", 128, (void*)x, 0, NULL);
    interruptSemaphore[x] = xSemaphoreCreateBinary();
  }
  
  pinMode(tapper0in, INPUT_PULLUP);
  pinMode(tapper1in, INPUT_PULLUP);

  Serial.begin(9600); 

  attachInterrupt(digitalPinToInterrupt(tapper0in), tap0interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(tapper1in), tap1interrupt, RISING);  
}

void loop()
{    
}
