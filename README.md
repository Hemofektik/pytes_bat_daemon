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
sudo apt install libcpprest-dev binutils-dev cmake

cd /tmp
git clone https://github.com/gbmhunter/CppLinuxSerial.git
cd CppLinuxSerial
mkdir build
cd build
cmake -DSERIAL_BUILD_SHARED_LIBS=ON ..
make -j4
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

Update the REST server port or the device path for the Pytes BMS UART interface in the [pytes.cfg](pytes.cfg) to your needs.

## Running as a service/daemon

Once compilation has been completed successfully run the following commands in the build directory to enable the pytes service.

```
sudo make install
sudo systemctl enable pytes
sudo systemctl start pytes
```

# Home Assistant Integration

Extend the [configuration.yaml](https://www.home-assistant.io/docs/configuration/) as follows and [restart Home Assistant](https://www.home-assistant.io/docs/configuration/#reloading-the-configuration-to-apply-changes).

```
rest:
  - scan_interval: 1
    resource: http://localhost:7735/power/aggregated
    sensor:
      - name: "Battery SoC"
        unique_id: "PYTES_BATTERY_PACK_0_SOC"
        value_template: "{{ value_json['soc'] }}"
        device_class: battery
        unit_of_measurement: "%"
      - name: "Battery Power"
        unique_id: "PYTES_BATTERY_PACK_0_POWER"
        value_template: "{{ value_json['power'] }}"
        device_class: power
        unit_of_measurement: "W"
      - name: "Battery Current"
        unique_id: "PYTES_BATTERY_PACK_0_CURRENT"
        value_template: "{{ value_json['current'] }}"
        device_class: current
        unit_of_measurement: "A"
      - name: "Battery Energy"
        unique_id: "PYTES_BATTERY_PACK_0_ENERGY"
        value_template: "{{ value_json['energy'] }}"
        device_class: energy
        unit_of_measurement: "kWh"
      - name: "Battery Daily Charge"
        unique_id: "PYTES_BATTERY_PACK_0_DAILY_CHARGE"
        value_template: "{{ value_json['energy_charge_daily'] }}"
        device_class: energy
        state_class: total_increasing
        unit_of_measurement: "kWh"
      - name: "Battery Daily Discharge"
        unique_id: "PYTES_BATTERY_PACK_0_DAILY_DISCHARGE"
        value_template: "{{ value_json['energy_discharge_daily'] }}"
        device_class: energy
        state_class: total_increasing
        unit_of_measurement: "kWh"
      - name: "Battery State"
        unique_id: "PYTES_BATTERY_PACK_0_STATE"
        value_template: "{{ value_json['state'] }}"
```