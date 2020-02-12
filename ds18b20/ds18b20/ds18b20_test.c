#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

#define DS18B20_RESET 0x100001
#define DS18B20_REL   0x100002

int main(int argc, char *argv[])
{
        int fd;
        int data;
		char cData[10];
        unsigned char rel = 12;
        int isread = 1;
		double dValue;
		
        fd = open("/dev/ds18b20", O_RDWR);
        if (fd < 0) {
                printf("open ds18b20 failed.\n");
                exit(-1);
        }   
    


		ioctl(fd,DS18B20_REL,&rel);

        
        while (1) {
					read(fd,&data,sizeof(data));
					dValue = (float)data/10000;
					sprintf(cData, "%.3f", dValue);
					printf("%s\n",cData );
					sleep(1);
        }
        close(fd);
        return 0;
}
