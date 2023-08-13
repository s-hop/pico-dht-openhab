//
// Created by Steve Hopkins on 8/13/23.
//

#include <pico/cyw43_arch.h>

#include "wifi.h"

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
