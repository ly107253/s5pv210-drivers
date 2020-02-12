#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>

#include <linux/spi/spi.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <mach/hardware.h>
#include <mach/regs-gpio.h>


/* 参考drivers/spi/spidev.c */

static int major;
static struct class *oled_class;
static struct device *oled_device;
static struct spi_device *spi_oled_dev;
static int oled_dc_pin;

static unsigned char *spi_buffer;

#define OLED_IOC_INIT        0x10001
#define OLED_IOC_CLEAR       0x10002
#define OLED_IOC_CLEAR_PAGE  0x10003
#define OLED_IOC_SET_POS     0x10004

static void OLED_Set_DC(char val)
{
    if (val)
        s3c2410_gpio_setpin(oled_dc_pin, 1);
    else
        s3c2410_gpio_setpin(oled_dc_pin, 0);
}

static void SPISendByte(unsigned char val)
{
    spi_write(spi_oled_dev, &val, 1);
}

static void OLEDWriteCmd(unsigned char cmd)
{
    OLED_Set_DC(0); /* command */

    SPISendByte(cmd);

    OLED_Set_DC(1); /*  */
}

static void OLEDWriteDat(unsigned char dat)
{
    OLED_Set_DC(1); /* data */

    SPISendByte(dat);

    OLED_Set_DC(1); /*  */
}


static void OLEDSetPageAddrMode(void)
{
    OLEDWriteCmd(0x20);
    OLEDWriteCmd(0x02);
}

static void OLEDSetPos(int page, int col)
{
    OLEDWriteCmd(0xB0 + page); /* page address */

    OLEDWriteCmd(col & 0xf);   /* Lower Column Start Address */
    OLEDWriteCmd(0x10 + (col >> 4));   /* Lower Higher Start Address */
}


static void OLEDClear(void)
{
    int page, i;
    for (page = 0; page < 8; page ++)
    {
        OLEDSetPos(page, 0);
        for (i = 0; i < 128; i++)
            OLEDWriteDat(0);
    }
}

static void OLEDInit(void)
{
    /* 向OLED发命令以初始化 */
    OLEDWriteCmd(0xAE); /*display off*/ 
    OLEDWriteCmd(0x00); /*set lower column address*/ 
    OLEDWriteCmd(0x10); /*set higher column address*/ 
    OLEDWriteCmd(0x40); /*set display start line*/ 
    OLEDWriteCmd(0xB0); /*set page address*/ 
    OLEDWriteCmd(0x81); /*contract control*/ 
    OLEDWriteCmd(0x66); /*128*/ 
    OLEDWriteCmd(0xA1); /*set segment remap*/ 
    OLEDWriteCmd(0xA6); /*normal / reverse*/ 
    OLEDWriteCmd(0xA8); /*multiplex ratio*/ 
    OLEDWriteCmd(0x3F); /*duty = 1/64*/ 
    OLEDWriteCmd(0xC8); /*Com scan direction*/ 
    OLEDWriteCmd(0xD3); /*set display offset*/ 
    OLEDWriteCmd(0x00); 
    OLEDWriteCmd(0xD5); /*set osc division*/ 
    OLEDWriteCmd(0x80); 
    OLEDWriteCmd(0xD9); /*set pre-charge period*/ 
    OLEDWriteCmd(0x1f); 
    OLEDWriteCmd(0xDA); /*set COM pins*/ 
    OLEDWriteCmd(0x12); 
    OLEDWriteCmd(0xdb); /*set vcomh*/ 
    OLEDWriteCmd(0x30); 
    OLEDWriteCmd(0x8d); /*set charge pump enable*/ 
    OLEDWriteCmd(0x14); 

    OLEDSetPageAddrMode();

    OLEDClear();
    
    OLEDWriteCmd(0xAF); /*display ON*/    
}


void OLEDClearPage(int page)
{
    int i;
    OLEDSetPos(page, 0);
    for (i = 0; i < 128; i++)
        OLEDWriteDat(0);    
}



static ssize_t oled_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
    int i;
    int ret;
    
    if (count > 4096)
        return -EINVAL;
    
    ret = copy_from_user(spi_buffer, buf, count);
    
    for (i = 0; i < count; i++)
    {
        OLEDWriteDat(spi_buffer[i]);
    }
    return count;
}

static long oled_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int page;
    int col;
    
    switch (cmd)
    {
        case OLED_IOC_INIT :
        {
            printk("oled init ...\n");
            OLEDInit();
            break;
        }

        case OLED_IOC_CLEAR :
        {
            printk("oled clear ...\n");
            OLEDClear();
            break;
        }

        case OLED_IOC_CLEAR_PAGE :
        {
            page = arg;
            if (page > 8)
                return -EINVAL;
            printk("oled clear page ...\n");
            OLEDClearPage(page);
            break;
        }

        case OLED_IOC_SET_POS :
        {
            page = arg & 0xff;
            col  = (arg >> 8) & 0xff;
            OLEDSetPos(page, col);
            break;
        }
    }
    
    return 0;
}

static const struct file_operations oled_fops = {
	.owner = THIS_MODULE,

	.write = oled_write,
	.unlocked_ioctl = oled_ioctl,
};

static int oled_probe(struct spi_device *spi)
{
    spi_oled_dev = spi;
    
    oled_dc_pin = (int)spi->dev.platform_data;

    s3c2410_gpio_cfgpin(oled_dc_pin, S3C2410_GPIO_OUTPUT);
    s3c2410_gpio_cfgpin(spi->chip_select, S3C2410_GPIO_OUTPUT);
        
	major = register_chrdev(0, "oled", &oled_fops);

	oled_class = class_create(THIS_MODULE, "oled_cls");

	/* 为了让mdev根据这些信息来创建设备节点 */
	oled_device = device_create(oled_class, NULL, MKDEV(major, 0), NULL, "oled"); /* /dev/oled */

    spi_setup(spi);
    
	return 0;
	
}

static int oled_remove(struct spi_device *spi)
{
	unregister_chrdev(major, "oled");
	device_destroy(oled_class, MKDEV(major, 0));
	class_destroy(oled_class);
	return 0;
}

static struct spi_driver oled_spi_driver = {
	.driver = {
		.name =		"oled",
		.owner =	THIS_MODULE,
	},
	.probe  =	oled_probe,
	.remove =	oled_remove,
};

static int oled_init(void)
{
	spi_buffer = kmalloc(4094, GFP_KERNEL);
	return spi_register_driver(&oled_spi_driver);
}

static void oled_exit(void)
{
    kfree(spi_buffer);
	spi_unregister_driver(&oled_spi_driver);
}

module_init(oled_init);
module_exit(oled_exit);

MODULE_AUTHOR("weidongshan@qq.com, www.100ask.net");
MODULE_LICENSE("GPL");

