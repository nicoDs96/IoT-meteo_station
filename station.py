import random as r
import math
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import logging
import time
import json
import GLOBAL_PARAMS


# TODO: implement persistence
# TODO: QoS at least 1
# TODO: client to show data -> subscriber + cusotm callback + dashboard
# TODO: if required add storage



def clip_value(minimum, maximum, value):
    '''to ensure we respect values limits we clip randomly
    generated values to lie in theit min-max range'''
    return max(minimum, min(value, maximum))


def read_sensors():
    '''
    generate random data simulating sensors.
    returns:
    -temperature
    -humidity
    -wind_direction
    -wind_intensity
    -rain_height
    '''
    gaussian_noise = r.gauss(GLOBAL_PARAMS.NOISE_MEAN, GLOBAL_PARAMS.NOISE_STD)
    temperature = r.uniform(GLOBAL_PARAMS.TEMP_MIN, GLOBAL_PARAMS.TEMP_MAX) + gaussian_noise
    temperature = clip_value(GLOBAL_PARAMS.TEMP_MIN, GLOBAL_PARAMS.TEMP_MAX, temperature)  # (-50 ... 50 Celsius)

    gaussian_noise = r.gauss(GLOBAL_PARAMS.NOISE_MEAN, GLOBAL_PARAMS.NOISE_STD)
    humidity = r.gauss(GLOBAL_PARAMS.HUM_MEAN, GLOBAL_PARAMS.HUM_STD) + gaussian_noise
    humidity = clip_value(GLOBAL_PARAMS.HUM_MIN, GLOBAL_PARAMS.HUM_MAX, humidity)  # (0 ... 100%)

    gaussian_noise = r.gauss(GLOBAL_PARAMS.NOISE_MEAN, GLOBAL_PARAMS.NOISE_STD)
    wind_direction = math.degrees(r.vonmisesvariate(GLOBAL_PARAMS.WIND_D_MU, GLOBAL_PARAMS.WIND_D_K))

    gaussian_noise = r.gauss(GLOBAL_PARAMS.NOISE_MEAN, GLOBAL_PARAMS.NOISE_STD)
    wind_intensity = r.uniform(GLOBAL_PARAMS.WIND_I_MIN, GLOBAL_PARAMS.WIND_I_MAX) + gaussian_noise
    wind_intensity = clip_value(GLOBAL_PARAMS.WIND_I_MIN, GLOBAL_PARAMS.WIND_I_MAX, wind_intensity)  # (0 ... 100 m/s)

    gaussian_noise = r.gauss(GLOBAL_PARAMS.NOISE_MEAN, GLOBAL_PARAMS.NOISE_STD)
    rain_height = r.triangular(GLOBAL_PARAMS.RAIN_H_LOW, GLOBAL_PARAMS.RAIN_H_HIGH, GLOBAL_PARAMS.RAIN_H_MODE) + gaussian_noise  # (0 ... 50 mm / h)
    rain_height = clip_value(GLOBAL_PARAMS.RAIN_H_MIN, GLOBAL_PARAMS.RAIN_H_MAX, rain_height)

    return temperature, humidity, wind_direction, wind_intensity, rain_height

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
    logger.setLevel(logging.DEBUG)
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


if __name__ == "__main__":
    clientId = 'station1'
    topic = 'station'

    myAWSIoTMQTTClient =  init_mqtt_connection(clientId=clientId)
    myAWSIoTMQTTClient.connect()


    def disconnect():
        myAWSIoTMQTTClient.disconnect()
        print("\nDISCONNECTED\n")

    while True:
        temperature, humidity, wind_direction, wind_intensity, rain_height = read_sensors()
        data = {}
        data['temperature'] = temperature
        data['humidity'] = humidity
        data['wind_direction'] = wind_direction
        data['wind_intensity'] = wind_intensity
        data['rain_height'] = rain_height
        send_data(myAWSIoTMQTTClient, data, topic)
        time.sleep(1)

    myAWSIoTMQTTClient.disconnect()


