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
#include <linux/mtd/mtd.h>


/* ²Î¿¼ drivers\mtd\devices\m25p80.c 
        drivers\mtd\devices\mtdram.c
*/

static struct mtd_info spi_flash_mtd;
static struct spi_device *spi_flash_dev;

static void SPIFlashSendAddr(unsigned int addr)
{
    unsigned char buf[3];
    buf[0] = addr >> 16;
    buf[1] = addr >> 8;
    buf[2] = addr;
    spi_write(spi_flash_dev, buf, 3);
}

/* 
 * 
 */
void SPIFlashReadID(int *pMID, int *pDID)
{
    unsigned char tx_buf[4];
    unsigned char rx_buf[2];

    tx_buf[0] = 0x90;
    tx_buf[1] = 0;
    tx_buf[2] = 0;
    tx_buf[3] = 0;

    spi_write_then_read(spi_flash_dev, tx_buf, 4, rx_buf, 2);
    *pMID = rx_buf[0];
    *pDID = rx_buf[1];
}

static void SPIFlashWriteEnable(int enable)
{
    unsigned char val;

    val = enable ? 0x06 : 0x04;

    spi_write(spi_flash_dev, &val, 1);
}

static unsigned char SPIFlashReadStatusReg1(void)
{
    unsigned char tx_buf[1];
    unsigned char rx_buf[1];

    tx_buf[0] = 0x05;

    spi_write_then_read(spi_flash_dev, tx_buf, 1, rx_buf, 1);
    return rx_buf[0];
}

static unsigned char SPIFlashReadStatusReg2(void)
{
    unsigned char tx_buf[1];
    unsigned char rx_buf[1];

    tx_buf[0] = 0x35;

    spi_write_then_read(spi_flash_dev, tx_buf, 1, rx_buf, 1);
    return rx_buf[0];

}

static void SPIFlashWaitWhenBusy(void)
{
    /* SPIFlashWaitWhenBusy : 60ms
     * SPIFlashWaitWhenBusy : 0.7ms
     * SPIFlashWaitWhenBusy : 10ms
     */
    while (SPIFlashReadStatusReg1() & 1)
    {
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(HZ/100); /* 10ms */
    }
}

static void SPIFlashWriteStatusReg(unsigned char reg1, unsigned char reg2)
{    
    unsigned char buf[3];
    buf[0] = 0x01;
    buf[1] = reg1;
    buf[2] = reg2;

    SPIFlashWriteEnable(1);  
    spi_write(spi_flash_dev, buf, 3);

    SPIFlashWaitWhenBusy();
}

static void SPIFlashClearProtectForStatusReg(void)
{
    unsigned char reg1, reg2;

    reg1 = SPIFlashReadStatusReg1();
    reg2 = SPIFlashReadStatusReg2();

    reg1 &= ~(1<<7);
    reg2 &= ~(1<<0);

    SPIFlashWriteStatusReg(reg1, reg2);
}


static void SPIFlashClearProtectForData(void)
{
    /* cmp=0,bp2,1,0=0b000 */
    unsigned char reg1, reg2;

    reg1 = SPIFlashReadStatusReg1();
    reg2 = SPIFlashReadStatusReg2();

    reg1 &= ~(7<<2);
    reg2 &= ~(1<<6);

    SPIFlashWriteStatusReg(reg1, reg2);
}

/* erase 4K */
void SPIFlashEraseSector(unsigned int addr)
{
    unsigned char buf[4];
    buf[0] = 0x20;
    buf[1] = addr >> 16;
    buf[2] = addr >> 8;
    buf[3] = addr;

    SPIFlashWriteEnable(1);  
    spi_write(spi_flash_dev, buf, 4);

    SPIFlashWaitWhenBusy();
}

/* program */
void SPIFlashProgram(unsigned int addr, unsigned char *buf, int len)
{
    unsigned char tx_buf[4];
    
#if 0
    tx_buf[0] = 0x02;
    tx_buf[1] = addr >> 16;
    tx_buf[2] = addr >> 8;
    tx_buf[3] = addr;

    SPIFlashWriteEnable(1);  
    spi_write(spi_flash_dev, tx_buf, 4);
    spi_write(spi_flash_dev, buf, len);
#else
	struct spi_transfer t[] = {
		{
			.tx_buf = tx_buf,
			.len = 4,
		}, {
			.tx_buf = buf,
			.len = len,
		},
	};
	struct spi_message m;

    tx_buf[0] = 0x02;
    tx_buf[1] = addr >> 16;
    tx_buf[2] = addr >> 8;
    tx_buf[3] = addr;

    SPIFlashWriteEnable(1);  

	spi_message_init(&m);
	spi_message_add_tail(&t[0], &m);
	spi_message_add_tail(&t[1], &m);

    spi_sync(spi_flash_dev, &m);

#endif
    SPIFlashWaitWhenBusy();
    
}

void SPIFlashRead(unsigned int addr, unsigned char *buf, int len)
{
    unsigned char tx_buf[4];

	struct spi_transfer t[] = {
		{
			.tx_buf = tx_buf,
			.len = 4,
		}, {
			.rx_buf = buf,
			.len = len,
		},
	};
	struct spi_message m;
    
    tx_buf[0] = 0x03;
    tx_buf[1] = addr >> 16;
    tx_buf[2] = addr >> 8;
    tx_buf[3] = addr;

	spi_message_init(&m);
	spi_message_add_tail(&t[0], &m);
	spi_message_add_tail(&t[1], &m);

    spi_sync(spi_flash_dev, &m);

    //spi_write_then_read(spi_flash_dev, tx_buf, 4, buf, len);    
}


void SPIFlashInit(void)
{
    SPIFlashClearProtectForStatusReg();
    SPIFlashClearProtectForData();
}

static int spi_flash_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	//memset((char *)mtd->priv + instr->addr, 0xff, instr->len);
	int len;
    unsigned int addr = instr->addr;

    //printk("spi_flash_erase : addr = 0x%x, len = 0x%x\n", addr, (unsigned int)instr->len);
    if ((instr->len & (spi_flash_mtd.erasesize - 1)) || (instr->addr & (spi_flash_mtd.erasesize - 1)))
    {
        printk("spi_flash_erase error, addr or len is not aligned!\n");
        return -EINVAL;
    }
    
    for (len = 0; len < instr->len; len += spi_flash_mtd.erasesize)
    {
        SPIFlashEraseSector(addr);
        addr += spi_flash_mtd.erasesize;
    }
	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);
	return 0;
}

static int spi_flash_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf)
{
	//memcpy(buf, mtd->priv + from, len);
    //printk("spi_flash_read : addr = 0x%x, len = 0x%x\n", (unsigned int)from, len);
    SPIFlashRead(from, buf, len);
	*retlen = len;
	return 0;
}

static int spi_flash_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, const u_char *buf)
{
	unsigned int wlen;

	//memcpy((char *)mtd->priv + to, buf, len);
    //printk("spi_flash_write : addr = 0x%x, len = 0x%x\n", (unsigned int)to, len);
    if ((len & (spi_flash_mtd.erasesize - 1)) || (to & (spi_flash_mtd.erasesize - 1)))
    {
        printk("spi_flash_write error, addr or len is not aligned!\n");
        return -EINVAL;
    }

    for (wlen = 0; wlen < len; wlen += 256)
    {
    	SPIFlashProgram(to, (unsigned char *)buf, 256);
        to += 256;
        buf += 256;
    }
	*retlen = len;
	return 0;
}

static int spi_flash_probe(struct spi_device *spi)
{    
    unsigned char str[256];
    unsigned char str2[256];
    int i;
    int mid, did;
    printk("spi_flash_probe ...\n");

    s3c2410_gpio_cfgpin(spi->chip_select, S3C2410_GPIO_OUTPUT);

    memset(&spi_flash_mtd, 0, sizeof(spi_flash_mtd));

    spi_flash_dev = spi;

    SPIFlashReadID(&mid, &did);
    printk("SPI Flash ID: %02x %02x\n", mid, did);

    SPIFlashInit();
#if 0
#define LEN 256
    SPIFlashEraseSector(4096);
    SPIFlashProgram(4096, str, LEN);
    SPIFlashRead(4096, str2, LEN);
    if (!memcmp(str, str2, LEN))
        printk("Test spi flash, OK!\n");
    else
    {
        printk("Test spi flash, error!\n");
    }
    printk("str : ");
    for (i = 0; i < LEN; i++)
        printk("%02x ", str[i]);
    printk("\n");

    printk("str2: ");
    for (i = 0; i < LEN; i++)
        printk("%02x ", str2[i]);
    printk("\n");
#endif
	/* Setup the MTD structure */
	spi_flash_mtd.name = "100ask_spi_flash";
	spi_flash_mtd.type = MTD_NORFLASH;
	spi_flash_mtd.flags = MTD_CAP_NORFLASH;
	spi_flash_mtd.size = 0x200000; /* 2M Byte */
	spi_flash_mtd.writesize = 1;
	spi_flash_mtd.writebufsize = 4*1024;
	spi_flash_mtd.erasesize = 4*1024;

	spi_flash_mtd.owner = THIS_MODULE;
	spi_flash_mtd._erase = spi_flash_erase;
	spi_flash_mtd._read  = spi_flash_read;
	spi_flash_mtd._write = spi_flash_write;

	if (mtd_device_register(&spi_flash_mtd, NULL, 0))
		return -EIO;
    
	return 0;
}

static int spi_flash_remove(struct spi_device *spi)
{
    mtd_device_unregister(&spi_flash_mtd);
	return 0;
}

static struct spi_driver spi_flash_driver = {
	.driver = {
		.name =		"spi_flash",
		.owner =	THIS_MODULE,
	},
	.probe  =	spi_flash_probe,
	.remove =	spi_flash_remove,
};

static int spi_flash_init(void)
{
	return spi_register_driver(&spi_flash_driver);
}

static void spi_flash_exit(void)
{
	spi_unregister_driver(&spi_flash_driver);
}

module_init(spi_flash_init);
module_exit(spi_flash_exit);

MODULE_AUTHOR("weidongshan@qq.com, www.100ask.net");
MODULE_LICENSE("GPL");

