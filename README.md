# SmartTankFillingSystem
**You can deploy this solution to the place where you need to fill the tank automatically by controlling the motor.**

# Parts

* **3 X ESP12 Microcontroller board**
* **Ultrasonic Sensor**
* **Relay**
* **2 X 5V Power Supply**
* **3.6V Rechargeable battery**
* **2 X AMS1117 3.3 V Voltage Regulator**

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


