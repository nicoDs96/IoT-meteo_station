from station import init_mqtt_connection
import time
import _sqlite3
# Custom MQTT message callback
def customCallback(client, userdata, message):
    print("Received a new message: ")
    print(message.payload)
    print("from topic: ")
    print(message.topic)
    print("--------------\n\n")


if __name__ == "__main__":

    clientId = 'cons1'
    topic = 'station'

    # Connect and subscribe to AWS IoT
    myAWSIoTMQTTClient = init_mqtt_connection(clientId=clientId)
    myAWSIoTMQTTClient.connect()

    myAWSIoTMQTTClient.subscribe(topic, 1, customCallback)
    #myAWSIoTMQTTClient.subscribe(topic, 1, customCallback)
    #myAWSIoTMQTTClient.subscribe(topic, 2, customCallback)

    while True:
        print("Listening...")
        time.sleep(60)
        pass

    #myAWSIoTMQTTClient.disconnect()