#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>


#define tapper_amount 2
#define ledpin 15
static uint8_t tapper_input[tapper_amount] = {2,3};
static uint8_t tapper_output[tapper_amount] = {14,16};
SemaphoreHandle_t interruptSemaphore[tapper_amount];
SemaphoreHandle_t sem_led;



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
  static int buttonstate[tapper_amount] = {1,1};  
  for(;;) {
    for(int x = 0; x< tapper_amount; x++) {
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
        digitalWrite(ledpin, LOW);
        vTaskDelay(100/ portTICK_PERIOD_MS);
        digitalWrite(ledpin, HIGH);
        vTaskDelay(100 /portTICK_PERIOD_MS);      
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  pinMode(ledpin, OUTPUT);
  for(int x = 0; x< tapper_amount; x++ ){
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
