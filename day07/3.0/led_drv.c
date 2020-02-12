#include <linux/init.h>
#include <linux/module.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

//分配延时工作对象
static struct delayed_work dwork;//延时工作

//延时工作处理函数
//work指针指向延时工作dwork的第一个成员，也就是指向延时工作对象
static void dwork_function(struct work_struct *work)
{
    //开关灯,如果是开，你就关，如果是关，你就开，不允许使用if...else来判断
    gpio_set_value(S5PV210_GPC1(3), 
            gpio_get_value(S5PV210_GPC1(3))^1);
    //重新登记
    schedule_delayed_work(&dwork, 2*HZ);
}

static int led_init(void)
{
    //1.申请GPIO资源，配置GPIO为输出口，输出0
    gpio_request(S5PV210_GPC1(3), "LED1");
    gpio_direction_output(S5PV210_GPC1(3), 0);
    //2.初始化延时工作，指定它们的处理函数
    INIT_DELAYED_WORK(&dwork, dwork_function);
    schedule_delayed_work(&dwork, 2*HZ);
    return 0;
}

static void led_exit(void)
{
    gpio_set_value(S5PV210_GPC1(3), 0);
    gpio_free(S5PV210_GPC1(3));
   cancel_delayed_work(&dwork); //取消延时工作
}
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
