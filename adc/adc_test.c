#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/*
	./adc_test port rel
	./adc_test 1 12
*/
#define ADC_REL		0x0D
#define ADC_PORT	0x0C

int main(int argc, char *argv[])
{
	int fd;
	unsigned long data;
	double voltage;
	int rel;
	int port;

	if (argc < 3) {
		printf("usage:\n %s port rel\n", argv[0]);
		return -1;
	}

	fd = open("/dev/adc", O_RDWR);
	if (fd < 0) {
		printf("open adc failed.\n");
		return -1;
	}
//	printf("222222222222\n");
	rel = strtoul(argv[2], NULL, 10);
	port = strtoul(argv[1], NULL, 10);
//	printf("222222222222222\n");
	ioctl(fd, ADC_REL, &rel);
//	printf("333333333333333\n");
	ioctl(fd, ADC_PORT, &port);
//	printf("11111111111111\n");
	while(1) {
		read(fd, &data, sizeof(data));	
		voltage = 3.3 * data / (1 << rel);
		printf("vol = %lf\n", voltage);

		sleep(1);
	}
	
	close(fd);
	return 0;
}
