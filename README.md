# PYTES Battery Daemon

This program connects to a PYTES battery through the serial port and reads out the battery telemetry. A REST service is running to provide that information to a consumer such as [Home Assistant](https://www.home-assistant.io/).

## Hardware Setup

This is an example setup that this program has been developed for. 

 * Orange Pi Zero 3 @ Armbian 24
 * OIKWAN Console Cable USB-C TO RJ45 (RS-232)
 * PYTES battery of type E-BOX-48100R-C

## Building

### Installing dependencies

Run the following commands to install the dependencies.

```
sudo apt install libcpprest-dev

cd /tmp
git clone https://github.com/gbmhunter/CppLinuxSerial.git
cd CppLinuxSerial
mkdir build
cd build
cmake ..
make
sudo make install
```

### Compiling pytes daemon

```
git clone https://github.com/Hemofektik/pytes_bat_daemon.git
cd pytes_bat_daemon
mkdir build
cd build
cmake ..
make -j4
```

## Configuration

TBD

## Running as a service/daemon

TBD

# Home Assistant Integration

TBD