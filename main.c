#include <stdio.h>
#include <math.h>
#include <pico/cyw43_arch.h>

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"

#include "connection_config.h"
#include "wifi.h"
#include "my_mqtt.h"
#include "dht.h"

int main() {
    stdio_init_all();

    // Wi-Fi connection - Modify parameters as required.
    char ssid[] = WIFI_SSID;
    char password[] = WIFI_PASS;
    uint32_t country = CYW43_COUNTRY_UK;
    uint32_t auth = CYW43_AUTH_WPA2_AES_PSK;
    wifi_connect(ssid, password, country, auth);

    // MQTT client connection
    mqtt_client_t *client = mqtt_client_new();
    if (client != NULL) {
        mqtt_do_connect(client);
    }

    // DHT sensor initialisation
    uint sm = dht_initialise(pio0, 28);
    dhtData reading;

    while (true) {
        dht_read(pio0, sm, &reading);

        float humidity = roundf(reading.humidity * 100) / 100;
        float temperature = roundf(reading.temperature * 100) / 100;

        // Convert float values to a JSON string for MQTT publishing payload
        char payload_string[52];
        sprintf(payload_string, "{\"humidity\": \"%.1f\", \"temperature\": \"%.1f\"}", humidity, temperature);

        mqtt_do_publish(client, "pico/dht", payload_string, 0);

        // Remember: DHT22/AM2302 can only be read once every 2 seconds
        sleep_ms(5000);
    }
}
