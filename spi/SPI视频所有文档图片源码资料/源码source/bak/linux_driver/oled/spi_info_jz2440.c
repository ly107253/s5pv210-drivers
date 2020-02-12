#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

#include <linux/gpio.h>
#include <linux/io.h>
#include <mach/hardware.h>
#include <mach/regs-gpio.h>

static struct spi_board_info jz2440_info[] = {
	{
		/* spi oled */
		.modalias	   = "oled",          /* 根据此名匹配spi_driver */
		.bus_num       = 1,               /* 根据bus_num匹配spi_master, sc2440 spi controller 1 */
		.max_speed_hz  = 10000000,
		.chip_select   = S3C2410_GPF(1),  /* OLED_CS */
		.mode		   = SPI_MODE_0,
		.platform_data = (const void *)S3C2410_GPG(4),  /* OLED_DC pin */
	},
	{
		/* spi oled */
		.modalias	   = "spi_flash",     /* 根据此名匹配spi_driver */
		.bus_num       = 1,               /* 根据bus_num匹配spi_master, sc2440 spi controller 1 */
		.max_speed_hz  = 80000000,
		.chip_select   = S3C2410_GPG(2),  /* OLED_CS */
		.mode		   = SPI_MODE_0,
	}
};

static int spi_dev_info_init(void)
{
    return spi_register_board_info(jz2440_info, ARRAY_SIZE(jz2440_info));
}

module_init(spi_dev_info_init);

MODULE_LICENSE("GPL");


