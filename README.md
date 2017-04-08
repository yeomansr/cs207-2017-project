# Cyborg Headpiece (cs207-2017-project) [Version 1.3.2]
License:  [*Unlicense*](http://unlicense.org)

This is the repository for Richard Yeomans' CS 207 Project.

A cyborg headpiece, by my definition, is a piece of wearable technology affixed to the head of the wearer.  While I had originally intended to make the headpiece look as though it was integrated into the flesh of the wearer, I did not have enough time to finish it to this level of detail.

This class project has evolved a great deal from what I had originally intended.  At the beginning of the class, I did not really understand enough about what I would be able to do.  Now that the class is pretty well completed, this is the result of my work.

![Cyborg Headpiece][pic1]

[pic1]: https://github.com/yeomansr/cs207-2017-project/blob/master/img/20170407_160036.jpg "Cyborg Headpiece"

## Manifest
- **/doc** - technical documents (mostly PDF files)
- **/img** - image files (including Fritzing circuit diagrams)
- **/src** - source code file (.ino)
- **/LICENSE** - Unlicense file
- **/README.md** - this file


## Requirements and Materials
The following constitutes the materials I used in this project.  Many of these materials can be replaced with other similar materials as appropriate.
### Dependencies
- [add support for Adafruit to Arduino IDE](https://learn.adafruit.com/introducing-circuit-playground/arduino-1-dot-6-x-ide)
- [Adafruit NeoPixel Library](https://github.com/adafruit/Adafruit_NeoPixel) (not used, but helpful)

### Bill of Materials
- 1x [Adafruit Circuit Playground - Developer Edition](https://www.adafruit.com/products/3000)
- 1x [Adafruit NeoPixel Ring - 24 x 5050 RGB LED](https://www.adafruit.com/products/1586)
- 1x [Adafruit NeoPixel Shield for Arduino - 40 RGB LED Pixel Matrix](https://www.adafruit.com/products/1430)
- 1x [Adafruit USB cable - A/MicroB - 3ft](https://www.adafruit.com/product/592)
- 1x plain black visor
- 1x [Adafruit Premium Male/Male Jumper Wires - 40 x 6" (150mm)](https://www.adafruit.com/products/758)
- 1x [Adafruit Premium Female/Female Jumper Wires - 40 x 6"](https://www.adafruit.com/products/266)
- 1x [Adafruit JST-PH Battery Extension Cable - 500mm](https://www.adafruit.com/products/1131)
- 1x [Adafruit 3 x AAA Battery Holder with On/Off Switch and 2-Pin JST](https://www.adafruit.com/products/727)
- 3x Alkaline AAA batteries
- 1x [Adafruit Needle set - 3/9 sizes - 20 needles](https://www.adafruit.com/products/615)
- black thread
- clear nail polish
- black electrical tape
- wire cutters
- wire strippers


## Build Instructions
Ultimately, the components will be connected as per the following diagrams:

![Fritzing Diagram 1][pic2]

[pic2]: https://github.com/yeomansr/cs207-2017-project/blob/master/img/Project_Cyborg-Headpiece_bb.png "Fritzing Diagram 1"

![Fritzing Diagram 2][pic3]

[pic2]: https://github.com/yeomansr/cs207-2017-project/blob/master/img/Project_Cyborg-Headpiece_schem.png "Fritzing Diagram 2"

First, prepare the NeoPixel Shield and NeoPixel Ring by attaching some wires to the appropriate connectors:
1. Pull the Male/Male Jumper Wires apart, so that you are working with individual wires.
2. Take a black wire and cut it in half.
3. Strip about 3 cm off the wire end off both pieces (maintain the connector ends)
4. Wrap the stripped end of one of the wire pieces to the connector labeled *Ext.* on the NeoPixel Shield.  Continue wrapping it around several times.
5. Wrap the stripped end of one of the wire pieces to the connector labeled *GND* on the NeoPixel Ring.  Continue wrapping it around several times.
6. Place a dab of nail polish onto the front and back of both connectors, to seal the wires to the connectors.
7. Allow the nail polish to dry.
8. Trim any excess wire from the connectors, and make them neat.  Ensure you don't create any shorts while doing this.
9. Repeat steps 2 to 8, this time using a red wire, and connecting to *5V* on the NeoPixel Shield, and connecting to *PWR +5V* on the NeoPixel Ring.
10. Repeat steps 2 to 8, this time using a blue wire, and connecting to *Din* on the NeoPixel Shield, and connecting to *Data Input* on the NeoPixel Ring.

![NeoPixel Shield][pic4]

[pic4]: https://github.com/yeomansr/cs207-2017-project/blob/master/img/20170407_160055.jpg "NeoPixel Shield"

Attach the NeoPixel Shield and NeoPixel Ring to the visor using the black thread.  Simply sew the thread through any unused connector holes in the NeoPixel Shield to the side of the visor, allowing the end where the wires are not attached hang down.  When wearing the visor, the NeoPixel Shield should hang roughly over one of the wearer's ears.

![NeoPixel Ring][pic5]

[pic5]: https://github.com/yeomansr/cs207-2017-project/blob/master/img/20170407_160047.jpg "NeoPixel Ring"

To attach the NeoPixel Ring, simply sew thread around the ring, between the NeoPixels to the end of the visor.  It will be loose to begin with, but as you sew more loops of thread to the visor, it will stabalize.  Ensure the wires are going up and over the visor.  Place the ring approximately in front of one of the wearer's eyes, sort of like a monocle.

Hold the Circuit Playground against the visor in an appropriate place between the NeoPixel Shield and NeoPixel Ring.  It isn't critical it be in any exact place, but you will want to hold it in place to identify how long the wires will need to be to connect to both the NeoPixel Shield and NeoPixel Ring.
1. Pull the Female/Female Jumper Wires apart, so that you are working with individual wires.
2. Repeat steps 2 to 8 above, this time using the Female/Female wire pieces.  Cut them to appropriate lengths based on the position of the components.  Make the wire pieces a little longer than you need, so that the wires are not too tight when connected.
  - For connections to the NeoPixel Shield: black wire to *GND*, red wire to *VBATT*, blue wire to *Pin #6*
  - For connections to the NeoPixel Ring: black wire to *GND*, red wire to *VBATT*, blue wire to *Pin #3*

![Circuit Playground][pic6]

[pic6]: https://github.com/yeomansr/cs207-2017-project/blob/master/img/20170407_160104.jpg "Circuit Playground"

Sew the Circuit Playground to the visor in the same place you picked earlier.  Once it is secured to the visor, you may now connect the male and female ends of the wires to establish the connections between the Circuit Playground and the NeoPixel Shield and NeoPixel Ring.

Put batteries into the Battery Holder.  Find an appropriate place to attach the Battery Holder; use the electrical tape to attach it.  If the connector for the Battery Holder is too far from the Circuit Playground to connect, make use of the Battery Extension Cable, as I did.

Use black thread as appropriate to hold the wires nicely to the visor.


## Firmware Installation
In order to upload the code to the Circuit Playground, you will need to first install the Arduino IDE software.  Follow the instructions on Adafruit's website: *Arduino 1.6.x IDE* in the references.  Don't forget to add the *Adafruit Board Support* package, further down on the page.

Connect the Circuit Playground using the USB cable.  Load the code *sketch_project.ino* that can be found in the *src* directory.


## Usage
The Battery Holder has a power switch on it; use this switch to turn on or off the headpiece.

![Sound Reactor][pic7]

[pic7]: https://github.com/yeomansr/cs207-2017-project/blob/master/img/20170407_160514.jpg "Sound Reactor"

The NeoPixel Shield responds to sounds.  Talk or otherwise make some noises and you will see the NeoPixel Shield light up.

![Orientation Reactor][pic8]

[pic8]: https://github.com/yeomansr/cs207-2017-project/blob/master/img/20170407_160332.jpg "Orientation Reactor"

The NeoPixel Ring responds to the visor's orientation.  The 3 dots will rotate around the ring based on the X, Y, and Z orientations of the Circuit Playground.  Move it around to see the effects.

![Basic Operation][pic9]

[pic9]: https://github.com/yeomansr/cs207-2017-project/blob/master/img/20170407_160338.jpg "Basic Operation"

The Circuit Playground has some NeoPixels on it as well.  The dot will go around it to indicate that there is stuff going on.  Both buttons can be pressed and will change the behavior of the Circuit Playground dot.  Furthermore, the switch on the Circuit Playground will freeze all responses on all components when switched.


## Team
The project team consists of:
- [Richard Yeomans](mailto:yeomansr@uregina.ca)

It's a small team.


## Acknowledgements
### Credits
- Robin Canham - her assistance and support was invaluable
- Trevor Tomesh - CS 207 class instructor, provided insight and assistance all over the place

### References
- *Arduino 1.6.x IDE | Introducing Circuit Playground | Adafruit Learning System*.  (2017).  *Learn.adafruit.com*.  Retrieved 8 April 2017, from [https://learn.adafruit.com/introducing-circuit-playground/arduino-1-dot-6-x-ide](https://learn.adafruit.com/introducing-circuit-playground/arduino-1-dot-6-x-ide)
- *adafruit/Fritzing-Library*.  (2017).  *GitHub*.  Retrieved 8 April 2017, from [https://github.com/adafruit/Fritzing-Library](https://github.com/adafruit/Fritzing-Library)
- *Downloads | Introducing Circuit Playground | Adafruit Learning System*.  (2017).  *Learn.adafruit.com*.  Retrieved 19 March 2017, from [https://learn.adafruit.com/introducing-circuit-playground/downloads](https://learn.adafruit.com/introducing-circuit-playground/downloads)
- *Mastering Markdown · GitHub Guides*.  (2017).  *Guides.github.com*.  Retrieved 5 February 2017, from [https://guides.github.com/features/mastering-markdown/](https://guides.github.com/features/mastering-markdown/)
- Stern, B. & Burgess, P.  (2017).  *Cyberpunk Spikes | Make:. Make: DIY Projects and Ideas for Makers*.  Retrieved 5 February 2017, from [http://makezine.com/projects/make-42/cyberpunk-spikes/](http://makezine.com/projects/make-42/cyberpunk-spikes/)
- *The Magic of NeoPixels | Adafruit NeoPixel Überguide | Adafruit Learning System*.  (2017).  *Learn.adafruit.com*.  Retrieved 25 March 2017, from [https://learn.adafruit.com/adafruit-neopixel-uberguide/overview](https://learn.adafruit.com/adafruit-neopixel-uberguide/overview)
- Tomesh, T.  (2017).  *trevortomesh/OSHRepo*.  *GitHub*.  Retrieved 5 February 2017, from [https://github.com/trevortomesh/OSHRepo](https://github.com/trevortomesh/OSHRepo)


## Version History
version 1.3.2
- added pictures and diagrams

version 1.3.1
- fixing formatting errors in README

version 1.3.0
- updated description
- updated Manifest
- updated Requirements and Materials
- added Build Instructions
- added Firmware Installation
- added Usage
- added reference to Arduino 1.6.x IDE
- added reference to Adafruit Fritzing Library

version 1.2.0
- added reference to NeoPixel guide

version 1.1.5
- fixing formatting errors in README

version 1.1.4
- fixing formatting errors in README

version 1.1.3
- added reference to Adafruit downloads for Circuit Playground

version 1.1.2
- added manufacturer information to materials

version 1.1.1
- fixing formatting errors in README

version 1.1.0
- updates to bill of materials

version 1.0.3
- added important reference

version 1.0.2
- fixing formatting errors in README

version 1.0.1
- fixing formatting errors in README

version 1.0.0
- starting project

