### Installation (Pre-Req)
* Before running, please ensure that espeak, libmpg123 and sox are all installed on your BeagleBone:

```sudo apt install espeak ```
```sudo apt-get install sox ```
```sudo apt-get install libmpg123-dev ```


* And the libmpg123 library installed on your host:

```sudo apt-get install libmpg123-dev ```

### Timezone (Pre-Req)
* Due to constraints of the linux O/S requiring sudo to set the timezone, run the command below to sync to PST (Default is UTC)

```sudo timedatectl set-timezone America/Vancouver```


# Joystick
### Displaying Time (Hold):
| Input     | Value              |
|:----------|:------------------:|
|```Left``` |Hours               |
|```Up```   |Minutes             |
|```Right```|Seconds             |
|```Down``` |Text to Speech Current Time|


### Once Alarm Sounds:
| Input                        | Action             |
|:-----------------------------|:------------------:|
|```Any Direction         ```  |Snooze              |
|```Inwards               ```  |Shut off            |
|```Waive Hand Over Sensor```  |Shut off            |   

### LEDS:
In order to enable the two wired LEDs please do the following:
* Follow Brian Fraser's guide on setting up a device tree to enable our first (left) LED. Do this for only this one LED as the other LED, we will be driving the gpio pins instead which has already been done. Please go through this part of the guide for configuring the left LED:
     * Step 3. Device Tree: Enabling the LED (guide link: https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/guides/files/WiringAnLEDGuide.pdf)

* The two GPIO pins that have the wires connected on them are PIN23 and PIN27 both on the P9 headers. The power is outputting from PIN3 also on the P9 header. 
* For wiring up the LED: <br>
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/ledwiring/20230318_184828.jpg" width="500" height="500">
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/ledwiring/20230318_184838.jpg" width="500" height="500">
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/ledwiring/20230321_113237.jpg" width="500" height="500">
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/ledwiring/led1.png" width="500" height="500">
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/ledwiring/led2.png" width="500" height="500"><br>
  
### MOTION SENSOR:
In order to enable the motion sensor you will need to do the following:
* You will need 3 wires (female/male)
* You will need 2 resisitors
* You will need Adafruit 3 pin motion sensor (https://www.adafruit.com/product/4871) <br>
* You will need cardboard to cover the sides of the motion sensor due to high sensitivity
* Have the first wire connected to PIN40 on the P9 header. This pin is where the motion sensor output will be sent to via A2D
* Have 2 wires that are connected to the ground 
* Power wire connected to PIN3 on P9 header as above
* The motion sensor has 3 pins to it. The first pin is where the 2 resistors should meet, this motion sensor pin is for the power. The second motion sensor pin is where the output wire (yellow) is connected to. The third motion sensor pin is for grounding the motion sensor where the second wire (blue) is connected to ground. The last wire (brown) is where the second resisitor's end point is which also connects to ground. The pictures and diagram below should show how to do the wiring. 
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/motionSensorWiring/20230329_200606.jpg" width="500" height="500">
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/motionSensorWiring/20230329_200621.jpg" width="500" height="500">
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/motionSensorWiring/20230329_200638.jpg" width="500" height="500">
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/motionSensorWiring/motionsensor.png" width="500" height="500">
