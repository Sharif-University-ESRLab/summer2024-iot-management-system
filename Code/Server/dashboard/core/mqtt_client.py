import json
import threading

import paho.mqtt.client as mqtt


MQTT_BROKER = "bore.pub"
MQTT_PORT = 25506
MQTT_KEEPALIVE = 60
MQTT_USER = "uname"
MQTT_PASSWORD = "upass"
MQTT_DEVICES = {}


# os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'myproject.settings')
# django.setup()
class SingletonMeta(type):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super().__call__(*args, **kwargs)
        return cls._instances[cls]


class SingletonClient(metaclass=SingletonMeta):
    def __init__(self):
        self.client = mqtt.Client()


def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")


def on_message(client, userdata, msg):
    payload = json.loads(msg.payload.decode())
    print(f"Topic: {msg.topic}\nMessage: {msg.payload.decode()}")
    from core.models import Device,Sensor
    device = Device.objects.get(token=payload['token'])
    if device.device_type == 2:
        device: Sensor = Sensor.objects.get(token=payload['token'])
    device.on_message(payload['data'])


def start_mqtt_client():
    client = SingletonClient().client
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    client.connect(MQTT_BROKER, MQTT_PORT, MQTT_KEEPALIVE)
    client.loop_forever()


def run_mqtt_client_in_thread():
    mqtt_thread = threading.Thread(target=start_mqtt_client)
    mqtt_thread.daemon = True
    mqtt_thread.start()
