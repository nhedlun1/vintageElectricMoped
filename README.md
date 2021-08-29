# vintageElectricMoped
The story of how a vintage moped got a new electrified life! 

Almost two years ago i got the idea to convert an old 70's two-stroke moped to electric power. 
Now, two years later, and not too many iterations of my ideas it's almost finished!

Basically i fitted a electric skateboard motor inside the old two-stroke motor, leaving the 2-speed gearbox.
I chopped off the old crankshaft axle and drilled a hole in the shaft so that the electric motor shaft
would fit directly onto it. I then removed enough material from one of the motor blocks so the eletric
motor would fit inside the motor case, a simple adapter plate in aluminium holds the BLDC motor in place. 

The electric motor is driven by a BLDC motor controller which offers a lot of settings and also CAN-communication.
I'm getting the throttle signal from the regular wire throttle handle, however i've modified the carburetor "slightly".
The throttle slide is now 3d printed and instead of a needle letting fuel inside the cylinder, there is now a magnet in it's place. 
This magnet affects the linear hall effect sensor which is mounted in the bottom of the carburetor, which in turn gives a analog 
signal based on the amount of flux it's picking up, this analog signal is read by the motor controller as the input throttle signal.

The battery is made up out of geniune 18650 Tesla cells salvaged from a model S, which are 4.2volts max and 3Ah each. 
There are 12 cells in series and 7 in parallel, a total of 84cells. Fully charged it's 50.4Volts and 21Ah, which gives 1kWh of power!
Making the battery was probably the most tedius job of the build, a part from the sanding and painting. All 84 cells are spot welded
together with pure nickel strip, and controlled and monitored by a BMS system, which lets you connect over Bluetooth to monitor 
the health and status of the battery. 
