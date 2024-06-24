# Pulsometer

## Schematic
### Parts of schematic were designed based on NXP "Pulse Oximeter Fundamentals and Design" by Santiago Lopez. Link below.

https://www.nxp.com/docs/en/application-note/AN4327.pdf

KiCad full scale electronic scheme. Below I will describe each part and its function.
![image](https://github.com/Matii178/Pulsometer/assets/62108776/59c3f091-2fa5-4775-a320-713e405176e0)

### Part_1: Power supply

* U1 - TL071 : Is a core of this part, its aim is to create virtual ground equal to 0.5 of Vcc.
* R1, R2 - Same values of these resistors to create 0.5 of Vcc. 
* C1, C2 - Same values, ensure stability of virtual gnd.
* H8, H9 - Inputs for external power supply.
![image](https://github.com/Matii178/Pulsometer/assets/62108776/a2e76fdf-27c5-4052-8269-b8cfc732db69)

### Part_2 Signal receiver with custom amplification

* H1, H2 - Connections for external red LED
* H3, H4 - Connections for external photodiode. It needs to be reverse biased.
* R4 - Thanks to current created by photodiode it enables 'interchanging' current to voltage. (Simple Ohm law)
* C3, R5 - It creates high-pass filter, eliminating frequencies below 0.53 Hz (approx. 32 heartbeats per minute). C3 is important to eliminate offset of signal, and R5 is used to discharge C3.
* U2A - TLC272 : Voltage follower. Used to eliminate influence of high resistance R5 on op-amp input due to its strong amplification.
* U2B - TLC272 : Non-inverting amplifier with custom amplification due to potentiometer.
* R6, R7, RV1 - Used with U2B TLC272 as parts of non-inverting aplifier. Enables to amplify signal 39-122 times.

![image](https://github.com/Matii178/Pulsometer/assets/62108776/80064c45-7169-4e76-85dc-b7e891a1b709)

### Part_3 Filters section

* R8, C4 - Low-pass filter, cuts off frequencies above 5.3 Hz.
* R9, R10, R11, C5, C6, C7 - Notch filter, eliminates 50 Hz frequency influence.
* C8, R12 - High-pass filter, cuts off frequencies below 0.5 Hz.

You may notice, that filter section is a filter with bandwidth starting from 0.5 Hz and ending on 5.3 Hz.
When converted to bpm, it heart rate signal starting from 32 - 318 bpm. Which is even more than enough for our purpouse.
![image](https://github.com/Matii178/Pulsometer/assets/62108776/c758b0cf-175f-45da-8030-7acf88ac5799)

### Part_4 output

* U3A - TLC272: Voltage follower, just as previously mentioned with U2A.
* U3B - TLC272: Non-inverting amplifier. Evens losses originating form filter section.
* R13, R14 - Parts of non-inverting amplifier U3B.
* R15, C9 - Once again low-pass filter (eliminates frequencies above 8 Hz), and also stabilizer for future ADC measurements.
* JP1, R16, R17 - Jumper with voltage divider, its aim is to ensure that even if device is powered with 5 V, it still can be connected to STM32 (ADC only supports maximal value of 3.3 V)
![image](https://github.com/Matii178/Pulsometer/assets/62108776/6c132628-2718-40ce-b9f7-8abe6150cc67)

### PCB board layout

Visualisation of my pcb board was created using KiCad 7.0. The front and bottom parts are visible below.
Size of the board is: 6.3 x 4.1 cm.

![image](https://github.com/Matii178/Pulsometer/assets/62108776/01aefd0d-650c-49ba-926f-12e461ec67ff)

![image](https://github.com/Matii178/Pulsometer/assets/62108776/e0fedf9e-b99d-4d74-b492-418e550ab545)

### Photo of assembled device 

I have ordered ready to solder boards from China, but unfortunatelly I would have to wait more than one month for them, so I decided to etch them on my own. 
It is one-sided board, with 'bottom' traces created with external wires (yellow ones).

![image](https://github.com/Matii178/Pulsometer/assets/62108776/1d74299a-2f7c-48bd-9be5-3d8a11962031)

### Real life experiment

In the end I have powered my pulsometer with 3.3V (So it is compatible with STM32). I have not soldered the jumper, and I have directly connected the ouputs to oscilloscope.
Below you can see the analogue signal of my pulse. To make sure it works, I have also measured my pulse with another method (simple metronome) to compare the results. It was pretty the same :)

![image](https://github.com/Matii178/Pulsometer/assets/62108776/3532decd-4453-4305-bd9f-3213bf647ab9)
