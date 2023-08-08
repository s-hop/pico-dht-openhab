#include <stdio.h>
#include <math.h>
#include <pico/cyw43_arch.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "dht.h"
#include "wifi_config.h"

int wifi_connect(char ssid[], char password[], uint32_t country, uint32_t auth) {
    if (cyw43_arch_init_with_country(country)) {
        printf("WiFi failed to initialise\n");
        return 1;
    }
    printf("WiFi initialised\n");

    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, auth, 10000)) {
        printf("WiFi failed to connect\n");
        return 2;
    }
    printf("WiFi connected\n");

    return 0;
}

int main() {
    stdio_init_all();

    /*
     * Modify these variables as required.
     * For this example, Wi-Fi SSID and password are declared in a wifi_config.h header file for privacy, but can be
     * declared directly here.
     *
     * Country codes are declared here:         pico-sdk/lib/cyw43-driver/src/cyw43_country.h
     * Authentication modes are declared here:  pico-sdk/lib/cyw43-driver/src/cyw43_ll.h
    */
    char ssid[] = WIFI_SSID;
    char password[] = WIFI_PASS;
    uint32_t country = CYW43_COUNTRY_UK;
    uint32_t auth = CYW43_AUTH_WPA2_MIXED_PSK;
    wifi_connect(ssid, password, country, auth);

    uint sm = dht_initialise(pio0, 28);
    dhtData reading;

    while (true) {
        dht_read(pio0, sm, &reading);

        printf("Humidity:    %.1f%%\n", roundf(reading.humidity * 100) / 100);
        printf("Temperature: %.1fC\n\n", roundf(reading.temperature * 100) / 100);

        // Remember: DHT22/AM2302 can only be read once every 2 seconds.
        sleep_ms(5000);
    }
}
