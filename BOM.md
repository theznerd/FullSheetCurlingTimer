# Bill of Materials

**STATUS:** INITIAL DRAFT - DO NOT USE FOR PROCUREMENT

**VERSION:** 0.1

**LAST UPDATED:** 2025-12-16

This document lists the components required to build the Full Sheet Curling
Timer for a single sheet.

## Electronic and Mechanical Components

| Component | Quantity | Description | Link | Price (each) | Total Price | Notes |
|-----------|----------|-------------|------|--------------|-------------|-------|
| ESP32 Dev Board with IPEX Connector | 4 | ESP32-WROOM-32 or similar | [Amazon](https://www.amazon.com/dp/B0865YSHSS) | $5.50 | $22.00 | Ensure it has an IPEX connector for external antenna |
| 2.4GHz Antenna | 4 | WiFi antenna with U.FL to Female SMA cable | [Amazon](https://www.amazon.com/dp/B0CD7LBJ3D) | $3.00 | $12.00 | 3dBi gain recommended |
| 18650 Batteries | 4 | Rechargeable Li-ion batteries | [Amazon](https://www.amazon.com/dp/B0DF6CPJFR) | $5.75 | $23.00 | 3000mAh or higher |
| 18650 Battery Holders | 4 | Single 18650 battery holder with leads | [Amazon](https://www.amazon.com/dp/B00LSG5BKO) | $0.80 | $8.00 | Pack comes with 10 |
| 18650 Battery Charger Module | 4 | TP4056 charger module with protection and 5V step-up | [Amazon](https://www.amazon.com/gp/product/B0BJYRGZR4) | $1.00 | $10.00 | Pack comes with 10 |
| Laser Diode Module | 4 | 650nm red laser module | [Amazon](https://www.amazon.com/dp/B071FT9HSV) | $0.68 | $6.80 | Pack comes with 10 |
| 5MM Phototransistor | 4 | Light-sensitive phototransistor | [Amazon](https://www.amazon.com/dp/B00M1PMHO4) | $0.29 | $5.79 | Pack comes with 20 |
| LM393 Comparator | 4 | LM393-based light sensor module | [Amazon](https://www.amazon.com/dp/B0FPQP47M5) | $0.40 | $8.00 | Pack comes with 20 |
| Various Resistors | N/A | Various resistors for voltage dividers and pull-ups | [Amazon](https://www.amazon.com/dp/B08FD1XVL6) | N/A | $10.00 | Pack comes with assorted values, and way too many. Will optimize later based on actual schematic needs. |
| Various Capacitors | N/A | Various capacitors for power smoothing | [Amazon](https://www.amazon.com/dp/B07PBQXQNQ) | N/A | $17.00 | Pack comes with assorted values, and way too many. Will optimize later based on actual schematic needs. |
| Rocker Switch | 4 | SPST rocker switch for power control | [Amazon](https://www.amazon.com/dp/B0CVTRKJTV) | $0.75 | $6.00 | Pack comes with 8 |
| Piezo Buzzer | 4 | Active piezo buzzer for audio feedback | [Amazon](https://www.amazon.com/gp/product/B0F1KCZGZG) | $0.40 | $8.00 | Pack comes with 20 |
| 5 position DIP Switch | 4 | 5-position DIP switch for ID configuration | [Amazon](https://www.amazon.com/dp/B082GMFTKZ) | $0.65 | $6.50 | Pack comes with 10 |
| Engineering Grade Retroreflective Tape | 1 | 1" x 30' roll for laser reflection | [Amazon](https://www.amazon.com/dp/B079DC1DK8) | $13.80 | $13.80 | Cut into smaller pieces as needed |

For the resistors and capacitors, the BOM above includes many more than
actually needed. The actual required values are as follows if you can source
them individually:

| Component | Quantity | Value | Purpose |
|-----------|----------|-------|---------|
| Resistor | 8 | 10kΩ | LM393 Output and Phototransistor pull-up to 3.3v |
| Resistor | 8 | 100kΩ | LM393 1.65v reference divider |
| Resistor | 4 | 1MΩ | LM393 Hysteresis feedback |
| Resistor | 4 | 1kΩ | Laser current limiting |
| Capacitor | 8 | 0.1µF | LM393 Power supply decoupling and ESP32 GPIO noise suppression |

> NOTE: Above list does not yet include resistors for voltage dividers on ESP32
> ADC input for battery-voltage monitoring. Will update once those values are
> determined.

## PCB Fabrication

The design can be fabricated using a PCB manufacturing service. Below are the
estimated costs for a batch of 5 PCBs (give yourself some grace for mistakes).
It is recommended you take the PCB approach rather than breadboarding as the
enclosure models are designed to fit the PCB form factor and to properly align
the laser and phototransistor components.

| Service | Quantity | Price per PCB | Total Price | Notes |
|---------|----------|---------------|-------------|-------|

\#\#\# TO BE DETERMINED

## Sensor Enclosure and Reflector 3D Prints

The enclosures for the laser and phototransistor sensors, as well as the
retroreflector mounts, can be 3D printed. Below are estimated costs based on
material usage. ABS or PETG is recommended for durability, but PLA can be used
as well as it is generally more accessible for hobbyists (someone in your club
probably has a 3D printer they can lend to the effort).

| Material | Quantity | Price per kg | Estimated Usage (g) | Total Price | Notes |
|----------|----------|--------------|---------------------|-------------|-------|

\#\#\# TO BE DETERMINED
