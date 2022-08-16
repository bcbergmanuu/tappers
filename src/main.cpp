#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

SemaphoreHandle_t interruptSemaphore[8];
SemaphoreHandle_t sem_led;

static uint8_t tapper_input[8] = {6,7,8,9,10,11,12,13};
static uint8_t tapper_output[8] = {24,28,32,36,40,44,48,52};

void taptask(void * pvParameters) {
  int tapper = (int)pvParameters;  
  for (;;) {
    if (xSemaphoreTake(interruptSemaphore[tapper], portMAX_DELAY) == pdPASS) {      
      digitalWrite(tapper_output[tapper], HIGH);

      vTaskDelay(20 / portTICK_PERIOD_MS);  

      digitalWrite(tapper_output[tapper], LOW);      
      vTaskDelay(500 / portTICK_PERIOD_MS);                 
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void checkTask(void * pvParameters) {
  static int buttonstate[8] = {1,1,1,1,1,1,1,1};  
  for(;;) {
    for(int x = 0; x< 8; x++) {
      int signal = digitalRead(tapper_input[x]);
      if(signal == 1) {
        buttonstate[x] = 1;
      }
      else if ((signal == 0) && (buttonstate[x] == 1)) {        
        xSemaphoreGive(interruptSemaphore[x]);
        xSemaphoreGive(sem_led);
        buttonstate[x] = 0;        
      }      
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void ledIndicationTask(void * pvParameters) {   
  for(;;)  {
    if(xSemaphoreTake(sem_led, portMAX_DELAY) == pdPASS) {      
        digitalWrite(3, LOW);
        vTaskDelay(100/ portTICK_PERIOD_MS);
        digitalWrite(3, HIGH);
        vTaskDelay(100 /portTICK_PERIOD_MS);      
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  pinMode(3, OUTPUT);
  for(int x = 0; x< 8; x++ ){
    pinMode(tapper_input[x], INPUT_PULLUP);
    pinMode(tapper_output[x], OUTPUT);
    
    xTaskCreate(taptask, "tap", 128, (void*)x, 0, NULL);
    interruptSemaphore[x] = xSemaphoreCreateBinary();
  }
  sem_led = xSemaphoreCreateBinary();


   xTaskCreate(checkTask, "check", 128, 0, 0, NULL);
   xTaskCreate(ledIndicationTask, "led", 128, 0, 0, NULL);

}

void loop()
{    
  
}
