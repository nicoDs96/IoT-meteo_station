from station import init_mqtt_connection
import time
import sqlite3
import json




# Custom MQTT message callback
def store_data(client, userdata, message):


    msg = message.payload.decode('utf8')#.replace("'", '"')
    data = json.loads( msg )
    '''print("NO EXC - Check access to data:")
    print(str(data['station_id']))
    print(str(data['timestamp']))
    print(str(data['temperature']))
    print(str(data['humidity']))
    print(str(data['wind_direction']))
    print(str(data['wind_intensity']))
    print(str(data['rain_height']))
    print("-------------------------\n\n\n")'''

    conn = sqlite3.connect('db/Stations.db')
    c = conn.cursor()
    # Create table TODO: remove it
    c.execute('''CREATE TABLE IF NOT EXISTS stations_data
                 (station_id text not null,
                 time_stamp datetime not null,  
                 temperature float, 
                 humidity float, 
                 wind_direction float, 
                 wind_intensity float, 
                 rain_height float,
                 primary key(station_id, time_stamp)
                 )'''
              )

    # Insert a row of data
    c.execute("INSERT INTO stations_data VALUES "
              "(\'"+str(data['station_id'])+"\'," 
              "\'"+str(data['timestamp'])+"\',"
              + str(data['temperature'])+","
              + str(data['humidity']) + ","
              + str(data['wind_direction']) + ","
              + str(data['wind_intensity'] ) + ","
              + str(data['rain_height']) +");"
              )

    # Commit
    conn.commit()

    # We can also close the connection if we are done with it.
    # Just be sure any changes have been committed or they will be lost.
    conn.close()


if __name__ == "__main__":

    clientId = 'cons1'
    topic = 'station'

    # Connect and subscribe to AWS IoT
    myAWSIoTMQTTClient = init_mqtt_connection(clientId=clientId)
    myAWSIoTMQTTClient.connect()

    myAWSIoTMQTTClient.subscribe(topic, 1, store_data)

    while True:
        print("Listening...")
        time.sleep(60)


    myAWSIoTMQTTClient.disconnect()