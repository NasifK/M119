# MQTT code inspired by Warren's MQTT code he presented in discussion. Thanks Warren!
# https://github.com/warrenwallis/ECEM119/tree/main/M2

# pong implentation
# https://www.geeksforgeeks.org/create-pong-game-using-python-turtle/

import paho.mqtt.client as mqtt
import numpy as np
import time
import matplotlib.pyplot as plt
import re
from pong import Pong

x_data = []
y_data = []

def mqtt_publisher():
    # 0. define callbacks - functions that run when events happen.
    # The callback for when the client receives a CONNACK response from the server.
    def on_connect(client, userdata, flags, rc):
        print("Connection returned result: " + str(rc))
        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect then subscriptions will be renewed.
        client.subscribe("ECEM119")
        # The callback of the client when it disconnects.

    def on_disconnect(client, userdata, rc):
        if rc != 0:
            print('Unexpected Disconnect')
        else:
            print('Expected Disconnect')
        # The default message callback.
        # (won't be used if only publishing, but can still exist)

    def on_message(client, userdata, message):
        my_message = str(message.payload)[2:-1]
        print('Received message: ', my_message)

        if "ax" in my_message:
            ax_match = float(re.search(r'ax\s*=\s*([-+]?\d*\.\d+|\d+)', my_message).group(1))
            print(axs)
            axs.append(ax_match)

    axs = [0]
    # 1. create a client instance.
    client = mqtt.Client()
    # add additional client options (security, certifications, etc.)
    # many default options should be good to start off.
    # add callbacks to client.
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message

    # 2. connect to a broker using one of the connect*() functions.
    # client.connect_async("test.mosquitto.org")
    client.connect_async('mqtt.eclipseprojects.io')

    # 3. call one of the loop*() functions to maintain network traffic flow with the broker.
    client.loop_start()
    # 4. use subscribe() to subscribe to a topic and receive messages.
    # 5. use publish() to publish messages to the broker.
    # payload must be a string, bytearray, int, float or None.
    #client.publish("ECEM119", curr, qos=1)

    # create new pong game
    pong = Pong()
    count = 0
    while True:
        # play pong game
        if count % 1000 == 0:
            pong.update_pong_game(axs[-1])
        count = count + 1

    # 6. use disconnect() to disconnect from the broker.
    client.loop_stop()
    client.disconnect()

def main():
    mqtt_publisher()

if __name__ == '__main__':
    main()