from django.urls import path

from core import views
from core.views import HomePageView

urlpatterns = [
    path('login/', views.login, name='login'),
    path('signup/', views.signup, name='signup'),
    path('', HomePageView.as_view(), name='home'),
    path('sensor/data/<str:uid>', views.get_data, name='get_data'),

    path('devices', views.get_device_list, name='get_device_list'),

    path('boolean/<str:uid>/on/', views.turn_on, name='turn_on'),
    path('boolean/<str:uid>/off/', views.turn_off, name='turn_off'),

    path('integer/<str:uid>/', views.integer, name='send_int'),

    path('add/', views.add, name='add'),

    path('start', views.start),

]
