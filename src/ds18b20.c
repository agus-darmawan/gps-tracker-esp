#include "ds18b20.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "onewire_bus.h"
#include "ds18b20.h"

static const char *TAG = "DS18B20";

// DS18B20 Configuration
#define DS18B20_GPIO        GPIO_NUM_4
#define MAX_DEVICES         1
#define TEMPERATURE_RESOLUTION  DS18B20_RESOLUTION_12B

static ds18b20_data_t current_temp_data = {0};
static onewire_bus_handle_t bus = NULL;
static onewire_device_iter_handle_t iter = NULL;
static onewire_device_t device;
static int num_devices = 0;

/**
 * DS18B20 reading task
 */
void ds18b20_task(void *pvParameters) {
    ESP_LOGI(TAG, "DS18B20 task started");
    
    while (1) {
        if (num_devices > 0) {
            // Trigger temperature conversion
            ds18b20_trigger_temperature_conversion(&device, 1);
            
            // Wait for conversion to complete (750ms for 12-bit resolution)
            vTaskDelay(pdMS_TO_TICKS(800));
            
            // Read temperature
            float temperature;
            esp_err_t err = ds18b20_get_temperature(&device, &temperature);
            
            if (err == ESP_OK) {
                current_temp_data.temperature = temperature;
                current_temp_data.valid = true;
                ESP_LOGI(TAG, "Temperature: %.2f°C", temperature);
            } else {
                current_temp_data.valid = false;
                ESP_LOGW(TAG, "Failed to read temperature");
            }
        } else {
            ESP_LOGW(TAG, "No DS18B20 device found");
            current_temp_data.valid = false;
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000)); // Read every 2 seconds
    }
}

/**
 * Initialize DS18B20 sensor
 */
void ds18b20_init(void) {
    // Install 1-wire bus
    onewire_bus_config_t bus_config = {
        .bus_gpio_num = DS18B20_GPIO,
    };
    
    onewire_bus_rmt_config_t rmt_config = {
        .max_rx_bytes = 10,
    };
    
    ESP_ERROR_CHECK(onewire_new_bus_rmt(&bus_config, &rmt_config, &bus));
    ESP_LOGI(TAG, "1-Wire bus installed on GPIO%d", DS18B20_GPIO);
    
    // Search for DS18B20 devices
    ESP_ERROR_CHECK(onewire_new_device_iter(bus, &iter));
    
    ESP_LOGI(TAG, "Searching for DS18B20 devices...");
    do {
        esp_err_t err = onewire_device_iter_get_next(iter, &device);
        if (err == ESP_OK) {
            num_devices++;
            ESP_LOGI(TAG, "Found DS18B20[%d], address: %016llX", num_devices - 1, device.address);
            
            // Set resolution
            ds18b20_set_resolution(&device, TEMPERATURE_RESOLUTION);
        } else if (err == ESP_ERR_NOT_FOUND) {
            ESP_LOGI(TAG, "Total DS18B20 devices found: %d", num_devices);
            break;
        } else {
            ESP_LOGE(TAG, "Error searching for devices: %s", esp_err_to_name(err));
            break;
        }
    } while (num_devices < MAX_DEVICES);
    
    if (num_devices == 0) {
        ESP_LOGW(TAG, "No DS18B20 device found!");
    }
}

/**
 * Get current temperature data
 */
ds18b20_data_t ds18b20_get_data(void) {
    return current_temp_data;
}

/**
 * Check if temperature data is valid
 */
bool ds18b20_is_valid(void) {
    return current_temp_data.valid;
}