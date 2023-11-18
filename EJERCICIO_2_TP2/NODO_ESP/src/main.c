#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "mqtt_client.h"
#include <esp_log.h>
#include "driver/gpio.h"
#include <math.h>

#define TAG "[MQTT]"
#define min(a, b) (a > b ? b : a)
static bool mqttOk = false;

#define id 3

#define ledPin GPIO_NUM_2
#define sensorPin GPIO_NUM_34

void hardware_init(void)
{
    gpio_config_t gpioLed =
        {
            .pin_bit_mask = 1ULL << ledPin,
            .mode = GPIO_MODE_DEF_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE};
    ESP_ERROR_CHECK(gpio_config(&gpioLed));
    ESP_ERROR_CHECK(gpio_set_level(ledPin, 0));

    //adc1_config_width(ADC_WIDTH_BIT_12);
    //adc1_config_channel_atten(sensorPin, ADC_ATTEN_DB_11);
}

static esp_mqtt_client_handle_t actClient;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    char topic[64];
    char data[64];
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
    actClient = client;
    msg_id = esp_mqtt_client_subscribe(client, "/ej02/cmd", 1);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

    char idTopicCmd[20];
    snprintf(idTopicCmd, sizeof(idTopicCmd), "/ej02/%d/cmd", id);
    msg_id = esp_mqtt_client_subscribe(client, idTopicCmd, 1);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

    char idTopicSensor[20];
    snprintf(idTopicSensor, sizeof(idTopicSensor), "/ej02/%d/sensor", id);
    msg_id = esp_mqtt_client_subscribe(client, idTopicSensor, 1);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

    mqttOk = true;
    break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        mqttOk = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        strncpy(topic, event->topic, min(63, event->topic_len));
        strncpy(data, event->data, min(63, event->data_len));
        printf("TOPIC=%s\r\n", topic);
        printf("DATA=%s\r\n", data);

        if (strcmp(topic, "/ej02/cmd") == 0)
        {
            if (strcmp(data, "getId") == 0)
            {
                char idTopic[20];
                snprintf(idTopic, sizeof(idTopic), "/ej02/%d/id", id);
                msg_id = esp_mqtt_client_publish(client, idTopic, "3", 0, 1, 0);
                ESP_LOGI(TAG, "Published ID, msg_id=%d", msg_id);
            }
        }
        else if (strstr(topic, "/ej02/") != NULL && strstr(topic, "/sensor") != NULL)
        {
            // Process sensor data for the corresponding node
            // Parse node ID from the topic
            char *token = strtok(topic, "/");
            token = strtok(NULL, "/");
            int nodeId = atoi(token);

            // TODO: Process sensor data for nodeId using 'data'
        }
        else if (strstr(topic, "/ej02/") != NULL && strstr(topic, "/cmd") != NULL)
        {
            // Process command for the corresponding node
            // Parse node ID from the topic
            char *token = strtok(topic, "/");
            token = strtok(NULL, "/");
            int nodeId = atoi(token);

            if (strcmp(data, "LedOn") == 0 || strcmp(data, "ledOn") == 0)
            {
                ESP_LOGI(TAG, "LED ON");
                ESP_ERROR_CHECK(gpio_set_level(ledPin, 1));
            }
            else if (strcmp(data, "LedOff") == 0)
            {
                ESP_LOGI(TAG, "LED OFF");
                ESP_ERROR_CHECK(gpio_set_level(ledPin, 0));
            }
        }
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        ESP_LOGE(TAG, "Error type: %d", event->error_handle->error_type);
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            ESP_LOGE(TAG, "Last errno string: %s", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void taskPublish(void *param)
{
    char toSend[128];
    float cont = 1;

    while (1)
    {
        vTaskDelay(2500 / portTICK_PERIOD_MS);
        if (mqttOk)
        {
            if (cont == 2)
            {
                ESP_ERROR_CHECK(gpio_set_level(ledPin, 1));
            }
            float senoidal = sin(cont);
            sprintf(toSend, "%f", senoidal);

            char idTopic[20];
            snprintf(idTopic, sizeof(idTopic), "/ej02/%d/sensor", id);
            esp_mqtt_client_publish(actClient, idTopic, toSend, 0, 2, 0);
            cont = cont + (0.2);
        }
    }
}

void app_main(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://192.168.0.12",
        .credentials.username = "esp32",
        .credentials.authentication.password = "esp32esp32",
    };

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_flash_init();
    }

    hardware_init();
    wifi_init_sta();

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
    xTaskCreate(taskPublish, "Publish Task", configMINIMAL_STACK_SIZE + 4096, NULL, 1, NULL);
}
