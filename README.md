# Transitions: A Generative Four-Season Tree
**Transitions** is a generative animation that visualizes the passage of time through a single tree. Over a 60-second cycle, the tree moves through spring, summer, autumn, and winter. While the structure of the tree remains constant, elements such as leaves and snow are generated with randomized properties, making each cycle slightly different from the last.

## Project Demo

https://github.com/user-attachments/assets/8a48e132-4e04-4dd2-9100-3c42677b5e5f

## Recreation

### What You Need

- LilyGo T-Display ESP32
- USB-C cable
- Computer with the [Arduino IDE](https://www.arduino.cc/en/software)
- [`TFT_eSPI`](https://github.com/Bodmer/TFT_eSPI) library by Bodmer

### Setup

1. Download and install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Add ESP32 support to the Arduino IDE by following the [Arduino-ESP32 installation guide](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).
3. Install the `TFT_eSPI` library:
   - Open **Tools > Manage Libraries** in the Arduino IDE.
   - Search for `TFT_eSPI`.
   - Install `TFT_eSPI` by Bodmer.
4. Configure `TFT_eSPI` for the LilyGo T-Display:
   - Locate the `TFT_eSPI` library folder in your Arduino libraries.
   - Open `User_Setup_Select.h`.
   - Comment out:
     `#include <User_Setup.h>`
   - Uncomment:
     `#include <User_Setups/Setup25_TTGO_T_Display.h>`
5. Connect the LilyGo T-Display ESP32 to your computer with a USB-C cable.
6. In the Arduino IDE, select the appropriate ESP32 board and serial port under **Tools**.

### Running the Project

1. Clone or download this repository.
2. Open `Transitions/Transitions.ino` in the Arduino IDE.
3. Verify the sketch to make sure it compiles successfully.
4. Upload the sketch to the LilyGo T-Display ESP32.
5. After the upload finishes, the animation will begin automatically and continuously cycle through the four seasons.

## Project Blog

For more about the concept, design decisions, and development process behind **Transitions**, visit my [Project Blog](https://plump-line-d3b.notion.site/Module-1-TFT-Display-3dc28368cff9805dbe46eb8cc6d7e909?pvs=74).
