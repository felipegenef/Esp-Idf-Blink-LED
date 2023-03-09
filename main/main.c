
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"

#include "esp_log.h"

#define LED_BUILD 2

QueueHandle_t XQueue_comunication;

static const char * TAG = "MAIN-queue";

void read_button()
{
    int counter = 0;
    while(true)
    {
        counter++;
        xQueueSend(XQueue_comunication, &counter, portMAX_DELAY);
        ESP_LOGI(TAG, "O valor %i foi enviado na queue \n", counter);
        vTaskDelay(1000/ portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);

}


void print_events()
{
    int counter_recive;



    while(true)
    {
        xQueueReceive(XQueue_comunication, &counter_recive, portMAX_DELAY);
       
        ESP_LOGI(TAG, "O valor %i foi recebido na queue \n", counter_recive);
        vTaskDelay(2000/ portTICK_PERIOD_MS);
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

    if ( (xTaskCreate(producer, "producer", 2048, NULL, 5, NULL)) != pdTRUE )
    {
        ESP_LOGI(TAG, "Não foi posivel alocar a task producer");
        return;
    }

    if ( (xTaskCreate(consumer, "consumer", 2048, NULL, 5, NULL)) != pdTRUE )
    {
        ESP_LOGI(TAG, "Não foi posivel alocar a task consumer");
        return;
    }    

}
