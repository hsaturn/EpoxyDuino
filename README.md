# UnixHostDuino

This directory contains a small (but often effective) implementation of the
Arduino programming framework for Linux and MacOS. Originally, it was created to
allow [AUnit](https://github.com/bxparks/AUnit) unit tests to be written,
compiled and run on a Linux or MacOS machine, instead of running on the embedded
microcontroller. As more Arduino functionality was added, it became useful for
other Arduino programs, particularly ones that relied on just the `Serial`
interface.

The build process uses [GNU Make](https://www.gnu.org/software/make/manual/).
A `Makefile` needs to be created inside the sketch folder. For example, if the
sketch is `SampleTest/SampleTest.ino`, then the makefile should be
`SampleTest/Makefile`. The unit test is compiled with just a `make` command. It
produces an executable with a `.out` extension, for example, `SampleTest.out`.

Running an Arduino program natively on Linux or MacOS has some advantages:

* The development cycle can be lot faster because the compilers on the the
  desktop machines are a lot faster, and we also avoid the upload and flash
  process to the microcontroller.
* The desktop machine can run unit tests which are too much flash or too
  much memory to fit inside an embedded microcontroller.

The disadvantages are:

* Only a limited set of Arduino functions are supported (see below).
* There may be compiler differences between the desktop and the embedded
  environments (e.g. 8-bit integers versus 64-bit integers).

## How to Use

See the [BlinkSOS](examples/BlinkSOS) example project.
The minimal `Makefile` has 3 lines:
```
APP_NAME := {name of project}
ARDUINO_LIBS := {list of dependent Arduino libraries}
include {path/to/UnixHostDuino.mk}
```

To build the unit test, just run `make`:
```
$ cd examples/BlinkSOS
$ make clean
$ make
```

The executable will be created with a `.out` extension. To run it, just type:
```
$ ./BlinkSOS.out
```

The output that would normally be printed on the `Serial` on an Arduino
board will be sent to the `STDOUT` of the Linux or MacOS terminal. The output
should be identical to what would be shown on the serial port of the Arduino
controller.

### Difference from Arduino IDE

There are a number of small differences compared to the programming environment
provided by the Arduino IDE:

* The `*.ino` file is treated like a normal `*.cpp` file. So it must have
  an `#include <Arduino.h>` include line at the top of the file. This is
  compatible with the Arduino IDE which automatically includes `<Arduino.h>`.
* The Arduion IDE supports multiple `ino` files in the same directory. (I
  believe it simply concontenates them all into a single file.) UnixHostDuino
  supports only one `ino` file in a given directory.
* The Arduino IDE automatically generates forward declarations for functions
  that appear *after* the global `setup()` and `loop()` methods. In a normal
  C++ file, these forward declarations must be created by hand. The other
  alternative is to move `loop()` and `setup()` functions to the end of the
  `ino` file.

Fortunately, the changes required to make an `ino` file compatible with
UnixHostDuino are backwards compatible with the Arduino IDE. In other words, a
program that compiles with UnixHostDuino will also compile under Ardunio IDE.

### Conditional Code

If you want to add code that takes effect only on UnixHostDuino, you can use
the following macro:
```C++
#if ! defined(ARDUINO)
  {code for UnixHostDuino}
#endif
```

If you need to target Linux or MacOS specifically, I have found that the
following works for Linux:
```C++
#if defined(__linux__)
  ...
#endif
```
and the following works for MacOS:
```C++
#if defined(__APPLE__)
  ...
#endif
```

### Additional Arduino Libraries

The dependency to the `AUnit` library is always included by default.
If the unit test depends on additional Arduino libraries, they must be specified
in the `Makefile` using the `ARDUINO_LIBS` parameter. For example, this
includes the`[AUnit](https://github.com/bxparks/AUnit) library if it is
at the same level as UnixHostDuino::

```
APP_NAME := BlinkSOSTest
ARDUINO_LIBS := AUnit
include ../UnixHostDuino/UnixHostDuino.mk
```

The libraries are referred
to by their base directory name (e.g. `AceButton`, or `AceRoutine`) not the full
path. The `UnixHostDuino.mk` file will look for these additional libraries at
the same level as the `UnixHostDuino` directory itself. (We assume that the
additional libraries are siblings to the`UnixHostDuino/` library). This search
location can be changed by the user using the `ARDUINO_LIB_DIR` environment
variable. If this is set, then `make` will use this directory to look for the
additional libraries, instead of sibling directories of `UnixHostDuino`.

## Supported Arduino Features

The following functions and features of the Arduino framework are implemented:

* `Arduino.h`
    * `setup()`, `loop()`
    * `delay()`, `yield()`
    * `millis()`, `micros()`
    * `digitalWrite()`, `digitalRead()`, `pinMode()` (empty stubs)
    * `HIGH`, `LOW`, `INPUT`, `OUTPUT`, `INPUT_PULLUP`
* `StdioSerial.h`
    * `Serial.print()`, `Serial.println()`, `Serial.write()`
    * `Serial.read()`, `Serial.peek()`, `Serial.available()`
    * `SERIAL_PORT_MONITOR`
* `WString.h`
    * `class String`
    * `class __FlashStringHelper`, `F()`
    * `
* `Print.h`
    * `class Print`, `class Printable`
* `pgmspace.h`
    * `pgm_read_byte()`, `pgm_read_word()`, `pgm_read_dword()`,
      `pgm_read_float()`, `pgm_read_ptr()`
    * `strlen_P()`, `strcat_P()`, `strcpy_P()`, `strncpy_P()`, `strcmp_P()`,
      `strncmp_P()`, `strcasecmp_P()`, `strchr_P()`, `strrchr_P()`
    * `PROGMEM`, `PGM_P`, `PGM_VOID_P`, `PSTR()`
* `EEPROM.h`
    * compile only
* `Wire.h`
    * compile only

See [Arduino.h](https://github.com/bxparks/UnixHostDuino/blob/develop/Arduino.h)
for the latest list.

The `Serial` object sends the output to the `STDOUT`. It can also read from the
`STDIN` (in raw mode).

## System Requirements

This library has been tested on:

* Ubuntu 18.04
    * g++ 7.4.0
    * GNU Make 4.1
* MacOS 10.14.5
    * clang++ Apple LLVM version 10.0.1
    * GNU Make 3.81

## Changelog

See [CHANGELOG.md](CHANGELOG.md).

## License

[MIT License](https://opensource.org/licenses/MIT)

## Feedback and Support

If you have any questions, comments, bug reports, or feature requests, please
file a GitHub ticket instead of emailing me unless the content is sensitive.
(The problem with email is that I cannot reference the email conversation when
other people ask similar questions later.) I'd love to hear about how this
software and its documentation can be improved. I can't promise that I will
incorporate everything, but I will give your ideas serious consideration.

## Authors

Created by Brian T. Park (brian@xparks.net).
