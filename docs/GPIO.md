## GPIO models

A model of the GPIO peripherals is included with these models.

You can find information about their limitations and approximations in
[the source files](../src/HW_models/NRF_GPIO.c).

Note that as of today these models:
* Consider the output drivers to have 0 impedance: The drive is instantaneous.
* Do not allow outputs drivers to be in a tristate mode (an output can be disconnected, but not
 not-driven)
* Do not allow several outputs to be shortcircuited together to form logical OR's or AND's
* Do not model input pulls.

That is, inputs just sample the input pin, and outputs just deliver a logical level: high or low.

### Driving inputs:

Inputs can be driven in 3 different ways:

* From the input file backend (see below).
* From the FIFO backend (see below)
* From test code, calling `nrf_gpio_test_change_pin_level()`
* By shortcuiting the input and output, either set from a configuration file
  (`-gpio_conf_file=<path>`, see below), or
  programmatically, by calling `nrf_gpio_backend_register_short()`

#### Input file backend

The input file backend can be used to drive GPIOs with levels present in input files.

These input files follow the stimuli format described below.
Multiple instances of this backend can be enabled by either passing `-gpio_in_file=<path>`
to the executable from the command line, or by adding in the configuration file an
`input_file <path>` line as described below.
Each time this parameter is passed another backend will be instantiated with that new file.

You can configure as many input files to be used as necessary.

#### FIFO backend

It is also possible to connect two simulated devices through GPIO FIFOs.
This backend exchanges pin changes between devices and uses timed NOP messages to keep both
sides advancing in simulation time.

A single FIFO connection can be enabled, providing **both** command line options 
`-gpio_fifob_txfile=<path>` and `-gpio_fifob_rxfile=<path>`.
The FIFOs are created automatically.
Optionally, `-gpio_fifob_mdt=<usec>` controls the maximum idle time between NOP messages.

An arbitrary number of extra FIFO instances can be configured via the configuration file, adding
a line like `fifo <tx_path> <rx_path> [mdt=<usec>]` (see below).

### Output log file:

It is possible to have the GPIOs output activity dumped into a file.
For this just call the excutable with `-gpio_out_file=<path>`.
Any toggle in any pin configured as an output will be dumped to that file, following the
stimuli file format described below.

### Monitor inputs/outputs from test code:

Embedded test code specific for simulation can monitor the inputs and outputs changes
by registering a callback with `nrf_gpio_test_register_in_callback()` and
`nrf_gpio_test_register_out_callback()`.
With the first one, the callback will be called each time an *input gpio pin register* is modified.
With the second one, each time the output pin itself changes. That is, both will be called only
if the pin is connected/driven in that direction.

### Mapping of real ports to simulated port indexes

* 52822
    * P0: 0
    * P1: 1
* 5340 NOTE: The Net and App cores ports are mapped to different/separate simulated ports/pins,
  unlike in real HW where they are connected to the same physical pins. Note that from the embedded
  SW point of view (that is, when configuring an App core GPIOTE channel), the App core port P0 is
  port "0" just like when configuring it in real HW).
    * Net core P0: 0
    * Net core P1: 1
    * App core P0: 2
    * App core P1: 3
* 54L15
    * P0 (LP domain): 0
    * P1 (Per domain): 1
    * P2 (MCU domain): 2

### Stimuli file format

This is a comma separated file (.csv), with 4 columns: time,port,pin,level. Where:
* Time is a 64 bit unsigned value, representing time in microseconds since boot.
* Port is the GPIO port number starting from 0 (for a nrf52833: 0 or 1).
* Pin is the pin number in that port (for a nrf52833: 0..31 for port 0, and 0..9 for port 1)
* Level is either 0 (for low) or 1 (for high)

For example:

```
time(microsecond),port,pin,level
0,0,0,1
200,0,0,0
600,0,0,1
800,0,0,0
1000,0,0,1
1000,0,0,0
101624,0,0,1
```

Where pin 0 in port 0, is toggled at boot, 200microseconds, 600microseconds, 800microseconds, 1ms
(up and immediately down), and 101.624ms.

### Configuration file format

The configuration file can configure short-circuits, as well as instantiate
FIFO & input file backend instances.

**Short-circuits** (`short` / `s`):

Note that these shorts only work one way: output to input.
It is not possible to short 2 outputs together.
It is possible to short one output to several (up to 8) inputs.

Each short circuit is configured with a line like either:

`short out_port.out_pin in_port.in_pin`<br>
`s out_port.out_pin in_port.in_pin`

The first word must always be either "short" or "s"
And it must always be followed by 4 unsigned integers in 2 pairs.
Each pair separated by a dot. The first value of each pair being the port number,
the second value the pin in that port.
The first pair indicating the output pin that will be shorted from, and the 2nd pair
the input pin that will be shorted to.

For example:

```
short 0.1 1.2
short 0.1 1.3
```
To short pin 1 from port 0, to both pins 2 and 3 from port 1,<br>

```
short 1.0 0.2
```
to short pin 0 from port 1 to pin 2 from port 2.

Note that both ports and pins are numbered from 0.

**Input file backend instances** (`input_file`):

Adding a line containing:

`input_file <path>`

Adds a new file-input backend instance reading from `<path>`.
You can add as many of these as desired.

**FIFO instances** (`fifo`):

`fifo <tx_path> <rx_path> [mdt=<usec>]`

Adds a FIFO backend instance.
The optional `mdt=<usec>` overrides the default maximum idle time for this instance.

### Comments in input files

For both stimuli input and configuration files, '#' is treated as a comment delimiter: That
character and anything after until the end of the line will be ignored.

Empty lines, are also ignored.
