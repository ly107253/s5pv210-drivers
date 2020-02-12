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

//分配工作和延时工作对象
static struct work_struct work; //普通工作
static struct delayed_work dwork;//延时工作

//工作处理函数
//work指针指向分配普通工作对象
static void work_function(struct work_struct *work)
{
    printk("%s\n", __func__); 
}

//延时工作处理函数
//work指针指向延时工作dwork的第一个成员，也就是指向延时工作对象
static void dwork_function(struct work_struct *work)
{
    printk("%s\n", __func__);
}

//中断处理函数（顶半部）
static irqreturn_t button_isr(int irq, void *dev_id)
{
    //登记普通工作,CPU在适当的时候去执行
    //schedule_work(&work);

    //登记延时工作，指定登记延时时间为5秒,5秒以后登记
    schedule_delayed_work(&dwork, 5*HZ);
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
    
    //初始化工作和延时工作，指定它们的处理函数
    INIT_WORK(&work, work_function);
    INIT_DELAYED_WORK(&dwork, dwork_function);
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
