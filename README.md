A Bluetooth adapter for the IBM Model M PS2,
The adapter converts the PS2 signals from the keyboard to HID signals that are sent over bluetooth to a computer. 

**Project goals:**
  
When I started this project I wanted to create a wireless way for my IBM model M keyboard to connect to my apple computer. When I use the adapter I bought of amazon, my macbook thought my keyboard was a barcode reader which was not ideal.

**Project iterations:**
  
My first attempt on this project was to replace the controll board on my keyboard entirly with a custom solution, this approach had a few issues though. The main advantage of this method was power efficency, the giant microcontroller in the Model M is super old and eats energy, when plugged into my desktop it doesn't cause a problem, but on a limited battery this could be an issue.
  
  <img width="1162" height="1548" alt="image" src="https://github.com/user-attachments/assets/fb792cf8-607d-4a84-bc80-c35942a87bc6"      style="width:25%; height:auto;"/>
  
After some iniatal testing I choose to scrap this idea for a couple reasons. First, I needed to disassemble the keyboard everytime I wanted to work on my adapter, then re-assemble the keyboard every time I wanted to use it. Second, the membraine requires 26 GPIO ports to interface with the matrix, this was do-able with a I2C GPIO extender, but it made my bread-borded prototype very difficult to debug as each connection was a new point of failure. Finally I needed to use these terrible membrain connectors which make me want to jump.
  <img width="1162" height="684" alt="image" src="https://github.com/user-attachments/assets/281c3934-a48e-44d8-9f98-18a39830995e" style="width:25%; height:auto;"/>

My second iteration was creating an adapter that would plug into the Model M's PS2 output. This solution was very conveinient because I could cut one off one of the heads of the adapter I already had, and directly break out the wiring from that. This solution would be easy to wire and test because the PS2 standerd only has 4 wires: Data, Clock, 5V, and GND.
  
  <img width="1106" height="1514" alt="image" src="https://github.com/user-attachments/assets/3b665f21-dc4f-45f4-b874-ffda703eee45" style="width:25%; height:auto;"/>
  <img width="1600" height="1548" alt="image" src="https://github.com/user-attachments/assets/49bc11cf-7210-4cac-a5da-6700cc1a4665" style="width:25%; height:auto;"/>

For this project I choose to use the adafruit Feather ESP32. I chose this microcontroller because it had built in bluetooth and a  built in battery circut. It was also only like 8 dollars so very nice for my budget. One problem with my choice of micro controller which I didn't realize when I picked it was that the Model M and the PS2 standerd need 5V input, however the feather only supplys 3.3V. Because of this as of now, I break the power for the PS2 out directly of the 5V line from the USB in, this means the adapter for now needs to be plugged into a direct power source because the battery cannot supply 5V like usb. In the future, I am going to add a voltage booster for the 3.3V line but it hasn't arrived yet. Trying to read 5V on the Feather will destroy the microcontroller, so for the Data and Clock lines I use voltage dividers. they consist of 3 resistors all of the same resistance, with two wired in series. 
  
  <img width="1162" height="1548" alt="image" src="https://github.com/user-attachments/assets/8acb4d83-12d7-4a02-a896-2a17bf06d497" style="width:25%; height:auto;"/> 
  <img width="1162" height="1548" alt="image" src="https://github.com/user-attachments/assets/39bac22a-8372-4f2a-a254-4f36f4ea87f7" style="width:25%; height:auto;"/>

**Software:**

My code uses 3 Libraries, PS2KeyAdvanced to read the serial output from the keyboard, BleKeyboard to send hid commands via bluetooth, and PS2keyMap to map the output from PS2KeyAdvnaced to the proper HID commands. PS2KeyMap works about 90% but the last 10 percent I needed to key by key print the output from PS2KeyAdvnaced then map it manaualy through a switch statement. My code also reports the battery life of the keyboard. The adafruit feather maps the batterys voltage through in an internal divider to analog pin 13 or GPIO 35 so to read the voltage, I can read the GPIO 35 and multiply it by two.

**Problems with my Implementation**

One issue with my implementation is that the lock keys (Caps Lock, Scroll Lock, Num Lock) all don't light up their respective LEDs. At first pressing the keys froze the keyboard, so I had to got into the PS2AdvanceKey library and remap the keys to ones not on my keyboard,
then in my code I can map the keys myself.
