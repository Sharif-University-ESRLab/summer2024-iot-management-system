from django.contrib.auth.models import User
from django.http import JsonResponse
from django.views.generic import TemplateView

from core.models import Device, Sensor, BooleanActuator, IntegerActuator
from core.mqtt_client import SingletonClient


class HomePageView(TemplateView):
    template_name = 'core/index.html'


def get_data(request, uid):
    sensor: Sensor = Sensor.objects.get(token=uid)
    data = sensor.get_data()
    formated_data = [{'x': d.received_at.timestamp() * 1000, 'y': float(d.message)}
                     for d in data]
    return JsonResponse(formated_data, safe=False)


def get_device_list(request):
    devices = Device.objects.all()
    formatted = []
    for dev in devices:
        formatted.append([str(dev.name), str(dev.token), dev.device_type])
    return JsonResponse(formatted, safe=False)


def turn_on(request, uid):
    device: BooleanActuator = BooleanActuator.objects.get(token=uid)
    device.turn_on()
    return JsonResponse(data={'data': 'success'})


def turn_off(request, uid):
    device: BooleanActuator = BooleanActuator.objects.get(token=uid)
    device.turn_off()
    return JsonResponse(data={'data': 'success'})


def integer(request, uid):
    num = request.GET.get('value', None)
    device: IntegerActuator = IntegerActuator.objects.get(token=uid)
    device.send_int(num)
    return JsonResponse(data={'data': 'success'})


def start(request):
    client = SingletonClient().client
    from core.models import Device
    devices = Device.objects.all()
    for device in devices:
        client.subscribe(device.topic, 0)
    return JsonResponse(data={'data': 'success'})


def add(request):
    name = request.GET.get('name')
    device_type = request.GET.get('type')
    user = User.objects.get(username=request.GET.get('username', None))
    from core.models import Sensor, BooleanActuator, IntegerActuator
    if int(device_type) == 0:
        BooleanActuator.objects.create(name=name, topic=name, device_type=device_type, user=user)
    elif int(device_type) == 1:
        IntegerActuator.objects.create(name=name, topic=name, device_type=device_type, user=user)
    else:
        Sensor.objects.create(name=name, topic=name, device_type=device_type, user=user)
    return JsonResponse(data={'data': 'success'})


def login(request):
    username = request.GET.get('username', None)
    password = request.GET.get('password', None)
    try:
        user = User.objects.get(username=username)
        if user.password == password:
            data = {
                'success': True,
                'message': 'logged in successfully',
            }
            return JsonResponse(data, safe=False)
    except:
        print('e')
    data = {
        'success': False,
        'message': 'login fail',
    }
    return JsonResponse(data, safe=False)


def signup(request):
    username = request.GET.get('username', None)
    password = request.GET.get('password', None)
    try:
        User.objects.create(username=username, password=password)
        data = {
            'success': False,
            'message': 'Sorry cannot sign you in',
        }
        return JsonResponse(data, safe=False)

    except:
        print('signup error')
    data = {
        'success': True,
        'message': 'Sorry cannot sign you in',
    }
    return JsonResponse(data, safe=False)
