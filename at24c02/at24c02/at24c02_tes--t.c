#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define I2C_SLAVE	0x0703	/* Use this slave address */


int main(void)
{

    int ret = 0;
    unsigned char addr = 0x50;
	unsigned char chip_addr = 0xAA;

    unsigned char buf[2] = {0};
	unsigned char rbuf[2] = {0};
    int fd = open("/dev/i2c-0", O_RDWR);
    if (fd < 0)
    {
	printf("open failed\n");
	return -1;
    }
    ret = ioctl(fd,I2C_SLAVE,addr);
	printf("ret = %d\n",ret);
	buf[0] = 0xAA;
	buf[1] = 0x55;
    ret = write(fd, buf, 2);
		printf("ret = %d\n",ret);
	
	//ret = write(fd,buf, 1);
		//printf("ret = %d\n",ret);

	ret = read(fd,rbuf,2);
		printf("ret = %d\n",ret);

	printf("buf = %02x %02x\n",rbuf[0],rbuf[1]);

    return 0;
}
