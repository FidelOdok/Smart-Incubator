
import paho.mqtt.client as paho
from paho import mqtt
import ast
from typing import Counter
from pandas import read_csv
import psycopg2
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

cur.execute("DROP TABLE incubator;")
cur.execute("CREATE TABLE incubator(id serial PRIMARY KEY, value timestamp, temperature numeric, humidity numeric, inputVoltage numeric, maxPower numeric, heaterOntime numeric);")




insert_sql = """INSERT INTO incubator(value, temperature,humidity,inputVoltage,maxPower, heaterOntime) VALUES(%s, %s, %s,%s, %s, %s)"""


# setting callbacks for different events to see if it works, print the message etc.
def on_connect(client, userdata, flags, rc, properties=None):
    print("CONNACK received with code %s." % rc)

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

    cur.execute(insert_sql, [dt.datetime.now(), my_data['temperature']['value'],my_data['humidity']['value'],my_data['inputVoltage']['value'],my_data['maxPower']['value'],my_data['heaterOntime']['value']])
    conn.commit()
    print([dt.datetime.now(), my_data['temperature']['value'],my_data['humidity']['value']])
    

client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
client.on_connect = on_connect

# enable TLS for secure connection
client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
client.username_pw_set("smartIncubator", "SmartIncubator@1234")
client.connect("0657fea3e3c14f5cb989d7bffd886dfd.s1.eu.hivemq.cloud", 8883)


client.on_subscribe = on_subscribe
client.on_message = on_message



client.subscribe("data/monitor/batteryStatus", qos=0)

client.loop_forever()