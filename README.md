# pico-dht-openhab
Send temperature and humidity readings from a Raspberry Pi Pico W to an OpenHab home automation installation via the MQTT protocol. 

Temperature and humidity readings are taken from a DHT22/AM2302 sensor using the Pico's programmable input/output (PIO) peripheral.

The Pico W uses its onboard Wi-Fi chip and the LwiP library to connect to the local network and act as an MQTT client, publishing DHT readings to an MQTT broker. This example project uses the broker software, Mosquitto, but the Pico programme is platform-agnostic, so you should have no problems connecting to any other broker software you may wish to use.

<p align="center">
  <img src="/openhab/widget.png" />
</p>


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

Restart Mosquitto:
```
sudo systemctl restart mosquitto
```

### OpenHab
Navigate to the OpenHabian dashboard at ```http://openhabian:8080/```. 

To connect to Mosquitto, we need to install the MQTT Binding. The bindings available can be found under ```Settings>Add-ons>Bindings```.

As the incoming data is in JSON format and needs to be extracted, we also need to install the JSONPath Transformation add-on. This can be found under ```Settings>Add-ons>Other Add-ons>Transformation Add-ons```.

#### Things
The first 'thing' that needs to be created is the connection to Mosquitto:
- Under **Settings>Things** click the '**+**' icon to add a new 'thing'.
- Choose **MQTT Binding > MQTT Broker**.
- Give the thing a unique identifier of 'mosquitto'.
- Add the Hostname/IP of your mosquitto server under **Broker Hostname/IP**.
- Click '**Create Thing**'.

Ignore **advanced settings** for now. Once the thing is created, the configuration can be updated using the code in **openhab/thing_mosquitto.txt** to match the configuration of the publications from the Pico.

The second 'thing' to be created is a representation of the Pico:
- Create a new 'thing' as before.
- This time, choose **MQTT Binding > Generic MQTT Thing**.
- Give the thing a unique identifier of 'pico'.
- Set the **Bridge** to the previously created **Mosquitto** thing.

As with the Mosquitto thing, replace the configuration code of this thing with that in the **openhab/thing_pico.txt** file.

#### Channels
The Pico 'thing' requires two channels, one each for the humidity and temperature readings. JSONPath is used here to extract the values from the JSON string and convert them to number values for displaying with 'items'.

The configuration code used in the previous step should also have created these channels. All that is required now is the linking of each channel to its respective humidity/temperature 'item'.

#### Items
Create and link a new item for each channel, using the metadata code from the **openhab/metadata_x.txt** files to set the correct units and formatting.

Example code for an overview page is available in the **openhab/page_overview.txt** file. This creates a list widget that displays the readings provided by the humidity and temperature 'items'.