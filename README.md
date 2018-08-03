# GhostTunnel

GhostTunnel is a covert backdoor transmission method that can be used in an isolated environment. It can attack the target through the HID device only to release the payload (agent), then the HID device can be removed after the payload is released.

GhostTunnel use 802.11 Probe Request Frames and Beacon Frames to communicate and doesn't need to establish a wifi connection. Exactly, it communicates by embedding data in beacon and probe requests. We publish the GhostTunnel server and windows agent implemented in c/c++. The agent doesn't need elevated privileges, it uses the system wifi api to send the probe request and receive the beacon. such as on windows, uses the Native WiFi API. So you can implement the corresponding agent on other platforms. The server runs on linux, you need one or two usb wifi card that supports monitor mode and packet injection to run it. 


# Advantages

- Covertness.
- No interference with the target’s existing connection status and communications.
- Can bypass firewalls.
- Can be used to attack strictly isolated networks.
- Communication channel does not depend on the target’s existing network connection.
- Allow up to 256 clients 
- Effective range up to 50 meters
- Cross-Platform Support.
- Can be used to attack any device with wireless communication module, we tested this attack on Window 7 up to Windows 10, and OSX.


# Usage

- Server
Only need one or two wireless network cards that supports packet injection and monitor mode, like TP-LINK TL-WN722N, Alfa AWUS036ACH.
Usage: 
	./ghosttunnel [interface]
	./ghosttunnel [interface1] [interface2]

COMMANDS:
	sessions = list all clients
	use = select a client to operate, use [clientID]
	exit = exit current operation
	wget = download a file from a client, wget [filepath]
	quit = quit ghost tunnel
	help = show this usage help


- Client
	Release the payload to the target system (only windows client published) and execute it.


# Function Implementation

- Shell command
	Create a remote shell. 

- Download file
	The file maximum size limit is 10M and can only download one file at a time.

- You can add other functions as needed.

# Building

### Server Requirements
	apt-get install pkg-config libnl-3-dev libnl-genl-3-dev 


### Compiling

	server:
		cd src
		make
	windows client:
		Microsoft Visual Studio 2015 



# Thanks 

- Aircrack-ng
	[https://github.com/aircrack-ng/aircrack-ng](https://github.com/aircrack-ng/aircrack-ng)
- MDK4
	[https://github.com/aircrack-ng/mdk4](https://github.com/aircrack-ng/mdk4)
- hostapd
	[http://w1.fi/hostapd](http://w1.fi/hostapd)
- Kismet
	[https://github.com/kismetwireless/kismet](https://github.com/kismetwireless/kismet)



























