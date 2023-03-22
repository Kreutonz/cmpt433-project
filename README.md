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
| Input              | Action             |
|:-------------------|:------------------:|
|```Any Direction``` |Snooze              |
|```Inwards```       |Shut off            |


### LEDS:
In order to enable the two wired LEDs please do the following:
* Follow Brian Fraser's guide on setting up a device tree to enable our first (left) LED. Do this for only this one LED as the other LED, we will be driving the gpio pins instead which has already been done. Please go through this part of the guide for configuring the left LED:
     * Step 3. Device Tree: Enabling the LED (guide link: https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/guides/files/WiringAnLEDGuide.pdf)

* The two GPIO pins that have the wires connected on them are PIN23 and PIN27 both on the P9 headers. The power is outputting from PIN3 also on the P9 header. 
* For wiring up the LED: <br>
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/ledwiring/20230318_184828.jpg" width="500" height="500">
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/ledwiring/20230318_184838.jpg" width="500" height="500">
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/ledwiring/20230321_113237.jpg" width="500" height="500">
  
