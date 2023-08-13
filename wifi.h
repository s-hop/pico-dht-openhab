//
// Created by Steve Hopkins on 8/13/23.
//

#include <stdint.h>

#ifndef PICO_DHT_OPENHAB_WIFI_H
#define PICO_DHT_OPENHAB_WIFI_H

int wifi_connect(char ssid[], char password[], uint32_t country, uint32_t auth);

#endif //PICO_DHT_OPENHAB_WIFI_H
