from django.db.models.signals import post_migrate
from django.dispatch import receiver

from core.mqtt_client import SingletonClient


@receiver(post_migrate)
def subscribe_to_mqtt_topics(sender, **kwargs):
    client = SingletonClient().client
    from core.models import Device
    devices = Device.objects.all()
    for device in devices:
        client.subscribe(device.topic, 0)
