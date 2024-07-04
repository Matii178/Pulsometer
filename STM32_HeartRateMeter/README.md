# Heart Beat Measurement using STM32
![blood_pulse](https://github.com/Matii178/Pulsometer/assets/62108776/0a9ec920-e5a3-4d8e-a104-ca5c2e367701)

## Working Principle

The microcontroller used to measure output signal from constructed pulsometer is STM32F103RB. After turning on the device by pressing user button, it samples signal using ADC with sampling frequency equeal to 50 Hz (Resting human heart rate varies from 1 Hz (60 bpm) to 1.66 (100 bpm) with highest possible heart rate up to 3.66 Hz (220 bpm), this means that the lowest sampling frequency should be more than 7.32 Hz (Nyquist frequency). To maintain clear signal I decided to oversample it). The time length of last eight heart beats is measured, averaged, converted to bpm and then displayed using SWV ITM data console to a user. After user button is being pressed once again the device stops measuring the signal.

## Peripherals used

* ADC1 - samples heart signal.
* TIM3 - Set with 50 Hz frequency to tact ADC.
* TIM4 - Used as internal clock to measure time between two heart beats.
* GPIO - As a user button, used to turn on/off the device.
* NVIC - Used to controll interrupts caused by GPIO and ADC. 

## Code explanation

tbd.. 

## Result example
After filling the circular buffer every heart beat triggers data displaying. Below is an example of my heart rate measurement, I took a deep breath and exhaled it to speed up my heart rate, it is also visible in the example [gif below].

![ezgif com-video-to-gif-converter (1)](https://github.com/Matii178/Pulsometer/assets/62108776/e9d43915-f9a8-4ede-b088-eef4873f71fa)




