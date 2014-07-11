/********************************************************
 * Filename: mqdmx.c
 * Author: daedalus
 * Email: 
 * Description: 
 *
 *******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <libftdi1/ftdi.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <signal.h>

#include "misc.h"
#include "dmxd.h"


//char dmx_channels[513];
char diff[513];
char* dmx_channels;
int baud = 250000;
int bits = 8;
int parity = 0;
int stop_bits = 2;
int vendorid = 0x0403;
int productid = 0x6001;

struct ftdi_context* ctx;

void ftdi_break(useconds_t duration)
{
	if(ftdi_set_line_property2(ctx, bits, stop_bits, parity, BREAK_ON) < 0)
		errdie("break - lineproperty2 BREAK_ON");
	usleep(duration);
	if(ftdi_set_line_property2(ctx, bits, stop_bits, parity, BREAK_OFF) < 0)
		errdie("break - lineproperty2 BREAK_OFF");
}


static void* dmx_writer(void* unused)
{
	while(1) {
		if (dmx_channels) {
			ftdi_break(10000);
			if(ftdi_write_data(ctx, dmx_channels, 513) < 0)
				errdie("ftdi_write_data");
		} else {
			printf("dmx_channels has invalid address: %p\n", dmx_channels);
		}

	}
}

int main(int argc, char* const* argv)
{
	struct ftdi_device_list* lst;
	ctx = ftdi_new();

	if(argc < 2){
		printf("DEVICE LIST:\n");
		struct libusb_device* dev = NULL;
		ftdi_usb_find_all(ctx, &lst, vendorid, productid);
		
		for(; lst; lst = lst->next) {
			char serialid[100];
			serialid[0] = 0;
			ftdi_usb_get_strings(ctx, lst->dev, NULL, 0, NULL, 0, serialid, 100);
			if(serialid[0] != 0){
				printf("%s\n", serialid);
			} else {
				printf("<none>\n");
			}
		}
		die("usage: dmxd <udp-port> <serialid>");
	}

	dmx_channels = (char*)mmap(NULL, 513, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 1, 0);
	if(dmx_channels == MAP_FAILED) {
		errdie("mmaping dmx_channels");
	}
	printf("dmx_channels maped to %p (MAP_FAILED = %p)\n", dmx_channels, MAP_FAILED);
	struct libusb_device* dev = NULL;
	ftdi_usb_find_all(ctx, &lst, vendorid, productid);
	for(; lst; lst = lst->next) {
		char serialid[100];
		serialid[0] = 0;
		ftdi_usb_get_strings(ctx, lst->dev, NULL, 0, NULL, 0, serialid, 100);
		if(serialid[0] != 0){
			printf("%s\n", serialid);
			if(!strcmp(argv[2], serialid) || !strcmp(argv[2], "first_best")){
				dev = lst->dev;
				break;
			}
		} else {
			printf("<none>\n");
		}
	}
	if(!dev)
		errdie("unable to find dmx cable");
	if(ftdi_usb_open_dev(ctx, dev) < 0)
		errdie("ftdi_usb_open_dev");
	if(!ctx)
		die("ctx == NULL");
	if(ftdi_set_baudrate(ctx, baud) < 0)
		errdie("ftdi_set_baudrate");
	if(ftdi_set_line_property(ctx, bits, stop_bits, parity) < 0)
		errdie("ftdi_set_line_property");
	memset(dmx_channels, 0, 513);

	pthread_t tpid;
	pthread_create(&tpid, NULL, dmx_writer, NULL);
	pthread_setname_np(tpid, "dmx_writer");

	struct sockaddr_in host;
	host.sin_family = AF_INET;
	host.sin_port = htons(atoi(argv[1]));
	host.sin_addr.s_addr = INADDR_ANY;

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == -1)
		errdie("socket");
	if (bind(sock, (struct sockaddr*)&host, sizeof(struct sockaddr)) == -1)
		errdie("bind");

	while(1) {
		int res = recv(sock, &dmx_channels[1], 512, 0);
		if(res == -1) {
			errdie("recv");
		}
	}
	
}

