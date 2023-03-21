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
|```Down``` |HH:MM (Current Time)|


### Once Alarm Sounds:
| Input              | Action             |
|:-------------------|:------------------:|
|```Any Direction``` |Snooze              |
|```Inwards```       |Shut off            |


### LEDS:
In order to enable the two wired LEDs please do the following:
1). Follow Brian Fraser's guide on setting up a device tree to enable our first (left) LED. Do this for only this one LED as the other LED, we will be driving the gpio pins instead which has already been done. Please go through for configuring the left LED:
     - Step 3. Device Tree: Enabling the LED (guide link: https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/guides/files/WiringAnLEDGuide.pdf)

2). For wiring up the LED: <br>
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/ledwiring/20230318_184828.jpg" width="500" height="500">
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/ledwiring/20230318_184838.jpg" width="500" height="500">
<img src="https://github.com/Kreutonz/cmpt433-project/blob/Sumeet/ledwiring/20230321_113237.jpg" width="500" height="500">