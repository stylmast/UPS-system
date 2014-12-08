UPS-system

This is a test rig that is suppling power for my desktop & HA  computer systems. It will be upgraded to 24 Vdc at a later time.
Current spec's - 12 Vdc system, battery bank is 600Ah, charger is 30 A, feeding a 1Kw inverter.
Solar & wind chargering will be added at a later date.

I'm using arduino Ten from Freetronics to handle all of the data monitoring and some control.
System voltage is being monitored via a voltage divder with input into A0.
Battery current monitoring via a Allegro hall effects sensor, 50A Bi directional input A1.
Charger current monitoring via a Allegro hall effects sensor, 50A Uni directional input A2.
Inverter current monitoring via Allegro hall effects sensor, 200A Uni directional input A3.

All input to be displayed via Ajax gauges on various devices as req'd.

All sensors will be mounted in a metal enclosure to cut down interference, there will also be a DS18B20 sensor to monitor enclosure temp.

Future additions - monitoring of the Ahrs left in the bank, Solar panel charging 50A plus, to allow of the overnight recovery charge and also to maintain day time load on system. Load will be added to and charge will also be adjusted to cater for any axtra  requirements.
