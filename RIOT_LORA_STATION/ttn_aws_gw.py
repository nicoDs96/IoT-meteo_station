import paho.mqtt.client as mqtt
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import logging
import time
import json
import base64
import datetime
import argparse


def init_mqtt_connection(useWebsocket = False,
    clientId = 'basicPubSub',
    host = 'a1czszdg9cjrm-ats.iot.us-east-1.amazonaws.com',
    rootCAPath = 'root-CA.crt',
    privateKeyPath = 'station.private.key',
    certificatePath = 'station.cert.pem'):
    """

    Args:
        useWebsocket: boolean flag
        clientId: name of the client allowed to publish according to IoT Core policy
        host: hostname of your iot core btoker
        rootCAPath: AWS root ca
        privateKeyPath: path + filename of pvt key generated within the policy
        certificatePath: path + filename generated within the policy

    Returns:

    """
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
    Args:
        myAWSIoTMQTTClient: the connection to aws created with init_client()
        data: raw data to be sent
        topic: topic to publish to
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
    

def convert_ttn_to_aws(ttn_payload_raw):
    """
    converts the base64 payload from ttn to a dict.
    Args:
        ttn_payload_raw: payload_raw field into ttn json message payload still base 64
    """
    payload = base64.b64decode(ttn_payload_raw)
    payload = payload.split(',')
    new_msg = {}
    new_msg['timestamp'] = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')
    new_msg['temperature'] = payload[0]
    new_msg['humidity'] = payload[1]
    new_msg['wind_direction'] = payload[2]
    new_msg['wind_intensity'] = payload[3]
    new_msg['rain_height'] = payload[4]

    return new_msg


#defined globally for simplicity
aws_cli = init_mqtt_connection(clientId='station1',
                               rootCAPath='./../python_simulated_station/root-CA.crt',
                               privateKeyPath='./../python_simulated_station/station.private.key',
                               certificatePath='./../python_simulated_station/station.cert.pem')


def ttn_on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    payload = json.loads(msg.payload)
    new_payload = convert_ttn_to_aws( payload['payload_raw'] )
    new_payload['station_id'] = payload['dev_id']
    #print("New payload:")
    #print(json.dumps(new_payload))
    #print("\n\n")
    
    aws_cli.connect()
    topic = "station" if new_payload['station_id'] == "stat_1" else "station2"
    send_data(aws_cli, new_payload, topic)
    aws_cli.disconnect()

if __name__ == "__main__":
    
    ttn_app_id = "env_stat"
    ttn_access_key = "ttn-account-v2.-YKpgPfGoBOeCIURbXDcCA_0nGg_DMwFKveFfiUx2bs"
    ttn_client_init(ttn_app_id, ttn_access_key, ttn_on_connect, ttn_on_message)

    while True:
        print("Listening... ")
        time.sleep(60) #sec
