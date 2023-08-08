//
// Created by Steve Hopkins on 7/25/23.
//

#include <stdio.h>
#include "dht.h"

uint dht_initialise(PIO pio, int gpio) {
    uint offset = pio_add_program(pio, &dht_program);
    uint sm = pio_claim_unused_sm(pio, true);

    pio_gpio_init(pio, gpio);

    pio_sm_config c = dht_program_get_default_config(offset);
    sm_config_set_clkdiv_int_frac(&c, 255, 0);
    sm_config_set_set_pins(&c, gpio, 1);
    sm_config_set_in_pins(&c, gpio);
    sm_config_set_in_shift(&c, false, true, 32);
    pio_sm_init(pio0, sm, offset, &c);
    pio_sm_set_enabled(pio0, sm, true);

    return sm;
}

void dht_read(PIO pio, uint sm, dhtData *reading) {
    pio_sm_put_blocking(pio, sm, 500);

    uint32_t data = pio_sm_get_blocking(pio0, sm);
    uint8_t byte1 = (data >> 24 & 0xFF);
    uint8_t byte2 = (data >> 16 & 0xFF);
    uint8_t byte3 = (data >> 8 & 0xFF);
    uint8_t byte4 = (data & 0xFF);

    uint8_t checksum = pio_sm_get_blocking(pio, sm) & 0xFF;
    reading->error = (checksum != ((byte1 + byte2 + byte3 + byte4) & 0xFF));
    if (reading->error == true) {
        printf("Error reading data!\n");
        return;
    }

    reading->humidity = (float) (byte1 << 8 | byte2) / 10.0f;

    int neg = byte3 & 0x80;
    byte3 = byte3 & 0x7F;
    reading->temperature = (float) (byte3 << 8 | byte4) / 10.0f;
    if (neg > 0)
        reading->temperature = -reading->temperature;
}