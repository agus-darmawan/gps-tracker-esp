#ifndef DS18B20_H
#define DS18B20_H

#include <stdbool.h>

// DS18B20 Data Structure
typedef struct {
    float temperature;
    bool valid;
} ds18b20_data_t;

// Function declarations
void ds18b20_init(void);
void ds18b20_task(void *pvParameters);
ds18b20_data_t ds18b20_get_data(void);
bool ds18b20_is_valid(void);

#endif // DS18B20_H