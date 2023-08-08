//
// Created by steve on 7/25/23.
//

#ifndef DHT_OPENHAB_DHT_H
#define DHT_OPENHAB_DHT_H

#include <hardware/pio.h>
#include "dht.pio.h"

typedef struct {
    float temperature;
    float humidity;
    bool error;
} dhtData;

uint dht_initialise(PIO pio, int gpio);
void dht_read(PIO pio, uint sm, dhtData *reading);

#endif //DHT_OPENHAB_DHT_H
