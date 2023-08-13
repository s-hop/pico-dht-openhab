#include <stdio.h>
#include <math.h>
#include <pico/cyw43_arch.h>

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "lwip/apps/mqtt.h"

#include "dht.h"
#include "connection_config.h"

void mqtt_do_connect(mqtt_client_t *client);

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

// Called when publish is complete either with success or failure
static void mqtt_pub_request_cb(void *arg, err_t result) {
    if (result != ERR_OK) {
        printf("Publish result: %d\n", result);
    }
}

void mqtt_do_publish(mqtt_client_t *client, char *topic, char *payload, void *arg) {
    err_t err;
    u8_t qos = 1; // 0 1 or 2, see MQTT specification
    u8_t retain = 0; // Don't want to retain payload
    err = mqtt_publish(client, topic, payload, strlen(payload), qos, retain, mqtt_pub_request_cb, arg);
    if (err != ERR_OK) {
        printf("Publish err: %d\n", err);
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("mqtt_connection_cb: Successfully connected\n");
    } else {
        printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);
        // Attempt to reconnect when connection fails
        mqtt_do_connect(client);
    }
}

void mqtt_do_connect(mqtt_client_t *client) {
    struct mqtt_connect_client_info_t ci;
    err_t err;

    // Set up an empty client info structure
    memset(&ci, 0, sizeof(ci));

    // Minimal amount of information required is client identifier, so set it here
    ci.client_id = "pico";

    // Convert broker IP string to ipv4
    ip_addr_t mqtt_ip;
    ipaddr_aton(MQTT_IP, &mqtt_ip);

    /*
     * Initiate client and connect to server, if this fails immediately an error code is returned otherwise
     * mqtt_connection_cb will be called with connection result after attempting to establish a connection with
     * the server.
     */
    err = mqtt_client_connect(client, &mqtt_ip, MQTT_PORT, mqtt_connection_cb, 0, &ci);
    if (err != ERR_OK) {
        printf("mqtt_connect return %d\n", err);
    }
}


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
