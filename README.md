# Jacob Springs Particle Control

This code that controls a walk-in fridge freezer was written for Jacob Springs Farm near Boulder Colorado.

For my first year engineering projects class my group and I partnered with Jacob Springs Farm to help them implement a walk-in freezer fridge.

The code is implemented using a Particle Argon device. The code takes in four temperatures from sensors scattered around the fridge and freezer. The device determines if it should activate the compressor and the fan to cool the room. The compressor and the fan are controlled using relays, which the argon device can control with it's output pins. 

When preforming cooling logic for both the compressor and the fan the code first checks for if there are overrides present. If there are overrides then the code will make sure the compressor and the fan are adhereing to those. If no overrides are present then the logic will activate the compressor and the 

If you are trying to replicate the project, or have any questions about the code and or the implemenation don't hesitate to reach out. 

sko buffs
