# IOT management system

Control sensors and actuators connected to ESP32 over the internet!

## Tools
- Django
- Vue.js
- Mosquito
- ESP32
- ApexChart


## Implementation Details
This project has 4 part:
- ESP32 codes
- Django backend
- Vue.js frontend
- Musquito broker

ESP32s connect to a mqtt broker, and send their data and recive commands in json. Django server connect to broker, and send and recive data. each device has a object in django, with an token. this token is used for authentication and is send with each message.  to add device, you can use your panel in frontend. data is saved in a sql table, with device and time.


## How to Run

First, run a mosqutio broker on a server, you can use your it's offical documents. 
After runnig that, you should update the code for ESP32 to connect to wifi and also connect to mqtt broker.
also change the credentials of mqtt broker in mqtt_client file in the backend. now you can run django backend with this command in the folder of dashboard:
```bash
python manage.py runserver
```

after that, run this command in frontend folder to starts serving frontend:
```bash
npm run serve
```

for each esp32, you need a token, you can get that token by adding a new device in the frontend. you should be able to see your devices.
then upload code to the esp32 and customize ports if you like. when you login in you accuant, it gets your devices and show their contorls and data to you.

you can use bore for testing project over the Internet. 


## Results
login page:


![login](https://github.com/Sharif-University-ESRLab/summer2024-iot-management-system/blob/main/Miscellaneous/photo_2024-09-02_21-03-14.jpg)


user panel:


![user panel](https://github.com/Sharif-University-ESRLab/summer2024-iot-management-system/blob/main/Miscellaneous/photo_2024-09-02_21-03-20.jpg)


test:


![project test](https://github.com/Sharif-University-ESRLab/summer2024-iot-management-system/blob/main/Miscellaneous/photo_2024-09-02_21-02-33.jpg)



## Related Links
 - [ESP32 Pinout](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
 - [Django Doc](https://docs.djangoproject.com/en/5.0/)
 - [bore](https://github.com/ekzhang/bore)
 - [Apexcharts](http://apexcharts.com/)


## Authors
Authors and their github link come here.
- [@moeen89](https://github.com/moeen89/)
- [@amirhosse-in](https://github.com/amirhosse-in/)
- [@shayanshabani](https://github.com/shayanshabani/)
