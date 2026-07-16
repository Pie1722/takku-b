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


