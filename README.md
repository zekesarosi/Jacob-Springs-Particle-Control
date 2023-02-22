# Jacob Springs Particle Control

This code that controls a walk-in fridge freezer was written for Jacob Springs Farm near Boulder Colorado.

For my first year engineering projects class my group and I partnered with Jacob Springs Farm to help them implement a walk-in freezer fridge.

The code is implemented using a Particle Argon device. The code takes in four temperatures from sensors scattered around the fridge and freezer. The device determines if it should activate the compressor and the fan to cool the room. The compressor and the fan are controlled using relays, which the argon device can control with it's digital output pins. 

If you want to compile the code yourself you have to include `project.properties`. 
`particle complie argon` will throw errors because it will try to complile both source files into one.
To compile the `src/implementationTemps.ino` you would in the particle-cli `particle compile argon src/implementationTemps.ino particle.properties`  

When preforming cooling logic for both the compressor and the fan the code first checks for if there are overrides present. If there are overrides then the code will make sure the compressor and the fan are adhereing to those. If no overrides are present then the logic will activate the compressor and the 

If you are trying to replicate the project, or have any questions about the code and or the implemenation don't hesitate to reach out. 

sko buffs

recently added (22 Feb 2023) was functionality for push notifications via IFTTT. 
also added was data logging to google sheets via api. GS code to be implemented on google side can be found in `jacobsprings/implementation/data-logging`

Downloading the precompiled binary for Feb 22, 2023
can be located in `jacobsprings/implementation/bin/`

sha256 checksum: ceff975b37e6d55b58869d897cec4d0fac2bf7c26d79447ada3a20d95cef9284

verify the binary before flashing it to your device or compile from source for most security
