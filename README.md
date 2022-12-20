# room-lights

A project to control NeoPixel-compatible fairy lights over WiFi.

## Overview

I was inspired to make this project when I was browsing the [Adafruit](https://www.adafruit.com/) website and found [these adorable LED fairy lights](https://www.adafruit.com/product/4917). Although these LEDs are fixed-address and aren't techically NeoPixels, 100 LEDs is enough and I don't need to chain multiple strands. They're probably also cheaper this way.

To control the lights, I decided to go for a [Particle Photon](https://docs.particle.io/photon/), since I had a couple lying around and Particle Cloud's over-the-air firmware updates are just so convienient. However, the same concepts can be applied to other microcontrollers such as an ESP32 or Raspberry Pi Pico W.

## Hardware

### Parts

- [Particle Photon](https://docs.particle.io/photon/)
- [Fixed-Address NeoPixel LED Fairy Lights](https://www.adafruit.com/product/4917)
- Capacitors
- Male/Female Jumper Wires

The physical build is very straightforward: connect the LEDs to the Photon. [Adafruit's NeoPixel guide](https://learn.adafruit.com/adafruit-neopixel-uberguide/powering-neopixels) recommends attaching a 500-1000µF capacitor to the power line for stability, so I wired up 5 1000µF capacitors in parallel.

The LEDs are powered off the Photon's VIN pin, which acts as a 4.8V output when powered by USB. However, this pin will only supply up to 1A, which is not enough to run all the LEDs at full brightness. You could get around this by powering the LEDs from the power supply directly, but I didn't bother.

To control the LEDs, I used pin D6.

## Firmware

Although Particle Cloud makes it really easy to expose cloud functions or variables to control the Photon, I didn't want to use the cloud to control something on the local network. Instead, the Photon serves a very simple HTTP API on the local network. A separate web interface can then make requests to the API to control the lights. The separation of the interface and the API allows for the use of modern web frameworks, which would be difficult for the Photon to serve.

To flash the firmware to the Photon, you can use the [Particle CLI](https://docs.particle.io/getting-started/developer-tools/cli/). Once installed, log into your account with `particle login`. Clone this repo, then `cd` into it. If your Particle is connected to the cloud, you can flash it over-the-air with `particle flash <photon name>`.

## Web Interface

For simplicity, the web interface is a [widget in my personal utility site](https://tools.tomichen.com/lights/) built with [SvelteKit](https://kit.svelte.dev/). I already had the site set up, and it also had a color picker component I made for the [color contrast picker](https://tools.tomichen.com/color-contrast-picker/).

The [source for the web interface](https://github.com/tctree333/tools/blob/main/src/routes/lights/%2Bpage.svelte) is available on GitHub. To set the correct IP address, use the URL hash. For example, if my Photon's local IP is `192.168.1.10`, you can use `/lights/#192.168.1.10`. You may need to reload the page.

## Future Improvements

More lighting effects would be nice, since I only have a couple programmed in right now. I also think it would be really cool to have an alarm feature where it turns on the lights in the morning when it's dark to wake me up.
