#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>

struct rtc_time rtc_tm;

int main(int argc, char *argv[])
{
	int fd = -1;

	
	//memset(&rtc_tm,0x00,sizeof(rtc_tm);
	
	fd = open("/dev/rtc0", O_RDWR);   //允许读写
	if(fd < 0)
	{
		printf("打开时钟失败\n");		
	}
	rtc_tm.tm_year = 2020;
	rtc_tm.tm_mon = 02;
	rtc_tm.tm_mon = 14;
	rtc_tm.tm_hour = 21;
	rtc_tm.tm_min = 50;
	
	ioctl(fd, RTC_SET_TIME, &rtc_tm);
	ioctl(fd, RTC_RD_TIME, &rtc_tm);
	printf("%d %d %d %d %d\n",rtc_tm.tm_year,rtc_tm.tm_mon,rtc_tm.tm_mday,rtc_tm.tm_hour,rtc_tm.tm_min);
    return 0;
}


