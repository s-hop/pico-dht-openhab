# pico-dht-openhab
Send temperature and humidity readings from a Raspberry Pi Pico W to an OpenHab home automation installation via the MQTT protocol. 

Temperature and humidity readings are taken from a DHT22/AM2302 sensor using the Pico's programmable input/output (PIO) peripheral.

The Pico W uses its onboard Wi-Fi chip and the LwiP library to connect to the local network and act as an MQTT client, publishing DHT readings to an MQTT broker. This example project uses the broker software, Mosquitto, but the Pico programme is platform-agnostic, so you should have no problems connecting to any other broker software you may wish to use.

## Setting it up
This project was created with the Pico C/C++ SDK installed on an Ubuntu Linux machine, with a Raspberry Pi 4 hosting the OpenHab and Mosquitto servers on the [OpenHabian](https://www.openhab.org/docs/installation/openhabian.html) Debian Linux distribution.

It is assumed that you have the Pico SDK installed and have some knowledge of the MQTT protocol and OpenHab concepts (bindings, things, channels, etc.). 

Some useful resources can be found here:

- [Pico SDK Setup](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html)
- [MQTT Essentials video series](https://youtu.be/jTeJxQFD8Ak)
- [Mosquitto documentation](https://mosquitto.org/documentation/)
- [OpenHab Getting Started](https://www.openhab.org/docs/tutorial/)


### Pico
Create a new header file in the main project directory called, "connection_config.h" and paste the following code into it:
```
/*
 * Stores definitions for all connection configuration details
 */

#ifndef DHT_OPENHAB_CONNECTION_CONFIG_H
#define DHT_OPENHAB_CONNECTION_CONFIG_H

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"

#define MQTT_IP "192.168.0.0"

#endif //DHT_OPENHAB_CONNECTION_CONFIG_H
```
- Replace the WIFI_SSID and WIFI_PASS definitions with the details of your Wi-Fi access point. 
- Replace the MQTT_IP definition with the IP address of your MQTT broker. 
- **As a secure connection is not used in this project, you should only connect to a device on your local network.**

This should be all that is needed to be able to compile the program and upload it to your Pico. The program will connect to WiFi and the MQTT broker before publishing a JSON string containing the humidity and temperature readings from the DHT sensor. The readings will be extracted from the JSON string and converted to appropriate number values in OpenHab later.


### Mosquitto
The following steps are carried out on an OpenHabian (Debian) installation, so may not be applicable if you are using a different OS.

Install Mosquitto:
```
sudo apt install mosquitto
```

To get Mosquitto up and running in the background, input the following:

```
sudo systemctl start mosquitto
sudo systemctl daemon-reload
sudo systemctl enable mosquitto
```

You can use the [mosquitto_sub](https://mosquitto.org/man/mosquitto_sub-1.html) command to check that the broker is receiving messages from the Pico correctly:
```
mosquitto_sub -q 1 -t "pico/dht"
``` 

If you receive a "permission denied" error, you may need to modify the mosquitto.conf file to allow anonymous connections. Open the file with the following command:
```
sudo nano /etc/mosquitto/mosquitto.conf
``` 

Change "false" in this line to "true": 
```
allow_anonymous false
```


### OpenHab
