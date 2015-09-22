/*
 * main.c
 *
 *  Created on: Sep 22, 2015
 *      Author: leonardo
 *      Teste de GPIO e comunicacao UDP
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "colibrivf60sysfs.h"

#include <fcntl.h>                   // for open(), close(), lseek()
#include <sys/stat.h>                // for open()
#include <sys/types.h>               // for open()
#include <unistd.h>                  // for write(), open(), read(), usleep()



#define BUFFSIZE 255
 void ERRO(char *mess) { perror(mess);  }

static char *PORTS_KEY[]={"46","88","48"};
static char *PORTS_LED[]={"47","50","53"};

/*Variaveis Globais*/

Gpio leds[LEDS],keys[KEYS];


int main(int argc, char *argv[])
{
	int retval,i,x;




	struct sockaddr_in vf61clieth0;
	struct sockaddr_in dellpc;
	char buffer[BUFFSIZE];
    char message[BUFFSIZE];
	unsigned int udplen;
	int sock;
	unsigned int slen = sizeof(dellpc);
	char comunication=ON;
	char LAPTOP[]="192.168.100.228";

	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		ERRO("Erro de criacao do socket");
	}
	/* Construct the server sockaddr_in structure */

	memset(&vf61clieth0, 0, sizeof(vf61clieth0));       /* Clear struct */
	vf61clieth0.sin_family = AF_INET;                  /* Internet/IP */
	vf61clieth0.sin_addr.s_addr = inet_addr(LAPTOP);  /* IP address */
	vf61clieth0.sin_port = htons(atoi("1977"));       /* server port */




	exportgpio(PORTS_KEY,3);
	exportgpio(PORTS_LED,3);
	initstructgpio(PORTS_LED,PORTS_KEY);

	/* Abrir descritors LED*/
	for(i=0;i<LEDS;i++)
		leds[i].FD= open(leds[i].v_path,O_WRONLY);
	/* Abrir descritors KEYS*/
	for(i=0;i<KEYS;i++)
		keys[i].FD= open(keys[i].v_path,O_RDONLY);

	/* 20 segundos pisca leds*/
	x=0;
	do
	{
		for(i=0;i<LEDS;i++){
			leds[i].value=(leds[i].value==ON)?OFF:ON;
			retval = write(leds[i].FD, &leds[i].value, sizeof(leds[i].value));
		}
		usleep(500000);
		x++;
	}while(x<10);


	/* 10 segundos le chaves*/
	x=0;
	do
	{
		for(i=0;i<3;i++){

			retval = read(keys[i].FD, &keys[i].value, sizeof(keys[i].value));
			lseek(keys[i].FD,0,SEEK_SET);
			leds[i].value=(keys[i].value==ON)?OFF:ON;
			retval = write(leds[i].FD, &leds[i].value, sizeof(leds[i].value));


		}
		usleep(100000);

		x++;
	}while(x<100);


	do{
	/* Envia estado da chave */

	//receive a reply and print it
	//clear the buffer by filling null, it might have previously received data
	memset(buffer,'\0', BUFFSIZE);
	memset(&dellpc, 0, sizeof(dellpc));
	//try to receive some data, this is a blocking call
	if (recvfrom(sock, buffer, BUFFSIZE, 0, (struct sockaddr *) &dellpc, &slen ) == -1)
	{
		ERRO("UDP receiver error: ");
	}
	printf("Received packet from %s:%d\n", inet_ntoa(dellpc.sin_addr), ntohs(dellpc.sin_port));


	switch(buffer[0]){
	case'K':
		memset(message,'\0', BUFFSIZE);
		for(x=0;x<KEYS;x++){
		strcat(message,"|_Valor da chave: ");
		strcat(message,&keys[x].value);
		}

	udplen = strlen(message);
	if (sendto(sock, message, udplen , 0 , (struct sockaddr *) &vf61clieth0, sizeof(vf61clieth0))==-1)
	{
		ERRO("UDP transmit error:");
	}
	break;
	case'L':
		leds[3].value=(leds[3].value==ON)?OFF:ON;
		retval = write(leds[3].FD, &leds[3].value, sizeof(leds[3].value));

		break;

	case'E':
	comunication=OFF;
	break;

	}

		}while(comunication==ON);




	/*fechar descritores de arquivo LEDS*/
	for(x=0;x<LEDS;x++)
		retval=close(leds[x].FD);
	/*fechar descritores de arquivo KEYS*/
	for(x=0;x<KEYS;x++)
		retval=close(keys[x].FD);
	close(sock);
	unexportgpio(PORTS_KEY,3);
	unexportgpio(PORTS_LED,3);

	printf("Teste arm compiler toradex:%d \n",retval);

	return(0);

}


