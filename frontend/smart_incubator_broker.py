import time
import paho.mqtt.client as paho
from paho import mqtt
import ast
from typing import Counter
from pandas import read_csv
import psycopg2
import time
import numpy as np
import datetime as dt

# Converting the np.int datatype to python int type
from psycopg2.extensions import register_adapter, AsIs
def addapt_numpy_int64(numpy_int64):
    return AsIs(numpy_int64)
register_adapter(np.int64, addapt_numpy_int64)


#connect to an existing database
conn = psycopg2.connect(
	dbname="d9vue3b4a4ucqd",
	user="wcuqojikeffpul",
	password="a3f3ec48375c0a41e69f0eb67e433fad21d78f215c07f5f38581bc308e790d06",
	host="ec2-34-205-46-149.compute-1.amazonaws.com",
	port=5432
	)

cur = conn.cursor()
# An execute command that creates a new table
cur.execute("CREATE TABLE incubator(id serial PRIMARY KEY, value timestamp, voltage numeric);")


insert_sql = """INSERT INTO incubator(value, voltage) VALUES(%s, %s)"""


# setting callbacks for different events to see if it works, print the message etc.
def on_connect(client, userdata, flags, rc, properties=None):
    print("CONNACK received with code %s." % rc)

# with this callback you can see if your publish was successful
def on_publish(client, userdata, mid, properties=None):
    print("mid: " + str(mid))

# print which topic was subscribed to
def on_subscribe(client, userdata, mid, granted_qos, properties=None):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

# print message, useful for checking if it was successful
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    byte_str = msg.payload
    dict_str = byte_str.decode("UTF-8")
    my_data = ast.literal_eval(dict_str)
    print(repr(my_data))

    cur.execute(insert_sql, [dt.datetime.now(), my_data['maxVolt']['value']])
    conn.commit()
    print([dt.datetime.now(), my_data['maxVolt']['value']])
    

# using MQTT version 5 here, for 3.1.1: MQTTv311, 3.1: MQTTv31
# userdata is user defined data of any type, updated by user_data_set()
# client_id is the given name of the client
client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
client.on_connect = on_connect

# enable TLS for secure connection
client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
# set username and password
client.username_pw_set("smartIncubator", "SmartIncubator@1234")
# connect to HiveMQ Cloud on port 8883 (default for MQTT)
client.connect("0657fea3e3c14f5cb989d7bffd886dfd.s1.eu.hivemq.cloud", 8883)

# setting callbacks, use separate functions like above for better visibility
client.on_subscribe = on_subscribe
client.on_message = on_message
client.on_publish = on_publish

# subscribe to all topics of encyclopedia by using the wildcard "#"
client.subscribe("data/monitor/batteryStatus", qos=0)

# a single publish, this can also be done in loops, etc.
client.publish("encyclopedia/temperature", payload="hot", qos=0)

# loop_forever for simplicity, here you need to stop the loop manually
# you can also use loop_start and loop_stop
client.loop_forever()