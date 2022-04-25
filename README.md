# ESP32 Telstar client
A minimal Videotex client for connecting to [Telstar](https://glasstty.com/).

## Aspiration

A usable Videotex client running on ESP32, connecting over Wi-Fi, outputting composite video and controlled with a 3x4 matrix keypad for 'embedded' installation using an old TV etc. as a screen.

## Currently working features

- Configured with file credentials.h (see comment in sketch)
- Connects to the server and optionally requests 'fast' download of pages
- Parses most Telstar escape sequences and renders adequate pages in monochrome
- Navigation input on serial port
- Debug rendering output on serial port is mostly human readable showing control codes inline with text

## To be done

- Matrix keyboard connected to the ESP32
- Block graphic font for graphics
- Larger font for text
- Colour rendering of the composite video
- Filter navigation to ensure sending sensible requests to the servers

## Very broken features

- Form inputs, eg. the Weather page

## Optional features

- ANSI 'translation' so the Serial console that will render the page acceptably with colour etc. separately from the composite video

## How to build

- Should work with any classic ESP32 (not S1/S2/C3) development board
- Use Arduino IDE 1.8.x, not tested with 2.0.x. It may be worth a separate 'portable' installation because of the next requirement
- Install Espressif systems Arduino core v2.0.1 **other versions do not currently work** as the composite video output is a horrible hack and other versions may crash the board, refuse to set the I2S clock or just not work. Downgrade if necessary.
- Install the [composite video library](https://github.com/marciot/ESP32CompositeColorVideo) from [marciot](https://github.com/marciot).
- Install the [ESP32 Async TCP](https://github.com/me-no-dev/AsyncTCP/blob/master/src/AsyncTCP.h) library from [me-no-dev](https://github.com/me-no-dev).
- Install my [espBoilerplate library](https://github.com/ncmreynolds/espBoilerplate) for managing the Wi-Fi connection. You can easily remove this requirement from the code by commenting out line 13, there's some standard Wi-Fi connection code as well.

## Connecting the composite video

All you need is a connection from GPIO25 of the ESP32 to the 'tip' of the RCA composite video on a TV or monitor and ground to the 'shield' of the RCA composite video connector.

## Choosing PAL or NTSC

Even on nominal PAL devices, NTSC seems to work best. Set this in the top of the sketch by commenting out line 14.
