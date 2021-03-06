# Outdated as of 5/3/19. Will be updated in the near future.

# DragonHomeNetwork

The exponential growth of wireless communications markets around the world has allowed users from all corners of the globe to share data almost instantaneously. However, managing the collection and transmission of data from multiple devices connected in small, confined networks presents major challenges. The futuristic ideal of having millions of interconnected "things" (inanimate objects, devices, communities, and environments) that have the ability to sense, communicate, network, and produce mass amounts of data that can be utilized in a novel way is known as the Internet of Things (IoT). Automation, environmental analysis, and health and safety awareness are large areas of study that can benefit from IoT networks, but issues with compatibility, versatility, and cost inhibit implementation of such networks.

The purpose of this project is to create an inexpensive, robust, low-power IoT sensor network to serve as an easily implementable model for individuals, communities, and cities. Additionally, the network can serve as a tool within other areas of research for experimentation and QA analysis. The foundation of this system was successfully created using Arduino-based processing in tandem with packet radios that communicate over sub-GHz frequency bands. Ethernet data packet management was done using MQ Telemetry Transport (MQTT) and data visualization was performed with Grafana and MATLAB. This network also utilizes InfluxDB to store and manage data effectively.


## Contents
- [Features](#features)
- [Quick start](#quickstart)
  - [Necessary hardware](#necessaryhardware)
  - [Necessary tools](#necessarytools)
  - [Hardware setup](#hardwaresetup)
  - [Network setup](#networksetup)
- [Research Analysis](#ra)
  - [Cost Scaling](#cost)
- [Releases](#releases)
- [Contributors](#contributors)

## Features <a name="features"></a>
The completed network has the following features:
- Modular addition and modification of sensors,
- Low cost ($13.67 for the gateway node and $13.67 for each additional node on the network, with efficient cost scaling),
- Power consumption: 102mA on standby, 205mA sending.
- Packet transmission up to 200 meters.

## Quick start <a name="quickstart"></a>
### Necessary hardware <a name="necessaryhardware"></a>
The following hardware is necessary to set up the network and connect one node:
- (2) Arduino Uno
- (2) Arduino Proto Shield with Mini Breadboard
- (2) RFM69HCW Wireless Transceiver - 915MHz
- (1) 10Kohm through-hole resistors
- (1) DHT11 Temperature-Humidity sensor
- (1) 9V 1A Arduino power supplies
- (2) voltage/logic converters
- Solid core wires
- Solder

### Necessary tools <a name="necessarytools"></a>
- Soldering iron
- Wire strippers
- Arduino/USB cable
- Internet-connected computer

### Hardware setup <a name="necessarytools"></a>

Follow the below schematic to set up your nodes:
todo include image.


### Network setup <a name="networksetup"></a>

Below are the steps to initialize the server, gateway and one sensor node.

1. **Gateway node setup**

   Program Radio Gateway Arduino Uno with `DragonHomeNetwork/Arduino/rfm69-network/gateway_uart_mqtt`. This node receives Radio transmissions from every sensor node and forwards them to the mqtt server via USB.

   Connect the Arduino node to your server.


2. **Server setup.**

    Open `bash` on your server to execute the following commands.

    Clone the repository:
    ```
    git clone https://github.com/DrexelSmartHouse/DragonHomeNetwork.git
    ```

    Install mosquitto and mosquitto-clients, then enable mosquitto daemon:

    ```
    sudo apt-get install mosquitto mosquitto-clients
    sudo systemctl enable mosquitto
    ```

    Install the other dependent software below.
    ```
    sudo apt-get install python-pip
    sudo pip install paho-mqtt
    sudo apt-get install tmux

    ```

    Make sure that the mqtt server IP is the same as the IP address in `serial-to-mqtt.conf`, which can be accessed by the below instructions:
    ```
    cd /path/to/DataCollectionAndAnalysis
    vim serial-to-mqtt.conf
    ```

    Open a tmux session in the cloned `DataCollectionAndAnalysis` directory and run the following scripts.

    ```
    tmux new -s dsh
    ./rfm69_mqtt_manager.py
    CTRL+B SHIFT+5 to open a parallel window.
    ./rfm69-serial-to-mqtt.py
    ```

     Now you can view sweep and scan events, as well as the sensor data and log messages as they are generated. To leave the server running, type `CTRL+B d`. To bring the session back, type `tmux a`. To switch between parallel windows, type `CTRL+B` and use the arrow keys.


3. **Sensor node setup**

   Install simple dht library through the Arduino IDE.

   In the DragonHomeNetwork/Arduino/rfm69-network/dht11_sensor_node script, change the node_id (line 13) to match the number you wish to give the end node on the network. This variable helps you to identify where data is coming from when there are multiple sensor nodes on the network.

   Upload this script to the sensor end node equipped with the DHT11 Temperature and Humidity sensor.

   This node can be powered via USB or wall power. Once powered on, it will automatically send data to the server.


## Research Analysis <a name="ra"></a>
### Cost Scaling <a name="cost"></a>
The following table documents the cost of the core of the network - the Ethernet and gateway nodes.

| Component                                                                   | Price   |
|-----------------------------------------------------------------------------|---------|
| [Arduino Uno]("https://www.aliexpress.com/item/Free-shipping-high-quality-UNO-R3-MEGA328P-CH340G-for-Arduino-Compatible-NO-USB-CABLE/32231525607.html?spm=2114.search0104.3.286.65936d00tZ2ORH&ws_ab_test=searchweb0_0,searchweb201602_3_10152_10151_10065_10344_10068_10130_10324_10342_10547_10325_10343_10546_10340_10548_10341_10545_10084_10083_10618_10307_5711212_10313_10059_10534_100031_10103_10627_10626_10624_10623_10622_10621_10620_5711313_5722413,searchweb201603_2,ppcSwitch_5&algo_expid=20dba46e-7254-40ef-8ecb-a8b02234de01-45&algo_pvid=20dba46e-7254-40ef-8ecb-a8b02234de01&transAbTest=ae803_5&priceBeautifyAB=0")                                     | $3.05  |
|  [Proto Shield with Mini Breadboard]("https://www.aliexpress.com/item/Newbrand-Prototyping-Prototype-Shield-ProtoShield-Mini-Breadboard-for-Arduino-Free-Shipping/32279103685.html?spm=2114.search0104.3.112.2fb358f9cm0yb2&ws_ab_test=searchweb0_0,searchweb201602_3_10152_10151_10065_10344_10068_10130_10324_10342_10547_10325_10343_10546_10340_10548_10341_10545_10084_10083_10618_10307_5711212_10313_10059_10534_100031_10103_10627_10626_10624_10623_10622_10621_10620_5722413_5711312,searchweb201603_2,ppcSwitch_5&algo_expid=c98ab005-6807-4240-8d2e-648ed088282c-19&algo_pvid=c98ab005-6807-4240-8d2e-648ed088282c&transAbTest=ae803_5&priceBeautifyAB=0")              | $1.10   |
| [RFM69HCW Wireless Transciever - 915MHz (4 pack)]("https://www.aliexpress.com/item/RFM69HCW-frequency-transceiver-module-433-868-915MHZ-can-be-selected/32723542589.html?spm=2114.search0104.3.1.aaa7698dOttIEv&ws_ab_test=searchweb0_0,searchweb201602_3_10152_10151_10065_10344_10068_10130_10324_10342_10547_10325_10343_10546_10340_10548_10341_10545_10084_10083_10618_10307_5711211_10313_10059_10534_100031_10103_10627_10626_10624_10623_10622_10621_10620_5711311_5722413,searchweb201603_2,ppcSwitch_5&algo_expid=62be832f-9626-4ba1-a45e-ef94174177e5-0&algo_pvid=62be832f-9626-4ba1-a45e-ef94174177e5&transAbTest=ae803_5&priceBeautifyAB=0") | $2.15  |
| [10Kohm Resistor (100-pack)]("https://www.aliexpress.com/item/100pcs-RoHS-Lead-Free-Metal-Film-Resistor-1-6W-Watts-10K-ohm-10KR-1-Tolerance-Precision/1852552440.html?spm=2114.search0104.3.16.3af73fccKp6mC7&ws_ab_test=searchweb0_0,searchweb201602_3_10152_10151_10065_10344_10068_10130_10324_10342_10547_10325_10343_10546_10340_10548_10341_10545_10084_10083_10618_10307_5711212_10313_10059_10534_100031_10103_10627_10626_10624_10623_10622_10621_10620_5711313_5722413,searchweb201603_2,ppcSwitch_5&algo_expid=094f4518-adbe-4560-8d76-4bb5bbc8398d-2&algo_pvid=094f4518-adbe-4560-8d76-4bb5bbc8398d&transAbTest=ae803_5&priceBeautifyAB=0")                      | $0.69   |
| [9V 1A Arduino Power Supply]("https://www.aliexpress.com/item/US-Plug-9V-1A-Switching-Power-Supply-Converter-Adapter-Wall-Charger-4-0mmx1-7mm/32849270931.html?spm=2114.search0104.3.109.c56333bd76YIdY&ws_ab_test=searchweb0_0,searchweb201602_3_10152_10151_10065_10344_10068_10130_10324_10342_10547_10325_10343_10546_10340_10548_10341_10545_10084_10083_10618_10307_5711213_10313_10059_10534_100031_10103_10627_10626_10624_10623_10622_10621_10620_5711313_5722413,searchweb201603_2,ppcSwitch_5&algo_expid=22c8c7c1-85ab-437a-bb54-977e7bd0358f-16&algo_pvid=22c8c7c1-85ab-437a-bb54-977e7bd0358f&transAbTest=ae803_5&priceBeautifyAB=0")                      | $2.08   |
| [Voltage/logic converter]("https://www.aliexpress.com/item/4-channel-IIC-I2C-Logic-Level-Converter-Bi-Directional-Module-5V-to-3-3V-For-Arduino/32310628741.html?spm=2114.search0104.3.44.2cbb4b4cdcwCWG&ws_ab_test=searchweb0_0,searchweb201602_3_10152_10151_10065_10344_10068_10130_10324_10342_10547_10325_10343_10546_10340_10548_10341_10545_10084_10083_10618_10307_5711211_10313_10059_10534_100031_10103_10627_10626_10624_10623_10622_10621_5722412_10620_5711311,searchweb201603_2,ppcSwitch_5&algo_expid=d1208a18-bef2-48d7-8cd7-04bda35e2558-6&algo_pvid=d1208a18-bef2-48d7-8cd7-04bda35e2558&transAbTest=ae803_5&priceBeautifyAB=0")                         | $0.28   |
| [Solid core wire]("https://www.aliexpress.com/item/Tinned-copper-22AWG-2-pin-Red-Black-cable-PVC-insulated-wire-22-awg-wire-Electric-cable/32597557917.html?spm=2114.search0104.3.2.2f6215c6LjDKMM&ws_ab_test=searchweb0_0,searchweb201602_3_10152_10151_10065_10344_10068_10130_10324_10342_10547_10325_10343_10546_10340_10548_10341_10545_10084_10083_10618_10307_5711213_10313_10059_10534_100031_10103_10627_10626_10624_10623_10622_10621_10620_5711313_5722413,searchweb201603_2,ppcSwitch_5&algo_expid=34901f3e-f79f-4489-9022-3fd66b59f083-0&algo_pvid=34901f3e-f79f-4489-9022-3fd66b59f083&transAbTest=ae803_5&priceBeautifyAB=0")                            | $0.48  |
| Total (+ shipping)                                               | $13.67 |



The final table documents how cost scales as _n_ nodes are added to the network.

| Nodes | Cost      |
|-------|-----------|
| 1     | $13.67    |
| 10    | $106.28   |
| 100   | $1210.18  |
| 500   | $6893.38  |


## Releases <a name="releases"></a>
Not currently released.

## Contributors <a name="contributors"></a>
The DragonHome Network team:

- [Joshua Cohen](https://github.com/jcohen98)
- [Reno Farnesi](https://github.com/nfarnesi4)
- [Ryan Hassing](https://github.com/ryanhassing)
- [Timothy Lechman](https://github.com/tlechman49)
