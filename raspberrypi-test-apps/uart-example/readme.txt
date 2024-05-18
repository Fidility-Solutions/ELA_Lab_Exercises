Follow the below steps:-

1. Compiling and Running uart-echo-host.c on the Host:
	1. Compile the uart-echo-host.c file on the host machine:
		$ gcc -o uart-echo-host uart-echo-host.c
	2.Run the compiled uart-echo-host binary on the host.
	3. Observe UART working on the host.

2.Cross Compiling and Running uart-echo-target on the Target:
	1. Cross compile the uart-echo-target.c file for the target device.
	2. Transfer the compiled binary from the host to the target using scp
		$ scp uart-echo-target user@target:/path/to/destination
	3.Run the uart-echo-target binary on the target.

3.Compiling and Running uart-transmit-host.c on the Host:
	1. Compile the uart-transmit-host.c file on the host machine:
		$ gcc -o uart-transmit-host uart-transmit-host.c

4. Cross Compiling and Running uart-receive-target.c on the Host:
	1. Set up the target toolchain and cross compile the uart-receive-target.c file on the host.
	2. Transfer the compiled binary from the host to the target using scp:
		$ scp uart-receive-target user@target:/path/to/destination

5. Run the uart-receive-target binary on the target first, as the receiver always starts the operation
6. After starting the receiver on the target, run the transmitter on the host to ensure proper communication.

