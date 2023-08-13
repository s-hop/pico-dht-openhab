/*
 * Created by Steve Hopkins on 8/13/23.
 */

#include <string.h>

#include "my_mqtt.h"
#include "connection_config.h"

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

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("mqtt_connection_cb: Successfully connected\n");
    } else {
        printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);
        // Attempt to reconnect when connection fails
        mqtt_do_connect(client);
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

// Called when publish is complete either with success or failure
static void mqtt_pub_request_cb(void *arg, err_t result) {
    if (result != ERR_OK) {
        printf("Publish result: %d\n", result);
    }
}