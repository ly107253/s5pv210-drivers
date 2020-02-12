#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define IOCTL_GPIO_ON	1
#define IOCTL_GPIO_OFF	0

int main(void)
{
    int fd;

    //打开设备
    fd = open("/dev/led", O_RDWR);
    if (fd < 0) {
        printf("open led failed.\n");
        return -1;
    }

    ioctl(fd, IOCTL_GPIO_ON, 0);        
    sleep(3); //睡眠3秒钟

    close(fd); //关闭设备

    return 0;
}
