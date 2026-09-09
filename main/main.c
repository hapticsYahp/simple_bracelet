/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

static const char *TAG = "example";

#include "bracelet.h"
//#include "poma_tcpconnector.h"
#include "poma_bleconnector.h"

#define PORT CONFIG_EXAMPLE_PORT




int GlobalVar = 0;
Topic *topicHead;

void setterGlobalVar(WRITERFUNC, char *argument)
{
    if (argument != NULL)
        GlobalVar = atoi(argument);
    writer("done", strlen("done"));
}

void getterGlobalVar(WRITERFUNC, char *argument)
{
    char response[10];
    sprintf(response, "%d", GlobalVar);
    writer(response, strlen(response));
}

/*
static void tcp_server_task(void *pvParameters)
{

    PoMA_TCP_SPEC *tcp_spec = (PoMA_TCP_SPEC *)pvParameters;

    tcp_spec->processClientsLoop(tcp_spec, topicHead);

}
*/

static void ble_server_task(void *pvParameters)
{
    PoMA_BLE_SPEC *ble_spec = (PoMA_BLE_SPEC *)pvParameters;
    printf("before bleSpec->processClientsLoop");
    ble_spec->processClientsLoop(ble_spec, topicHead);
    vTaskDelete(NULL); // properly terminate the task
}

void app_main(void)
{

    initializeHaptic(&bracelet); 
    
    topicHead = createTopic("GlobalVar", getterGlobalVar, setterGlobalVar);
    addTopic(topicHead, createTopic("intensity", getIntensity, setIntensity));
    addTopic(topicHead, createTopic("motor_count", getMotorCount, setMotorCount));
    addTopic(topicHead, createTopic("motor_config", getMotorConfig, setMotorConfig));
    addTopic(topicHead, createTopic("enabled_motors", getEnabledMotors, setEnabledMotors));

    PoMA_BLE_SPEC *bleSpec = malloc(sizeof(PoMA_BLE_SPEC));
    bleSpec = createPoMABLEConnectSpec(bleSpec, 1, SINGLE_USER);

    xTaskCreate(ble_server_task, "ble_server", 4096 * 2, (void *)bleSpec, 5, NULL);

}
