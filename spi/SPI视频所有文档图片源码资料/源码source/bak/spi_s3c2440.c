#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/slab.h>

#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/spi/s3c24xx.h>
#include <linux/module.h>

#include <plat/regs-spi.h>

#include <plat/fiq.h>
#include <asm/fiq.h>

#if 0  /* mini2440,tq2440 */
#define BUS_NUM  0
#define SPI_IRQ  IRQ_SPI0
#else
#define BUS_NUM  0
#define SPI_IRQ  IRQ_SPI1
#endif

static struct spi_master *spi0_controller;
static struct spi_master *spi1_controller;

struct spi_master_info {
    int irq;
    unsigned int reg_base;
};

static int s3c_spi_setup(struct spi_device *spi)
{
    return 0;
}

static int s3c_spi_transfer(struct spi_device *spi, struct spi_message *mesg)
{
    return 0;
}

static irqreturn_t s3c_spi_irq(int irq, void *dev_id)
{
    return IRQ_HANDLED;
}

static struct spi_master *create_spi_master_s3c2440(int bus_num, int irq, unsigned int reg_base_phy)
{
    struct spi_master *spi_master;
    struct spi_master_info *info;
    
    spi_master = spi_alloc_master(NULL, sizeof(struct spi_master_info));
    spi_master->bus_num = bus_num;
    spi_master->num_chipselect = 0xffff;
    spi_master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;
    spi_master->setup    = s3c_spi_setup;
    spi_master->transfer = s3c_spi_transfer;

    info = spi_master_get_devdata(spi_master);
    info->irq = irq;
    info->reg_base = ioremap(reg_base_phy, 0x18);

    request_irq(irq, s3c_spi_irq, 0, "s3c_spi", spi_master);

    spi_register_master(spi_master);

    return 0;
}

static void destroy_spi_master_s3c2440(struct spi_master *master)
{
    struct spi_master_info *info = spi_master_get_devdata(spi_master);;
    
    spi_unregister_master(master);
    free_irq(info->irq, master);
    iounmap(info->reg_base);
    kfree(spi_master);
}

static int spi_s3c3440_init(void)
{
    spi0_controller = create_spi_master_s3c2440(0, IRQ_SPI0, 0x59000000);
    spi1_controller = create_spi_master_s3c2440(0, IRQ_SPI0, 0x59000020);
    return 0;
}

static void spi_s3c2440_exit(void)
{
    destroy_spi_master_s3c2440(spi0_controller);
    destroy_spi_master_s3c2440(spi1_controller);
}

module_init(spi_s3c3440_init);
module_exit(spi_s3c2440_exit);
MODULE_DESCRIPTION("SPI Controller Driver for s3c440");
MODULE_AUTHOR("weidongshan@qq.com,www.100ask.net");
MODULE_LICENSE("GPL");



