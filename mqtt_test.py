from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import logging
import time
import json

logger = logging.getLogger("AWSIoTPythonSDK.core")
logger.setLevel(logging.DEBUG)
streamHandler = logging.StreamHandler()
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
streamHandler.setFormatter(formatter)
logger.addHandler(streamHandler)
useWebsocket = False
clientId = 'basicPubSub'
host = 'a1czszdg9cjrm-ats.iot.us-east-1.amazonaws.com'
port = 8883 if not useWebsocket else 443
topic = "sdk/test/Python"
rootCAPath = 'root-CA.crt'
privateKeyPath = 'station.private.key'
certificatePath = 'station.cert.pem'

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
myAWSIoTMQTTClient.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
myAWSIoTMQTTClient.configureDrainingFrequency(2)  # Draining: 2 Hz
myAWSIoTMQTTClient.configureConnectDisconnectTimeout(10)  # 10 sec
myAWSIoTMQTTClient.configureMQTTOperationTimeout(5)  # 5 sec


# Custom MQTT message callback
def customCallback(client, userdata, message):
    print("Received a new message: ")
    print(message.payload)
    print("from topic: ")
    print(message.topic)
    print("--------------\n\n")

# Connect and subscribe to AWS IoT
myAWSIoTMQTTClient.connect()
#if 'subscribe':
#    myAWSIoTMQTTClient.subscribe(topic, 1, customCallback)
#time.sleep(2)

# Publish to the same topic in a loop forever
loopCount = 0
while loopCount<30:

    message = {}
    message['message'] = "messaggino nr. "+str(loopCount+1)
    message['sequence'] = loopCount
    messageJson = json.dumps(message)
    myAWSIoTMQTTClient.publish(topic, messageJson, 1)

    print('Published topic %s: %s\n' % (topic, messageJson))
    loopCount += 1
    time.sleep(1)
myAWSIoTMQTTClient.disconnect()
