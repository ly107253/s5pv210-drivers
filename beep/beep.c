#if 1
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("arg format:\n\
            argv[1]: 0-> sound off / 1-> sound on\n\
            argv[2]: sound volume, from 1 to 100\n\
            argv[3]: sound frequency level, from 0 to 9\n\
            argv[4]: beep secounds\n\
            e.g.: beep_test 1 50 5 2\n");
        return -1;
    }
    int fd;

    int beep_args[3];
    beep_args[0] = atoi(argv[2]);
    beep_args[1] = atoi(argv[3]);
    beep_args[2] = atoi(argv[4]);
    fd = open("/dev/my_beep", O_RDWR);

    if(fd < 0)
    {
        printf("open /dev/my_beep error ! \n");
        return -1;
    }
    
    /*
    * ioctl (*file, int cmd, int (*beep_args)[2])
    * cmd: 0-> sound off / 1-> sound on
    * (*beep_args)[0] : snd_lvl : from 1 to 100
    * (*beep_args)[1] : snd_freq : from 1 to 10
    * (*beep_args)[2] : beep_sec : from 1 to 10
    */
    ioctl(fd, atoi(argv[1]), &beep_args);
    close(fd);

    return 0;
}
#endif
#if 0
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("arg format:\n\
            argv[1]: 0-> sound off / 1-> sound on\n\
            argv[2]: sound volume, from 1 to 100\n\
            argv[3]: sound frequency level, from 0 to 9\n\
            argv[4]: beep secounds\n\
            e.g.: beep_test 1 50 5 2\n");
        return -1;
    }
    int fd;

    int beep_args[3];
    beep_args[0] = atoi(argv[2]);
    beep_args[1] = atoi(argv[3]);
   // beep_args[2] = atoi(argv[4]);
    fd = open("/dev/beep", O_RDWR);

    if(fd < 0)
    {
        printf("open /dev/my_beep error ! \n");
        return -1;
    }
    
    /*
    * ioctl (*file, int cmd, int (*beep_args)[2])
    * cmd: 0-> sound off / 1-> sound on
    * (*beep_args)[0] : snd_lvl : from 1 to 100
    * (*beep_args)[1] : snd_freq : from 1 to 10
    * (*beep_args)[2] : beep_sec : from 1 to 10
    */
    ioctl(fd, atoi(argv[1]), beep_args[1]);
    close(fd);

    return 0;
}
#endif
