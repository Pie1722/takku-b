# takku:b

<table>
<tr>
  <td width="75%">
    <img src="https://github.com/user-attachments/assets/858ae882-b4ef-4566-849a-3a6c6d4e0c5a" width="100%" alt="Binary Watch"/>
  </td>
  
  <td width="25%">
    <img src="https://github.com/user-attachments/assets/9bf011f8-855a-4db2-b611-9f7df4758a09" width="100%" alt="Glowing Watch"/> 
  <br><br>
    <img src="https://github.com/user-attachments/assets/1bdf2bfd-137a-479c-99f5-f7c3d789af99" width="100%" alt="PCB Front"/>
  <br><br>
    <img src="https://github.com/user-attachments/assets/37320292-618f-4576-a9f8-481ae585b982" width="100%" alt="PCB Back"/>
  </td>
  
</tr>
</table>

<p align="justify">
Most DIY binary watches sacrifice battery life for simplicity. I wanted to build one that could run for months from a CR2032 while still remaining compact and stylish. Using an STM32L0 microcontroller and aggressive power optimization, this watch sleeps at under 10 µA and only wakes when needed.

This watch consumes very little amount of power and thus it can run forever on the CR2032 battery but of course due to its self discharge, it will last for around 8-10 years. Along with time it shows weekday, date, month, year and you can even set alarms!
</p>

---

# Reading Time

<table>
<tr>
<td width="50%">
<img src="https://github.com/user-attachments/assets/4fd782c9-28a5-4d55-b54c-54e120cde544" width="100%" alt="Image 1">
</td>

<td width="50%">
<img src="https://github.com/user-attachments/assets/630f5f64-f013-440f-9885-0cf345753074" width="100%" alt="Image 2">
</td>
</tr>
</table>

<p align = "justify">
Reading the time in this watch is very easy. Just as the name suggests, it shows time in binary, more specifically it shows in (BCD) Binary Coded Decimal. BCD is a type of encoding in which each number is represented by fixed number of bits. So to show time we need 4 number in which two are for hours and then the other two are for minutes. Each binary digit has its own weight. The bottom digit represents 1 and the top most represents 8. So we need 16 LEDs to show the time (4 rows and 4 columns).

So to read the time just add the digits in which the LED is turned on and ignore the one's which are off. See the above figure for better understanding.
</p>

---

# How To Use?

<table>
<tr>
<td width="50%">
<img src="https://github.com/user-attachments/assets/780ba0ed-b3f8-4b78-84a9-861c173d9f51" width="100%" alt="Image 1">
</td>

<td width="50%">
<img src="https://github.com/user-attachments/assets/89ce1c6d-9a3b-473e-b5e2-33918f8d4284" width="100%" alt="Image 2">
</td>
</tr>
</table>

<p align = "jusify">
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
</p>
