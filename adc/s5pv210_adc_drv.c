#include <linux/init.h> 
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/delay.h>
 #include <linux/sched.h>
#define ADCCON		0x00
#define ADCDLY		0x08
#define ADCDATA		0x0c
#define INTADCLR	0x18
#define ADCMUX		0x1c

#define ADC_REL		0x0d
#define ADC_PORT	0x0c

static void *reg_base; //�Ĵ��������ַ

//����ȴ�����ͷ
static wait_queue_head_t adc_wq;

//���ݿ��ñ�־1:���ã�0  :������
static int EndConvert;

static ssize_t adc_read(struct file *file, char __user *buf, 
        size_t count, loff_t *ppos)
{
    unsigned long data; //ת��������

    /*1. ����ADC*/
    writel(readl(reg_base + ADCCON) | 1, 
            reg_base + ADCCON);

    /*2. �ж�ADC�����Ƿ���ã������ã�����
      ���ߣ�ADCת�������������жϣ�����
      ����
      */
    wait_event_interruptible(adc_wq, EndConvert);
    EndConvert = 0;

    /*3. ��ȡ���ݣ�Ȼ���ϱ�*/
    if (readl(reg_base + ADCCON) >> 16 & 1) 
        data = readl(reg_base + ADCDATA) & 0xfff;
    else
        data = readl(reg_base + ADCDATA) & 0x3ff;

    copy_to_user(buf, &data, sizeof(data));

    return count;
}

static int adc_ioctl(struct inode *inode, struct file *file,
        unsigned int cmd, unsigned long arg)
{
    int rel;  //��¼�ֱ���
    int port;//��¼ͨ��

    switch(cmd) {
        case ADC_REL: //���÷ֱ���
            copy_from_user(&rel, (int *)arg, sizeof(rel));
            if (rel == 12) 
                writel(readl(reg_base + ADCCON) | (1 << 16),
                        reg_base + ADCCON);
            else if(rel == 10) 
                writel(readl(reg_base + ADCCON) & ~(1 << 16),
                        reg_base + ADCCON);
            break;

        case ADC_PORT: //����ģ������ͨ��
            copy_from_user(&port, (int *)arg, sizeof(port));
            writel(port, reg_base + ADCMUX);
            break;
        default:
            return -1;
    }
    return 0;
}

/*���䣬��ʼ��fops*/
static struct file_operations adc_fops = {
    .owner = THIS_MODULE,
    .read = adc_read,
    .unlocked_ioctl = adc_ioctl,
};

/*���䣬��ʼ��miscdevice*/
static struct miscdevice adc_misc = {
    .minor = MISC_DYNAMIC_MINOR, //��̬������豸��
    .name = "adc", //dev/s5pv210-adc
    .fops = &adc_fops,
};

//adcת�������������ж�
static irqreturn_t adc_isr(int irq, void *dev_id)
{
    /*1. �������ߵĽ���*/
    EndConvert = 1;
    wake_up_interruptible(&adc_wq);

    /*2. ���ж�*/
    writel(0, reg_base + INTADCLR);

    return IRQ_HANDLED;
}

static int s5pv210_adc_init(void)
{
    unsigned int cur = 0;

    /*1. ע������豸*/
    misc_register(&adc_misc);

    /*2. ��ַӳ��*/
    reg_base = ioremap(0xe1700000, 0x100000);

    /*3. Ӳ����ʼ��*/
    /*3.1 ���÷ֱ���12 ���÷�Ƶϵ��*/
    cur = (1 << 16) | (1 << 14) | (19 << 6);
    writel(cur, reg_base + ADCCON);

    /*3.2 ����ģ�����ͨ��*/
    cur = 1;
    writel(cur, reg_base + ADCMUX);

    /*4. ע���ж�*/
    request_irq(IRQ_ADC, adc_isr, 0, "adc_irq", NULL);

    //5��ʼ���ȴ�����ͷ
    init_waitqueue_head(&adc_wq);
    return 0;
}

static void s5pv210_adc_exit(void)
{
    /*�ͷ��ж�*/
    free_irq(IRQ_ADC, NULL);
    /*ж�ػ����豸*/
    misc_deregister(&adc_misc);
}

module_init(s5pv210_adc_init);
module_exit(s5pv210_adc_exit);




MODULE_LICENSE("GPL");
