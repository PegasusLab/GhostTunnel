#ifndef OSDEP_WIFI_TRANSMIT_H
#define OSDEP_WIFI_TRANSMIT_H

/*#ifdef __cpluscplus
extern "C"
#endif*/

#define MAX_IEEE_PACKET_SIZE 2048

struct packet {
  unsigned char data[MAX_IEEE_PACKET_SIZE];
  unsigned int len;
};


int osdep_start(char *interface1, char *interface2);

int osdep_send_packet(struct packet *pkt);

struct packet osdep_read_packet();

void osdep_stop();

/*
#ifdef __cpluscplus
};
#endif*/

#endif