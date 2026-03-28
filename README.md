# IBM Model M Bluetooth Adapter

A Bluetooth adapter for the IBM Model M PS/2 keyboard. The adapter converts PS/2 signals from the keyboard to HID signals that are sent over Bluetooth to a computer.

<img width="678" height="1548" alt="image" src="https://github.com/user-attachments/assets/d4fe22b2-d67e-4fff-8d34-20a02ad64e25" style="width:25%; height:auto;" />

---

## Project Goals

When I started this project I wanted to create a wireless way for my IBM Model M keyboard to connect to my Apple computer. When I use the adapter I bought off Amazon, my MacBook thought my keyboard was a barcode reader, which was not ideal.

---

## Project Iterations

### Iteration 1 — Custom Controller Replacement

My first attempt was to replace the control board on my keyboard entirely with a custom solution. The main advantage of this method was power efficiency — the giant microcontroller in the Model M is very old and eats energy. When plugged into a desktop this doesn't cause a problem, but on a limited battery it could be an issue.

<img width="1162" height="1548" alt="image" src="https://github.com/user-attachments/assets/fb792cf8-607d-4a84-bc80-c35942a87bc6" style="width:25%; height:auto;"/>

After some initial testing I chose to scrap this idea for a few reasons:

- I needed to disassemble the keyboard every time I wanted to work on the adapter, then reassemble it every time I wanted to use it.
- The membrane requires 26 GPIO ports to interface with the matrix. This was doable with an I2C GPIO extender, but it made the breadboarded prototype very difficult to debug since each connection was a new point of failure.
- It required using terrible membrane connectors.

<img width="1162" height="684" alt="image" src="https://github.com/user-attachments/assets/281c3934-a48e-44d8-9f98-18a39830995e" style="width:25%; height:auto;"/>

---

### Iteration 2 — PS/2 Adapter

My second iteration was creating an adapter that plugs into the Model M's PS/2 output. This was very convenient because I could cut one head off an existing adapter and directly break out the wiring. This solution is easy to wire and test because the PS/2 standard only has 4 wires: Data, Clock, 5V, and GND.

<img width="1106" height="1514" alt="image" src="https://github.com/user-attachments/assets/3b665f21-dc4f-45f4-b874-ffda703eee45" style="width:25%; height:auto;"/>
<img width="1600" height="1548" alt="image" src="https://github.com/user-attachments/assets/49bc11cf-7210-4cac-a5da-6700cc1a4665" style="width:25%; height:auto;"/>

For this project I chose to use the **Adafruit Feather ESP32**. I chose this microcontroller because it has built-in Bluetooth and a built-in battery circuit, and it was only around $8 — great for my budget.

One problem I didn't realize when picking it was that the Model M and the PS/2 standard require 5V input, but the Feather only supplies 3.3V. Because of this, I currently pull power for the PS/2 directly from the 5V USB line, which means the adapter needs to be plugged into a power source for now since the battery cannot supply 5V. In the future I plan to add a voltage booster for the 3.3V line, but it hasn't arrived yet.

Since reading 5V on the Feather would destroy the microcontroller, I use **voltage dividers** on the Data and Clock lines. Each divider consists of 3 resistors of equal resistance, with two wired in series.

<img width="1162" height="1548" alt="image" src="https://github.com/user-attachments/assets/8acb4d83-12d7-4a02-a896-2a17bf06d497" style="width:25%; height:auto;"/>
<img width="1162" height="1548" alt="image" src="https://github.com/user-attachments/assets/39bac22a-8372-4f2a-a254-4f36f4ea87f7" style="width:25%; height:auto;"/>

---

## Software

The code uses 3 libraries:

- **PS2KeyAdvanced** — reads the serial output from the keyboard
- **PS2KeyMap** — maps PS2KeyAdvanced output to proper HID commands
- **BleKeyboard** — sends HID commands via Bluetooth

PS2KeyMap works for about 90% of keys. For the remaining 10%, I manually printed the output from PS2KeyAdvanced key by key and mapped them through a switch statement.

The code also reports battery level. The Adafruit Feather maps the battery voltage through an internal divider to analog pin 13 (GPIO 35), so to read the voltage I read GPIO 35 and multiply by two.

---

## Known Issues

The lock keys (Caps Lock, Scroll Lock, Num Lock) do not light up their respective LEDs. At first, pressing these keys froze the keyboard entirely. To fix this, I went into the PS2KeyAdvanced library and remapped the lock keys to keys not present on my keyboard, then handled them manually in code.
