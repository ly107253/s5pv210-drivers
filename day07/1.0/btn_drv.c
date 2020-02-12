#include <linux/init.h>
#include <linux/module.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

//定义硬件相关数据结构
struct btn_resource {
    int irq;    //中断号
    char *name; //中断名称
};
//分配初始化按键信息
static struct btn_resource btn_info[] = {
    [0] = {
        .irq = IRQ_EINT(0),
        .name = "KEY_UP"
    },
    [1] = {
        .irq = IRQ_EINT(1),
        .name = "KEY_DOWN"
    }
};

static int mydata = 0x5555;

static void btn_tasklet_func(unsigned long data)
{
    //数据类型转换
    int *p = (int *)data;
    printk("底半部:%s:data = %#x\n", __func__, *p);
}

//1.分配初始化tasklet对象
//第一个参数是tasklet对象名
//第二个参数是tasklet延后处理函数
//第三个参数是给延后处理函数传递的参数
static DECLARE_TASKLET(btn_tasklet, 
                        btn_tasklet_func,
                        (unsigned long)&mydata);

//中断处理函数（顶半部）
static irqreturn_t button_isr(int irq, void *dev_id)
{
    //2.登记底半部tasklet
    tasklet_schedule(&btn_tasklet);
    
    printk("顶半部:%s\n", __func__);
    return IRQ_HANDLED;
}

static int btn_init(void)
{
    int i;

    //申请硬件中断资源和注册中断处理函数
    for (i = 0; i < ARRAY_SIZE(btn_info); i++)
        request_irq(btn_info[i].irq,
                    button_isr,
        IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
                    btn_info[i].name,
                    &btn_info[i]);
    return 0;
}

static void btn_exit(void)
{
    int i;

    //释放硬件中断资源和卸载中断处理函数
    for (i = 0; i < ARRAY_SIZE(btn_info); i++)
        free_irq(btn_info[i].irq,&btn_info[i]);

}
module_init(btn_init);
module_exit(btn_exit);
MODULE_LICENSE("GPL");
