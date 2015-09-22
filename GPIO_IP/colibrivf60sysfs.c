#include "colibrivf60sysfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>                   // for open(), close(), lseek()
#include <sys/stat.h>                // for open()
#include <sys/types.h>               // for open()
#include <unistd.h>

extern Gpio leds[LEDS],keys[KEYS];

void exportgpio(char ** _portas,int y){
	int x,retval,fdescrisp;

	for( x=0;x<y;x++){

		fdescrisp=open(PATH_EXPORT,O_WRONLY);
		retval=write(fdescrisp,*_portas ,sizeof(*_portas));
		retval=close(fdescrisp);
		_portas++;
		}
	printf("%d",retval);

}
void unexportgpio(char ** _portas,int y){
	int x,retval,fdescrisp;

	fdescrisp=open(PATH_UNEXPORT,O_WRONLY);
	for(x=0;x<y;x++){
		retval=write(fdescrisp,*_portas ,sizeof(*_portas));
		_portas++;
		}
	retval=close(fdescrisp);
	printf("%d",retval);
}
void configpio(char * _x,char * _y){
	int retval,fdescrisp;

			fdescrisp=open(_x,O_RDWR);
			retval=write(fdescrisp,_y ,sizeof(_y));
			retval=close(fdescrisp);
			printf("%d",retval);

}
void initstructgpio(char **PORTS_LED,char **PORTS_KEY){

	/*inicializa Objetos LED e direcao mapeadas em sysfs*/
int x;
char p[40]="";

	for( x=0;x<LEDS;x++){
		leds[x].value=OFF;
		leds[x].direcao=OUT;
		strcat(p,PATH_OBJ);
		strcat(p,PORTS_LED[x]);
		strcat(p,"/value");
		leds[x].v_path=malloc(strlen(p)+1);
		strcpy(leds[x].v_path,p);
		memset(p,0,sizeof(p));
		strcat(p,PATH_OBJ);
		strcat(p,PORTS_LED[x]);
		strcat(p,"/direction");
		leds[x].d_path=malloc(strlen(p)+1);
		strcpy(leds[x].d_path,p);
		configpio(leds[x].d_path,OUT);
		memset(p,0,sizeof(p));
	}

	for( x=0;x<KEYS;x++){
		keys[x].direcao=IN;
		memset(p,0,sizeof(p));
		strcat(p,PATH_OBJ);
		strcat(p,PORTS_KEY[x]);
		strcat(p,"/value");
		keys[x].v_path=malloc(strlen(p)+1);
		strcpy(keys[x].v_path,p);
		memset(p,0,sizeof(p));
		strcat(p,PATH_OBJ);
		strcat(p,PORTS_KEY[x]);
		strcat(p,"/direction");
		keys[x].d_path=malloc(strlen(p)+1);
		strcpy(keys[x].d_path,p);
		configpio(keys[x].d_path,IN);


	}
}
