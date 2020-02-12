//[1]
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h> //file_operations
#include <linux/cdev.h> //cdev
#include <asm/gpio.h>
#include <plat/gpio-cfg.h>

//[4]
static dev_t dev; //设备号
static struct cdev led_cdev; //分配字符设备对象

//开灯
//app:open->软中断->sys_open->led_open
static int led_open(struct inode *inode, 
                        struct file *file)
{
    printk("%s\n", __func__);
    gpio_set_value(S5PV210_GPC0(3), 1);
    return 0; //返回成功，失败返回负值
}

//app:close->软中断->sys_close->led_close
static int led_close(struct inode *inode, 
                        struct file *file)
{
    printk("%s\n", __func__);
    gpio_set_value(S5PV210_GPC0(3), 0);
    return 0; //返回成功，失败返回负值
}

static struct file_operations led_fops = {
    .owner = THIS_MODULE, //表示模块的所属者，比如对驱动使用的计数，有两个进程访问,owner.count = 2
    .open = led_open, //开灯
    .release = led_close //关灯
}; //分配初始化硬件操作方法

//[3]
static int led_init(void)
{
    //[5]
    //5.1申请设备号
    alloc_chrdev_region(&dev, 0, 1, "leds");

    //5.2初始化字符设备对象led_cdev
    cdev_init(&led_cdev, &led_fops);//给字符设备对象提供操作硬件方法

    //5.3向内核注册字符设备对象led_cdev
    cdev_add(&led_cdev, dev, 1);

    //5.4申请GPC1_3硬件资源
    gpio_request(S5PV210_GPC0(3), "LED1");
    gpio_direction_output(S5PV210_GPC0(3), 0);
    return 0;
}

static void led_exit(void)
{
    //[6]
    //6.1卸载字符设备对象
    cdev_del(&led_cdev);
    //6.2释放GPIO
    gpio_set_value(S5PV210_GPC0(3), 0);
    gpio_free(S5PV210_GPC0(3));
    //6.3释放设备号
    unregister_chrdev_region(dev, 1);
}
//2
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
