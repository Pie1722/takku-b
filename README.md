# takku:b

<img width="1280" height="720" alt="github intro" src="https://github.com/user-attachments/assets/f415e720-7f43-49c3-abe8-44d24a5854e3" />

<p align="justify">
Most DIY binary watches sacrifice battery life for simplicity. I wanted to build one that could run for months from a CR2032 while still remaining compact and stylish. Using an STM32L0 microcontroller and aggressive power optimization, this watch sleeps at under 10 µA and only wakes when needed.
</p>
<p align="justify">
This watch consumes very little amount of power and thus it can run forever on the CR2032 battery but of course due to its self discharge, it will last for around 8-10 years. Along with time it shows weekday, date, month, year and you can even set alarms!
</p>

---

## Features

  1. Time (HH:MM)
  2. Weekday and Date
  3. Month and Year
  4. Set Alarm A and B
  5. Long lasting battery life "**4+ years**"
  6. Field watch style strap "**22mm NATO**"

## Reading Time

<img width="1280" height="720" alt="reading time" src="https://github.com/user-attachments/assets/d24877e2-714b-40f4-9713-630a59409227" />

<p align = "justify">
Reading the time in this watch is very easy. Just as the name suggests, it shows time in binary, more specifically it shows in (BCD) Binary Coded Decimal. BCD is a type of encoding in which each number is represented by fixed number of bits. So to show time we need 4 number in which two are for hours and then the other two are for minutes. Each binary digit has its own weight. The bottom digit represents 1 and the top most represents 8. So we need 16 LEDs to show the time (4 rows and 4 columns).
</p>
<p align = "justify">
So to read the time just add the digits in which the LED is turned on and ignore the one's which are off. See the above figure for better understanding.
</p>

---

## How To Use?

<img width="1280" height="720" alt="wach label" src="https://github.com/user-attachments/assets/b6505ead-afb1-4596-b570-f9c57307259a" />

**Note**: See the above figures for the naming conventions.

**General**

  1. Press the button (A) to turn on the display and see the time.
  2. To view Weekday and Date press the button (A) again. First two columns shows the Weekday in which Monday is represented by 1 and Sunday as 7 and the last two columns shows Date.
  3. To view Month and Year press the button (A) again. First two columns shows the Month and last two columns shows the year.
  4. This watch goes on in this cyclic way with each button press. First it shows Time -> Weekday & Date -> Month & Year. Time is represented with a beep to avoid confusion.
  5. The display remains on for 5 seconds. To change the display timeout see the Step 5.

**Setting Mode**

  1. To enter setting mode press the button (A) once to turn on the watch and got to the specific screen then keep pressing button (A) until you hear a beep which indicated you are in setting mode.
  2. To set the time press the button (A) and and enter the time screen and then keep pressing the button (A) until you hear a beep.
  3. Once inside setting mode, press button (B) to increment the numbers.
  4. To change the current field inside the setting mode then press button (C) once. Example - After setting the Hour then press button (C) and set the minutes. If the button (C) is pressed again it will go again to the hour setting mode.
  5. To exit the setting mode, keep pressing the button (A) until you hear a beep.
  6. For setting the other parameter of the watch go to that specific screen and follow the above instruction.

**Setting Alarm**

  1. To view the current alarm, press button (C) after turning the watch on.
  2. To turn on the alarm press the button (B) and same to turn it off. The alarm turned on will be indicated by a red led.
  3. To edit the alarm, press button (A) for one second until you hear the beep, to enter alarm editing mode.
  4. Then use button (B) to increment the value.
  5. To change the field between hour and minute, press button (C).
  6. To exit the alarm edit mode press the button (A) for one second till you hear the beep.
  7. Then either turn on or turn off the alarm by pressing the button (B).
  8. To stop the alarm after it is triggered at the set time, press button (A).
     
  **Note**: The alarm time is stored even if the battery is removed. But it wont be enabled, to turn on the alarm, enter the alarm mode and press the button (B) to turn it on.

**Flow Of Screen**

    Normal Mode -> Screen Time -> Screen Weekday_Date -> Screen Month_Year

---

# Hardware

## PCB Design

The PCB is fully open-sourced and is designed in EasyEDA standard edition. The PCB size is 36mm * 36mm. You can download the gerber files for PCB manufacturing from [/Gerber](Gerber/)

<img width="2923" height="2070" alt="Schematic_Binary-Watch_2026-06-24" src="https://github.com/user-attachments/assets/9cc99fe4-90c3-4f41-ba16-6e1910dd7919" />

<img width="1365" height="720" alt="image" src="https://github.com/user-attachments/assets/fde857bf-34e4-4fa5-bb56-8cfecddabd90" />

<img width="1280" height="720" alt="3d" src="https://github.com/user-attachments/assets/1cf8ab6b-2a96-42d4-92be-b7599bb9de22" />


## CAD Design

<img width="800" height="600" alt="binarywatch-ezgif com-video-to-gif-converter" src="https://github.com/user-attachments/assets/756d55a9-d5e9-4fac-a5ac-96491b0de56d" />

<img width="1280" height="720" alt="cad" src="https://github.com/user-attachments/assets/c003a56f-3511-40b2-9c8c-b3589d257d84" />

---
