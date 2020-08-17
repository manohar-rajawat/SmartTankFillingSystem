# SmartTankFillingSystem

*You can deploy this solution to the place where you need to fill the tank automatically by controlling the motor.*

# Parts

* **3 X ESP12 Microcontroller board**
* **Ultrasonic Sensor**
* **Relay**
* **2 X 5V Power Supply**
* **3.6V Rechargeable battery**
* **2 X AMS1117 3.3 V Voltage Regulator**
* **5V Solar Panel**

# Implementation Tips

### This whole implementation involves 3 different parts

* AP (Which works as central access point, Server and Client will be connected to this)
* Server (Which controlls the motor)
* Client (Which will be implemented on the tank)

# AP

*This is the central access point to which the server and client will be connected. If your tank and motor has less distance then you can remove this part and make the ap on the server itself. But if server's access point is not visible to client then you need to put this to somewhere between*

#### AP Configuration
* IP - 192.168.1.1
* GATEWAY - 192.168.1.1
* SUBNET - 255.255.255.0
* DNS - 8.8.8.8/8.8.4.4

> This AP has a simple http server which will help us to reboot it using web browser. If you don't need this please comment out the code.

# Server

*This is the backbone of this project. The server will be connected to ap using wifi with the below configurations. It will server a http web server to let the client control set the water level or start the motor. Relay will be connected to the ESP12 here, you can check the code to get the pins used to control them.*

#### Server Configuration
* IP - 192.168.1.150
* GATEWAY - 192.168.1.1
* SUBNET - 255.255.255.0
* DNS - 8.8.8.8/8.8.4.4

> This Server has a simple http server. Which is used to update level and starting motor. The client will hit the server each time it needs to update something. We have designed a simple HTML+CSS+JAVASCRIPT Homepage (192.168.1.150) Using this web page you can manually control the motor. It has built in web socket support to synchronize the data between each client in realtime.

### Motor Off
![MotorOff](https://i.ibb.co/s6H1Dwz/MotorOff.png)

### Motor On
![MotorOn](https://i.ibb.co/TwtQFnX/MotorOn.png)

# Client

*The client will be connected to ap using wifi. It will hit the server's http links to update the tank water levevl and start the motor if water level is less than defined limits. Ultrasonic sensor will be connected to the ESP12 here, you can check the code to get the pins used to control them. We are putting this ESP12 in DeepSleep after updating the water level. ESP12 will wake up after 15 minutes. It will save the battery life. And we are recharging the battery using solar panels. You can change the timing for DeepSLeep in the code.*

# Enjoy Home Automation :)



