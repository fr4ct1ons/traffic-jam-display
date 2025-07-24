import paho.mqtt.client as mqtt

MQTT_BROKER = "localhost"
MQTT_PORT = 1883

client = mqtt.Client()

def on_message(client, userData, message):
    print("Message received:")
    print(message)

def on_connect(client, userdata, flags, rc):
    print("Conectado ao MQTT")
    client.subscribe("traffic/+/raw")
    client.subscribe("traffic/all/health")

def start_mqtt(thisClient: mqtt.Client):
  
    #client.on_connect = on_connect
    thisClient.on_message = on_message

    thisClient.connect(MQTT_BROKER, MQTT_PORT, 60)
    thisClient.loop_start()
    
start_mqtt(client)

print(f"Client connected: {client.is_connected()}")

running = True

while running:
    print(f"Please insert an MQTT topic, or 'exit' to close:")
    messageTopic = input()

    if(messageTopic == "exit"):
        exit(0)

    print(f"Please write your MQTT message:")
    message = input()
    
    client.publish(messageTopic, message)
    
    



