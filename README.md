Packet Sniffer
Description
This project is a simple packet sniffer written in C that captures network packets at the Ethernet layer and processes the IP, TCP, and UDP headers. It uses raw sockets to receive packets and utilizes threads to handle the packet reception and protocol extraction.

Features
Captures Ethernet frames and logs source and destination MAC addresses.
Extracts and logs IP addresses, protocol types, and port numbers for TCP and UDP packets.
Multithreaded design for simultaneous packet reception and protocol extraction.
Files
main.c: The main source file containing the packet sniffer implementation.
logfileMac.txt: The log file where the captured packet details are stored.
Dependencies
POSIX Threads (pthread)
Standard C libraries: stdio.h, stdlib.h, string.h
Networking libraries: unistd.h, sys/socket.h, netinet/in.h, arpa/inet.h
Packet capture libraries: net/ethernet.h, netinet/ip.h, netinet/tcp.h, netinet/udp.h
How to Use
Compile the Program:

Use the following command to compile the program:

bash
Kodu kopyala
gcc -o packet_sniffer main.c -lpthread
Run the Program:

Execute the compiled binary and select an option to run either the Ethernet frame capture or the protocol extraction:

bash
Kodu kopyala
sudo ./packet_sniffer
Note: The program must be run with root privileges to access raw sockets.

Select an Option:

When prompted, select one of the following options:

Enter 1 to run the Ethernet frame capture.
Enter 2 to run the protocol extractor.
The program will then start capturing packets and logging the details to logfileMac.txt.

Explanation of the Code
Main Function
Socket Creation: Creates a raw socket to capture all packets at the Ethernet layer using socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)).
Thread Creation: Depending on the user's choice, it creates and runs threads for Ethernet frame capture (packetReceiver) or protocol extraction (protocolExtractor).
packetReceiver Function
Packet Reception: Continuously receives packets and copies the Ethernet header from the buffer.
Logging: Logs the source and destination MAC addresses of the received Ethernet frames to the log file.
protocolExtractor Function
Packet Reception: Continuously receives packets and extracts the IP header.
Protocol Handling: Depending on the IP protocol type (TCP, UDP, or other), extracts and logs the source and destination IP addresses, port numbers, and packet size.
Notes
Ensure the log file logfileMac.txt is writable.
The program requires root privileges to run due to the use of raw sockets.
Error handling is included to manage packet reception issues and log file operations.
