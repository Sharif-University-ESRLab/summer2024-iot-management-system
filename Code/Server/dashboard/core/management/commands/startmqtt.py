from django.core.management.base import BaseCommand
from core.mqtt_client import client, run


class Command(BaseCommand):
    help = 'Starts the MQTT client'

    def handle(self, *args, **kwargs):
        self.stdout.write(self.style.SUCCESS('Starting MQTT client...'))
        run()
