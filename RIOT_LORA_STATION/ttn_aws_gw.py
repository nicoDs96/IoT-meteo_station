import paho.mqtt.client as mqtt
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import logging
import time
import json

import datetime
import argparse


def init_mqtt_connection(useWebsocket = False,
    clientId = 'basicPubSub',
    host = 'a1czszdg9cjrm-ats.iot.us-east-1.amazonaws.com',
    rootCAPath = 'root-CA.crt',
    privateKeyPath = 'station.private.key',
    certificatePath = 'station.cert.pem'):

    port = 8883 if not useWebsocket else 443
    useWebsocket = useWebsocket
    clientId = clientId
    host = host
    port = port
    rootCAPath = rootCAPath
    privateKeyPath = privateKeyPath
    certificatePath = certificatePath

    logger = logging.getLogger("AWSIoTPythonSDK.core")
    logger.setLevel(logging.NOTSET)
    streamHandler = logging.StreamHandler()
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    streamHandler.setFormatter(formatter)
    logger.addHandler(streamHandler)

    # Init AWSIoTMQTTClient
    myAWSIoTMQTTClient = None
    if useWebsocket:
        myAWSIoTMQTTClient = AWSIoTMQTTClient(clientId, useWebsocket=True)
        myAWSIoTMQTTClient.configureEndpoint(host, port)
        myAWSIoTMQTTClient.configureCredentials(rootCAPath)
    else:
        myAWSIoTMQTTClient = AWSIoTMQTTClient(clientId)
        myAWSIoTMQTTClient.configureEndpoint(host, port)
        myAWSIoTMQTTClient.configureCredentials(rootCAPath, privateKeyPath, certificatePath)

    # AWSIoTMQTTClient connection configuration
    myAWSIoTMQTTClient.configureAutoReconnectBackoffTime(1, 32, 20)
    myAWSIoTMQTTClient.configureOfflinePublishQueueing(-1)  # param: queue_size,if set to 0, the queue is disabled. If set to -1, the queue size is set to be infinite.
    myAWSIoTMQTTClient.configureDrainingFrequency(2)  # Draining: 2 Hz
    myAWSIoTMQTTClient.configureConnectDisconnectTimeout(10)  # 10 sec
    myAWSIoTMQTTClient.configureMQTTOperationTimeout(5)  # 5 sec

    return myAWSIoTMQTTClient


def send_data(myAWSIoTMQTTClient, data, topic):
    """"
    data: dict to be transformed into json
    """
    messageJson = json.dumps(data)
    myAWSIoTMQTTClient.publish(topic, messageJson, 1)
    print('Published topic %s: %s\n' % (topic, messageJson))
    
def ttn_client_init(ttn_app_id, ttn_access_key, on_con, on_msg):
    '''
    ttn_app_id: identifier of the application defined on theThingsNetwork.
    ttn_access_key: v2 account accesskey associated with yout the things network account
    on_con: a callback executed on connection
    on_msg: a callback esecuted on received message
    '''
    client = mqtt.Client()
    client.on_connect = on_con
    client.on_message = on_msg
    client.username_pw_set(ttn_app_id, ttn_access_key )
    client.connect("eu.thethings.network", 1883, 60)

    # Blocking call that processes network traffic, dispatches callbacks and
    # handles reconnecting.
    # Other loop*() functions are available that give a threaded interface and a
    # manual interface.
    client.loop_forever()

def ttn_on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    print("userdata: %s\nflags: %s\n"%(userdata,flags))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("+/devices/+/up")


def ttn_on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    print("\n\n client: %s \nuserdata: %s"%(client,userdata))


if __name__ == "__main__":
    '''clientId = 'station1'
    topic = 'station'

    parser = argparse.ArgumentParser(description='Init Simulated Weather Station (MQTT Client) with proper clientId and topic name.')

    parser.add_argument('--clientid', type=str, default='station1', help='AWS IoT core Client Id. Allowed values \'station1\' or \'station2\'')
    parser.add_argument('--topic', type=str, default='station', help='Topic to publish to. Allowed values: \'station\' or \'station2\'')
    args = parser.parse_args()

    print( "========== Running With ==========\nClientID:\t%s\nTopic:\t%s"%(str(args.clientid),str(args.topic) ) )
    clientId = args.clientid
    topic = args.topic

    #sys.exit(0)

    myAWSIoTMQTTClient =  init_mqtt_connection(clientId=clientId)
    myAWSIoTMQTTClient.connect()

    i = 0
    '''
    ttn_app_id = "env_stat"
    ttn_access_key = "ttn-account-v2.-YKpgPfGoBOeCIURbXDcCA_0nGg_DMwFKveFfiUx2bs"
    ttn_client_init(ttn_app_id, ttn_access_key, ttn_on_connect, ttn_on_message)

    while True:
        print("Listening... ")
        time.sleep(60) #sec
