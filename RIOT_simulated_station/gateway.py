import random as r
import math
import sys
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import logging
import time
import json
import datetime
import argparse
# insert the library folder for supporting mqtts Client
sys.path.insert(1, 'lib')
from  MQTTSNclient import Client


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
    messageJson = json.dumps(data.replace("\\","")).replace("\\","")
    myAWSIoTMQTTClient.publish(topic, messageJson, 1)
    print('Published topic %s: %s\n' % (topic, messageJson))


class Callback:
    '''
    A class to add a MQTTS callback. It is a modification of Callback class into lib/ to support AWS retransission.

    '''

    def __init__(self, AWSClientId, aws_topic):
        self.events = []
        self.registered = {}
        self.aws_conn = init_mqtt_connection(clientId=AWSClientId)
        self.aws_conn.connect()
        self.aws_topic = aws_topic

    def connectionLost(self, cause):
        print
        "default connectionLost", cause
        self.events.append("disconnected")

    def messageArrived(self, topicName, payload, qos, retained, msgid):
        '''
        Each time a new message is received we forward it to aws IoT core broker
        '''
        print "default publishArrived", topicName, payload, qos, retained, msgid
        
        from_topic = str(payload)[len('{"station_id":')+1:len('{"station_id":')+7:]
        #to_topic = "station" if from_topic=="stat_1" else "station1"
        if from_topic == "stat_1":
            to_topic="station"
            send_data(self.aws_conn, payload, to_topic)
        elif from_topic == "stat_2":
            to_topic="station2"
            send_data(self.aws_conn, payload, to_topic)
        else:
            print("Topic <%s> Not recognized.\n"%from_topic)
        '''elif topicName == "stat_2":
            send_data(self.aws_conn, payload, "station1")
        else:
            print("Unknown Topic %s. Not forwarding.\nmsg:\t%s"%(topicName,payload))'''
        return True

    def deliveryComplete(self, msgid):
        print
        "default deliveryComplete"

    def advertise(self, address, gwid, duration):
        print
        "advertise", address, gwid, duration

    def register(self, topicid, topicName):
        self.registered[topicId] = topicName


if __name__ == "__main__":
    aws_clientId = 'station1'
    aws_topic = 'station'

    parser = argparse.ArgumentParser(description='Init Simulated Weather Station (MQTT Client) with proper clientId and topic name.')

    parser.add_argument('--awsclientid', type=str, default='station1',
                        help='AWS IoT core Client Id. Allowed values \'station1\' or \'station2\'')
    parser.add_argument('--awstopic', type=str, default='station',
                        help='AWS Topic to publish to. Allowed values: \'station\' or \'station2\'')
    parser.add_argument('--mqttclientid', type=str, default='gw',
                        help='MQTTS Broker Client Id. Allowed values: any.')
    parser.add_argument('--mqttstopics', type=str, default='stat_1',
                        help='MQTTS Topics to forward to AWS Iot Core Broker. Allowed values: \'stat_1\', \'stat_2\'.'
                             'To forward more than one topic write more topics separated with withespace, '
                             'like --mqttstopics "stat_1 stat2".')

    args = parser.parse_args()

    
    aws_clientId = args.awsclientid
    aws_topic = args.awstopic
    mqtts_topics = args.mqttstopics.split(" ")


    gateway = Client("gw", port=1885)
    gateway.registerCallback(Callback(AWSClientId=aws_clientId, aws_topic=aws_topic))
    gateway.connect()
    for topic in mqtts_topics:
        if(topic != ""):
            gateway.subscribe(topic)

    while True:
        time.sleep(1)


    myAWSIoTMQTTClient.disconnect()


