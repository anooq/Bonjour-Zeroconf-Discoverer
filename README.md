# Bonjour-Zeroconf-Discoverer
A software to discover devices in local subnet and display their found metadata using Bonjour/Zeroconf. Device addition, removal and update events are displayed using a callback function.

Requirements:
Development headers for the Avahi Apple Bonjour compatibility library
	- sudo apt-get install libavahi-compat-libdnssd-dev

Usage:
1. cd Bonjour-Zeroconf-Discoverer
2. make
3. cd build
4. ./device_finder
