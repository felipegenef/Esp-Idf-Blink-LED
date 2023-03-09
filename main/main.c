
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"

#include "esp_log.h"

#define LED_BUILD 2
#define BUTTON GPIO_NUM_0

QueueHandle_t XQueue_comunication;

static const char * TAG = "MAIN-queue";

void read_button()
{

    int counter = 0;
    bool status = 0; // 0 está apertado, 1 está solto
    //setup do hardware
    gpio_pad_select_gpio(BUTTON);
    // 1)Pino
    // 2) Out ou in (GPIO_MODE_OUTPUT,GPIO_MODE_INPUT)
    gpio_set_direction(BUTTON,GPIO_MODE_INPUT);
    // Setar como pullup (botão de default 1) (pulldown é de default 0)
    gpio_set_pull_mode(BUTTON,GPIO_PULLUP_ONLY);
    while(true)
    {
        //Quando botão estiver apertado
        if(gpio_get_level(BUTTON)==0&&status==0)
        {
            vTaskDelay(100/ portTICK_PERIOD_MS);
              if(gpio_get_level(BUTTON)==0&&status==0)
              { //Verifica se ainda está apertado apos 100ms
              
                if(xQueueSend(XQueue_comunication, &counter, portMAX_DELAY)==pdPASS)
                {
                        counter++;
                        ESP_LOGI(TAG,"Botão foi apertado");
                    
                }
                status=1;
              }
        }

        //Quando botão estiver solto
        if(gpio_get_level(BUTTON)==1&&status==1)
        {
            vTaskDelay(100/ portTICK_PERIOD_MS);
              if(gpio_get_level(BUTTON)==1&&status==1)
              { //Verifica se ainda está solto apos 100ms
              
                status=0;
              }
        }
        // Verifica a cada 10ms se os botão esta apertado ou solto
        vTaskDelay(10/ portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);

}


void print_events()
{
    int counter_recive;



    while(true)
    {
        xQueueReceive(XQueue_comunication, &counter_recive, portMAX_DELAY);
       
        ESP_LOGI(TAG, "O botão foi apertado %i vezes \n", counter_recive);
        vTaskDelay(1000/ portTICK_PERIOD_MS);
    }  
    vTaskDelete(NULL);  
}
void blink_led()
{
        //setup do hardware
    gpio_pad_select_gpio(LED_BUILD);
    // 1)Pino
    // 2) Out ou in (GPIO_MODE_OUTPUT,GPIO_MODE_INPUT)
    gpio_set_direction(LED_BUILD,GPIO_MODE_OUTPUT);
    while(true){

        // Ligado é 3.3V (1) e desligado é 0V (0)
        //1)Numero do Pino
        //2) Level
        //Liga led se counter for impar
        gpio_set_level(LED_BUILD,1 );
        // delay
        vTaskDelay(1000/ portTICK_PERIOD_MS);
        gpio_set_level(LED_BUILD,0);
    }
   vTaskDelete(NULL); 
}

void app_main(void)
{
    if( (XQueue_comunication = xQueueCreate(10, sizeof(uint32_t))) == NULL)
    {
        ESP_LOGI(TAG, "Aconteceu algum problema no alocamento da queue comunication");
        return;
    }

    if ( (xTaskCreate(read_button, "read_button", 2048, NULL, 5, NULL)) != pdTRUE )
    {
        ESP_LOGI(TAG, "Não foi posivel alocar a taskread_button");
        return;
    }

    if ( (xTaskCreate(print_events, "print_events", 2048, NULL, 5, NULL)) != pdTRUE )
    {
        ESP_LOGI(TAG, "Não foi posivel alocar a task print_events");
        return;
    }   

    if ( (xTaskCreate(blink_led, "blink_led", 2048, NULL, 5, NULL)) != pdTRUE )
    {
        ESP_LOGI(TAG, "Não foi posivel alocar a task blink_led");
        return;
    }   

}
