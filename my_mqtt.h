/*
 * Created by Steve Hopkins on 8/13/23.
 */

#ifndef PICO_DHT_OPENHAB_MY_MQTT_H
#define PICO_DHT_OPENHAB_MY_MQTT_H

#include "lwip/apps/mqtt.h"

void mqtt_do_connect(mqtt_client_t *client);
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
void mqtt_do_publish(mqtt_client_t *client, char *topic, char *payload, void *arg);
static void mqtt_pub_request_cb(void *arg, err_t result);

#endif //PICO_DHT_OPENHAB_MY_MQTT_H
