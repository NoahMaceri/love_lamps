from paho.mqtt import client as mqtt_client
import re
import time
import uuid

emulated_board_name 	= 'testb2'
emulated_board_target 	= 'testb1'

# Read src/private.hpp for the defines
defines = {}
with open('src/private.hpp') as f:
    lines = f.readlines()
    for line in lines:
        line.rstrip()   
        m = re.search(r'#define\s+([A-Za-z]\w+)\s+(.*)', line)
        if m:
            # remove ""
            defines[m.group(1)] = m.group(2).replace('"', '')

broker = defines['MQTT_BROKER']
port = int(defines['MQTT_PORT'])
client_id = f'python-mqtt-{uuid.uuid4()}'
username = defines['MQTT_USER']
password = defines['MQTT_PASS']

print(f'Emulating {emulated_board_name} connecting to {broker}:{port} as {username} with password {password}')

def connect_mqtt() -> mqtt_client:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print(f"Failed to connect, return code {rc}")

    client = mqtt_client.Client(client_id)
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")
        # wait four seconds then send the same message to the other board
        time.sleep(5)
        client.publish(f'love_lamps/{emulated_board_target}', msg.payload.decode())
        print(f"Sent `{msg.payload.decode()}` to `{emulated_board_target}` topic")

    client.subscribe(f'love_lamps/{emulated_board_name}')
    client.on_message = on_message


def mqtt_run():
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()

if __name__ == "__main__":
	mqtt_run()
