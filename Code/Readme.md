
# Code
Here is the code for backend and frontend and also code for 6 diffrent type of sensors and actuators. 

## Server
Server implemeted using `Django`. it use sqllite for database, and connect to mqtt broker with a thread in mqtt_client in core app. In order to use this sever, you need to also have a mqtt broker running and update the credentials for that.

## Client
The frontend developed by `vue.js`, it uses `apexcharts` for live updating charts.

## ESP32
ESP32 is prgorammed using `C++` in `Arduino IDE`. ServoMotor code need a library installed in order to work probebly. you also need to update the credential of wifi and mqtt.
