#include <stdio.h>
#include <math.h>
#include <pico/cyw43_arch.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "dht.h"
#include "wifi_config.h"

int main() {
    stdio_init_all();

    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK)) {
        printf("failed to initialise\n");
        return 1;
    }
    printf("initialised\n");
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("failed to connect\n");
        return 1;
    }
    printf("connected\n");

    //
    uint sm = dht_initialise(pio0, 28);
    dhtData reading;

    while (true) {
        dht_read(pio0, sm, &reading);

        printf("Humidity:    %.1f%%\n", roundf(reading.humidity * 100) / 100);
        printf("Temperature: %.1fC\n\n", roundf(reading.temperature * 100) / 100);

        // Remember: DHT22/AM2302 can only be read once every 2 seconds.
        sleep_ms(10000);
    }

    return 0;
}
