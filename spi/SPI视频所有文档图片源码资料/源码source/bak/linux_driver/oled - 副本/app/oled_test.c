
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "oledfont.h"

#define OLED_IOC_INIT        0x10001
#define OLED_IOC_CLEAR       0x10002
#define OLED_IOC_CLEAR_PAGE  0x10003
#define OLED_IOC_SET_POS     0x10004

static void print_usage(char *cmd)
{
    printf("Usage:\n");
    printf("  %s <page> <col> <string>\n", cmd);
    printf("  %s init\n", cmd);
    printf("  %s clear, clear all page\n", cmd);
    printf("  %s clear <page>\n", cmd);
    printf("  %s <page> <col> <string>\n", cmd);
    printf("  0 <= page <=7, 0 <= col <= 127\n");
    printf("  eg. %s 2 1 www.100ask.net\n", cmd);
}

/* page: 0-7
 * col : 0-127
 * 字符: 8x16象素
 */
static void OLEDPutChar(int fd, int page, int col, char c)
{
    int i = 0;
    /* 得到字模 */
    const unsigned char *dots = oled_asc2_8x16[c - ' '];

    /* 发给OLED */
    ioctl(fd, OLED_IOC_SET_POS, page | (col << 8));
    /* 发出8字节数据 */
    write(fd, &dots[i], 8);

    ioctl(fd, OLED_IOC_SET_POS, (page+1) | (col << 8));
    /* 发出8字节数据 */
    write(fd, &dots[i+8], 8);

}

/* page: 0-7
 * col : 0-127
 * 字符: 8x16象素
 */
static void OLEDPrint(int fd, int page, int col, char *str)
{
    int i = 0;
    while (str[i])
    {
        OLEDPutChar(fd, page, col, str[i]);
        col += 8;
        if (col > 127)
        {
            col = 0;
            page += 2;
        }
        i++;
    }
}

int main(int argc, char **argv)
{
    int page;
    int col;
    int fd;
    int ret;

    int do_init = 0;
    int do_clear = 0;
    int do_show = 0;

    if ((argc == 2) && !strcmp(argv[1], "init"))
        do_init = 1;
    if ((argc == 2) && !strcmp(argv[1], "clear"))
        do_clear = 1;
    if ((argc == 3) && !strcmp(argv[1], "clear"))
        do_clear = 1;
    
    if (argc == 4)
        do_show = 1;

    if (!do_clear && !do_show)
    {
        print_usage(argv[0]);
        return -1;
    }

    fd = open("/dev/oled", O_RDWR);
    if (fd < 0)
    {
        printf("can't open /dev/oled\n");
        return -1;
    }

    if (do_init)
        ret = ioctl(fd, OLED_IOC_INIT);        
    else if (do_clear)
    {
        if (argc == 2)
        {
            ret = ioctl(fd, OLED_IOC_CLEAR);
        }
        else
        {
            page = strtoul(argv[2], NULL, 0);
            ret = ioctl(fd, OLED_IOC_CLEAR_PAGE, page);
        }
    }
    else if (do_show)
    {
        page = strtoul(argv[1], NULL, 0);
        col = strtoul(argv[2], NULL, 0);
        if ((page < 0 || page > 7) || (col < 0 || col > 127))
        {
            printf("Error! Parameters should be: 0 <= page <=7, 0 <= col <= 127\n");
            return -1;
        }

        ret = ioctl(fd, OLED_IOC_CLEAR_PAGE, page);
        ret = ioctl(fd, OLED_IOC_CLEAR_PAGE, page+1);

        OLEDPrint(fd, page, col, argv[3]);
    }
    return 0;
}

