import time
import uuid
from datetime import datetime, timedelta
import json

import pytz
from django.contrib.auth.models import User
from django.db import models

from core.mqtt_client import SingletonClient

TYPE_CHOICES = [
    (0, 'Boolean Actuator'),
    (1, 'Integer Actuator'),
    (2, 'Sensor'),
]


class Device(models.Model):
    name = models.CharField(max_length=50)
    topic = models.CharField(max_length=50)
    token = models.UUIDField(default=uuid.uuid4, editable=False)
    user = models.ForeignKey(User, on_delete=models.CASCADE, related_name='data',null=True)
    device_type = models.SmallIntegerField(choices=TYPE_CHOICES)

    def publish_message(self, message):
        dic = {'data': message, 'token': str(self.token)}
        json_string = json.dumps(dic)
        SingletonClient().client.publish(self.topic, json_string)

    def on_message(self, message):
        # Implement your logic for handling incoming messages
        pass

    def get(self):
        # Implement your custom logic for retrieving data
        pass

    def get_element(self):
        # Implement your custom logic for retrieving specific elements
        pass

    def __str__(self):
        return self.token


class IntegerActuator(Device):

    def send_int(self, number):
        self.publish_message(str(number))


class BooleanActuator(Device):

    def turn_on(self):
        self.publish_message('on')

    def turn_off(self):
        self.publish_message('off')


class Sensor(Device):

    def on_message(self, message):
        DataModel.objects.create(device=self, message=message)

    def get_data(self):
        current_datetime = datetime.now()
        one_hour_ago = current_datetime - timedelta(minutes=10)
        one_hour_ago= one_hour_ago.astimezone(pytz.UTC)
        recent_messages = DataModel.objects.filter(device=self, received_at__gt=one_hour_ago).order_by('received_at')
        return recent_messages



class DataModel(models.Model):
    device = models.ForeignKey(Device, on_delete=models.CASCADE, related_name='data')  # Add this line
    message = models.TextField()
    received_at = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return f"{self.device} - {self.message[:50]}"
