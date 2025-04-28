# ESP32 SB

ESP32 SB is a firmware specifically developed for sound modules, designed to run on ESP32 microcontrollers. Its primary function is to manage the autonomous operation of each module, coordinating the various integrated hardware components.

This firmware handles the following tasks:

- Sensor Data Acquisition: Reads data from capacitive and inertial sensors (accelerometer and gyroscope).

- Real-Time Sound Generation: Produces sound via digital synthesis or audio playback from a microSD card.

- Data Transmission and Reception: Communicates over Wi-Fi, both between modules and with a central computer or external networks.

- Hot Configuration Updates: Allows dynamic behavior modifications without requiring a restart.

The code is developed in the Arduino environment, using optimized libraries for real-time processing and wireless communication. Its modular structure makes it easy to adapt the firmware to new hardware versions or specific artistic requirements.

Authors: Sabrina García, Laurence Bender, Germán Ito
