This project aimed to create a dual-axis solar tracker for use in a lab environment.

Dual axis solar trackers are useful in real-world applications where efficiency of solar panels is particularly important. This includes areas where sunlight is plentiful, and can provide meaningful electricity to communities around that area.

For this demo project, I used the following materials:
 - Arduino UNO
 - 4x 10k Ohm resistors
 - 4x Light Dependent Resistors (LDRs)
 - small solar panel
 - 5V Stepper Motor
 - 6V DC motor
 - Adafruit motor shield

The wiring was set up in the following way:
<img src="/assets/CircuitDiagram_D1.png" alt="circuit diagram 1"/>

Since I used 4 LDRs, each photoresistor was set up like so:
<img src="/assets/CircuitDiagram_D2.png" alt="circuit diagram 2"/>

For the code of the project, the following control flow diagram was used in a while loop to ensure that the average resistance value was achieved across all 4 LDRs:
<img src="/assets/ControlFlowDiagram.png" alt="control flow diagram"/>
