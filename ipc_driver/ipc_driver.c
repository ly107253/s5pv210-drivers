#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/delay.h>

#define __IS_KERNEL__ 		1
#include "mempool.h"

//#define DEBUG
#ifdef DEBUG
#define PLOG(fmt,args...) printk(" ly_ipc "fmt,##args)
#else
#define PLOG(fmt,args...)
#endif

#define IPCDRV_MAJOR            0
#define IPCDRV_MINOR            0
#define IPCDRV_SMALL            1
#define IPCDRV_STATE            'R'		///< R-Release(发布状态) D-Debug(调试状态)

#define _SKIP_SIZE 				64     //空闲大小


typedef struct {
	unsigned int 		len;
} msghead_t;

typedef struct {
	struct list_head 	list;
	msghead_t			head;
} list_msghead_t;

typedef struct {
	struct mutex  		lock;
	wait_queue_head_t 	waitq_read;
	unsigned int 		maxreadlen;
	MemoryPool 			pool;
	struct list_head 	msglist;
} msgque_t;

/**
* @brief 消息发送处理
* @param isHighPrio: 0-高优先级
*/
static inline void queue_push(msgque_t *pque, list_msghead_t *pmsg, int isHighPrio)
{
	if(isHighPrio)
	{
		list_add(&pmsg->list, &pque->msglist);
	}
	else
	{
		list_add_tail(&pmsg->list, &pque->msglist);
	}
}

static inline list_msghead_t *queue_pop(msgque_t *pque)
{
	list_msghead_t *pmsg;

	if(list_empty(&pque->msglist))
	{
		return NULL;
	}

	pmsg = (list_msghead_t *)pque->msglist.next;
	list_del(&pmsg->list);
	return pmsg;
}

/**
* @brief 读消息或事件
*/
static int read_message(msgque_t *pque, void __user *buf, int len, msghead_t *phead)
{
	list_msghead_t *plist;

 	for(;;)
 	{
		plist = queue_pop(pque);
		if(NULL == plist)
		{
			return -EFAULT;
		}

		PLOG("queue_pop:que=0x%08x, plist=0x%08x\r\n", (unsigned int)pque, (unsigned int)plist);

		if(plist->head.len > len || plist->head.len < 0)
		{
			PLOG("discard large memory\n");
			MemPool_Free(&pque->pool, plist);
			continue;
		}

		if(plist->head.len)
		{
			if(copy_to_user(buf, plist+1, plist->head.len))
			{
				queue_push(pque, plist, 1);
				return -EFAULT;
			}
		}
		*phead = plist->head;
 		MemPool_Free(&pque->pool, plist);
		return 0;
 	}

	return -EFAULT;
}

/**
* @brief 写消息
* @param tag: 1~32(事件)
* @param isHighPrio: 0-高优先级
*/
static int write_message(msgque_t *pque, const msghead_t *phead, void __user *buf)
{
	list_msghead_t *plist;

	if(NULL == buf)
	{
		return -EFAULT;
	}

	plist = MemPool_Alloc(&pque->pool, phead->len+sizeof(list_msghead_t));
	if(NULL == plist)
	{
		return -ENOMEM;
	}

	plist->head = *phead;
	if(copy_from_user(plist+1, buf, phead->len))	///< 消息拷贝
	{
		MemPool_Free(&pque->pool, plist);
		return -EFAULT;
	}

	PLOG("queue_push:que=0x%08x, plist=0x%08x\r\n", (unsigned int)pque, (unsigned int)plist);
	
	queue_push(pque, plist, 0);

	return 0;
}

static void msgque_init(msgque_t *pque, unsigned int maxsize, unsigned int maxreadlen)
{
	memset(pque, 0, sizeof(msgque_t));
	mutex_init(&pque->lock);
	init_waitqueue_head(&pque->waitq_read);
	pque->maxreadlen = maxreadlen;
	MemPool_Init(&pque->pool, maxsize+PAGE_SIZE);
	INIT_LIST_HEAD(&pque->msglist);
}

/**
* @brief 唤醒所有进程后 释放相应内存资源
*/
static void msgque_destroy(msgque_t *pque)
{
	if(!list_empty(&pque->waitq_read.task_list))
	{
		wake_up_interruptible_all(&pque->waitq_read);
	}
	MemPool_Destroy(&pque->pool);	///< 释放内存
}

#define LYIPC_DRVNAME   	"ly_ipc"
#define LYIPC_NR_DEVS   	1
#define LYIPC_MAX_QUE 		64		///< 最大支持64个消息队列
#define LYIPC_MAXQUENAME 	16		///< 队列名称最大支持32个字符

#define LYIPC_INITSERCNT	100		///< 队列ID高位序号
typedef struct
{
	struct cdev  		cdev;
	dev_t 				devt;
	struct class * 		class;
	struct device * 	dev;

	struct mutex 		glock;		///< 全局锁
	int 				serial_cnt;
	struct {
		int 			handle;						///< 队列句柄 0-无效
		char 			name[LYIPC_MAXQUENAME];		///< 队列名称
		msgque_t * 		pque;						///< 消息
	} channel[LYIPC_MAX_QUE];
} lyipc_dev_t;

static int lyipc_major = 0;		        // 设备号
static lyipc_dev_t *lyipc_dev;

#define LYIPC_CMD_CREATE_CHANNEL 		100 	// 创建通道
#define LYIPC_CMD_DESTROY_CHANNEL 		101 	// 销毁通道
#define LYIPC_CMD_QUERY_CHANNEL 		102 	// 查询通道
#define LYIPC_CMD_WRITE_MESSAGE 		103 	// 写异步消息
#define LYIPC_CMD_READ_MESSAGE 			104 	// 读消息
#define LYIPC_CMD_QRY_QUEINFO			105		// 读取通道信息
#define LYIPC_CMD_READ_PEND 			106 	// 读等待 

struct chan_ctrl_create {
	char 			name[LYIPC_MAXQUENAME]; // 通道名称
	unsigned int 	maxsize; 				// 最大缓存长度
	unsigned int 	maxreadlen; 			// 最大读取长度
};

struct chan_ctrl_query {
	char 			name[LYIPC_MAXQUENAME]; // 通道名称
};

struct chan_ctrl_rw {
	int 			handle; 				// 通道句柄
	unsigned int 	len; 					// 消息长度(写), 缓存区长度(读)
	void __user * 	buf; 					// 接收或发送缓存
};

struct chan_ctrl_queinfo {
	int 			handle; 				// 通道句柄
	unsigned int 	maxSize; 				// 队列最大内存
	unsigned int 	usedSize; 				// 队列已使用内存
};

/**
* @brief 查找队列
* @return 0~63:有效队列序号 -1:无有效队列
*/
static int find_channel(lyipc_dev_t *pdev, const char *name)
{
	int i, j;
	char *pdevname;

	for(i=0; i<LYIPC_MAX_QUE; i++)
	{
		///< 队列无效
		if(NULL == pdev->channel[i].pque || pdev->channel[i].handle <= 0)
		{
			continue;
		}

		///< 通过队列名查找
		pdevname = pdev->channel[i].name;
		for(j=0; j<LYIPC_MAXQUENAME; j++)
		{
			if(name[j] == 0 || pdevname[j] == 0)
			{
				break;
			}
			else if(name[j] != pdevname[j])
			{
				break;
			}
		}
		if(j >= LYIPC_MAXQUENAME || (name[j] == 0 && pdevname[j] == 0))
		{
			return i;
		}
	}

	return -1;
}

/**
* @brief 建立消息队列通道
* @return 成功-返回通道句柄
*/
static int create_channel(lyipc_dev_t *pdev, struct chan_ctrl_create *pctrl)
{
	int id, handle;

	id = find_channel(pdev, pctrl->name);
	if(id >= 0)		///< 队列存在
	{
		return -EFAULT;
	}

	for(id=0; id<LYIPC_MAX_QUE; id++)
	{
		if(NULL == pdev->channel[id].pque)
		{
			break;
		}
	}
	if(id >= LYIPC_MAX_QUE)
	{
		return -ENOMEM;
	}

	pdev->channel[id].pque = kmalloc(sizeof(msgque_t), GFP_KERNEL);
	if(NULL == pdev->channel[id].pque)
	{
		return -ENOMEM;
	}
	msgque_init(pdev->channel[id].pque, pctrl->maxsize, pctrl->maxreadlen);

	handle = (pdev->serial_cnt++)&0x7ff;
	handle <<= 16;
	handle += id + 0x10000000;

	pdev->channel[id].handle = handle;
	memcpy(pdev->channel[id].name, pctrl->name, LYIPC_MAXQUENAME);
	
	return handle;
}

/**
* @brief 注销设备
*/
static int destroy_channel(lyipc_dev_t *pdev, int handle)
{
	int id = handle & 0xffff;
	msgque_t *pque;

	if(handle <= 0 || id >= LYIPC_MAX_QUE)
	{
		return -EFAULT;
	}

	if(pdev->channel[id].handle != handle || NULL == pdev->channel[id].pque)
	{
		return -EFAULT;
	}

	pque = pdev->channel[id].pque;

	pdev->channel[id].pque = NULL;	
	pdev->channel[id].handle = 0;
	memset(pdev->channel[id].name, 0, LYIPC_MAXQUENAME);

	mutex_lock(&pque->lock);
	msgque_destroy(pque);
	mutex_unlock(&pque->lock);
	kfree(pque);

	return 0;
}

/**
* @brief 读阻塞条件
* @note 消息为空
*/
static int have_message(lyipc_dev_t *pdev, int id, int handle)
{
	msgque_t *pque = pdev->channel[id].pque;

	if(NULL == pque || pdev->channel[id].handle != handle)	///< 条件??
	{
		return -1;
	}

	else if(!list_empty(&pque->msglist))
	{
		return 1;
	}

	return 0;
}

/**
* @brief 读消息
*/
static int read_channel(lyipc_dev_t *pdev, struct chan_ctrl_rw *pctrl, int isblock)
{
	int handle = pctrl->handle;
	int id = handle & 0xffff;
	msgque_t *pque;
	int havemsg, rtn;
	msghead_t msg;
	
	if(NULL == pctrl->buf)
	{
		PLOG("read_channel:failed,line:%s\r\n", __LINE__);
		return -EFAULT;
	}

	if(id < 0 || id >= LYIPC_MAX_QUE)
	{
		PLOG("read_channel:failed,line:%s\r\n", __LINE__);
		return -EFAULT;
	}
	
	if(pdev->channel[id].handle != pctrl->handle || NULL == pdev->channel[id].pque)
	{
		PLOG("read_channel:failed,line:%s\r\n", __LINE__);
		return -EFAULT;
	}
	
	pque = pdev->channel[id].pque;
	if(pctrl->len < (int)pque->maxreadlen)
	{
		PLOG("read_channel:failed,line:%s\r\n", __LINE__);
		return -EFAULT;
	}

	mutex_lock(&pque->lock);
mark_continue:
	havemsg = have_message(pdev, id, handle);
	if(havemsg < 0)
	{
		mutex_unlock(&pque->lock);
		PLOG("read_channel:failed,line:%s\r\n", __LINE__);
		return -EFAULT;
	}
	else if(0 == havemsg)
	{
		if(isblock)
		{
			mutex_unlock(&pque->lock);
			if(wait_event_interruptible(pque->waitq_read, (havemsg=have_message(pdev, id, handle)) != 0))
			{
				PLOG("read_channel:failed,line:%s\r\n", __LINE__);
				return -ERESTARTSYS;
			}
			if(havemsg < 0)
			{
				PLOG("read_channel:failed,line:%s\r\n", __LINE__);
				return -EFAULT;
			}
			mutex_lock(&pque->lock);
			goto mark_continue;
		}
		else
		{
			mutex_unlock(&pque->lock);
			PLOG("read_channel:failed,line:%s\r\n", __LINE__);
			return -EWOULDBLOCK;
		}
	}
	
	msg.len = 0;
	rtn = read_message(pque, pctrl->buf, pctrl->len, &msg);
	pctrl->len = msg.len;
	mutex_unlock(&pque->lock);
	return rtn;	
}

/**
* @brief 写消息
*/
static int write_channel(lyipc_dev_t *pdev, struct chan_ctrl_rw *pctrl)
{
	int handle = pctrl->handle;
	int id = handle & 0xffff;
	msgque_t *pque;
	int rtn, memlen;
	msghead_t msg;
	///< 参数检查
	if(id < 0 || id >= LYIPC_MAX_QUE)
	{
		return -EFAULT;
	}

	if(pdev->channel[id].handle != pctrl->handle || NULL == pdev->channel[id].pque)
	{
		return -EFAULT;
	}

	pque = pdev->channel[id].pque;
	memlen = pctrl->len + sizeof(list_msghead_t);
	if(memlen > (int)(pque->pool.nMaxBufSize-_SKIP_SIZE*2) || pctrl->len > (int)pque->maxreadlen)
	{
		return -EFAULT;
	}
	msg.len = pctrl->len;	

	mutex_lock(&pque->lock);
	rtn = write_message(pque, &msg, pctrl->buf);
	if(rtn == -ENOMEM)
	{
		mutex_unlock(&pque->lock);
		return -EWOULDBLOCK;	
	}
	
	if(0 == rtn)
	{
		if(!list_empty(&pque->waitq_read.task_list))	///< 唤醒读进程
		{
			wake_up_interruptible(&pque->waitq_read);
		}
	}

 	mutex_unlock(&pque->lock);
	return rtn; 
}

/**
*@brief 获取队列信息
*/
static int query_channel(lyipc_dev_t *pdev, struct chan_ctrl_queinfo *pctrl)
{
	int id = pctrl->handle & 0xffff;
	msgque_t *pque;

	if(id < 0 || id >= LYIPC_MAX_QUE)
	{
		return -EFAULT;
	}

	if(pdev->channel[id].handle != pctrl->handle || NULL == pdev->channel[id].pque)
	{
		return -EFAULT;
	}

	pque = pdev->channel[id].pque;

	pctrl->maxSize       = pque->pool.nMaxBufSize;
	pctrl->usedSize      = pque->pool.nUsedSize;

	return 0;
 }

static long lyipc_ioctl(struct file *file, u_int cmd, u_long arg)
{
	void __user *argp = (void __user *)arg;
	lyipc_dev_t *pdev = lyipc_dev;
	int rtn;

	//printk("ioctl cmd: %d\n", cmd);
	switch(cmd)
	{
	case LYIPC_CMD_CREATE_CHANNEL:		///< 创建通道
		{
			struct chan_ctrl_create ctrl;

			if(copy_from_user(&ctrl, argp, sizeof(ctrl)))
			{
				return -EFAULT;
			}
			ctrl.name[LYIPC_MAXQUENAME-1] = 0;
			if(0 == ctrl.name[0])
			{
				return -EFAULT;
			}
			if(ctrl.maxreadlen < 32 || ctrl.maxsize < 32)
			{
				return -EFAULT;
			}

			mutex_lock(&pdev->glock);
			rtn = create_channel(pdev, &ctrl);
			mutex_unlock(&pdev->glock);
			return rtn;
		}
		break;

	case LYIPC_CMD_DESTROY_CHANNEL:		///< 销毁通道
		{
			int handle = (int)arg;

			mutex_lock(&pdev->glock);
			rtn = destroy_channel(pdev, handle);
			mutex_unlock(&pdev->glock);
			return rtn;
		}
		break;

	case LYIPC_CMD_QUERY_CHANNEL:		///< 查询通道
		{
			struct chan_ctrl_query ctrl;

			if(copy_from_user(&ctrl, argp, sizeof(ctrl)))
			{
				return -EFAULT;
			}
			ctrl.name[LYIPC_MAXQUENAME-1] = 0;
			if(0 == ctrl.name[0])
			{
				return -EFAULT;
			}
			mutex_lock(&pdev->glock);
			rtn = find_channel(pdev, ctrl.name);
			if(rtn >= 0)
			{
				rtn = pdev->channel[rtn].handle;
			}
			mutex_unlock(&pdev->glock);
			return rtn;
		}
		break;

	case LYIPC_CMD_WRITE_MESSAGE:		///< 写异步消息
		{
			struct chan_ctrl_rw ctrl;

			if(copy_from_user(&ctrl, argp, sizeof(ctrl)))
			{
				return -EFAULT;
			}
			if(ctrl.handle <= 0)
			{
				return -EFAULT;
			}
			PLOG("write handle:handle:%08x, len:%d\r\n", ctrl.handle, ctrl.len);
			return write_channel(pdev, &ctrl);
		}
		break;

	case LYIPC_CMD_READ_MESSAGE:		///< 读消息
		{
			struct chan_ctrl_rw ctrl;

			if(copy_from_user(&ctrl, argp, sizeof(ctrl)))
			{
				return -EFAULT;
			}
			if(ctrl.handle <= 0 || NULL == ctrl.buf || 0 == ctrl.len)
			{
				return -EFAULT;
			}

			rtn =  read_channel(pdev, &ctrl, 0);
			if(0 == rtn)
			{
				if(copy_to_user(argp, &ctrl, sizeof(ctrl)))
				{
					return -EFAULT;
				}
			}
			return rtn;
		}
		break;
	case LYIPC_CMD_QRY_QUEINFO:
		{
			struct chan_ctrl_queinfo ctrl;

			if(copy_from_user(&ctrl, argp, sizeof(ctrl)))
			{
				return -EFAULT;
			}
			if(ctrl.handle <= 0)
			{
				return -EFAULT;
			}

			rtn =  query_channel(pdev, &ctrl);
			if(0 == rtn)
			{
				if(copy_to_user(argp, &ctrl, sizeof(ctrl)))
				{
					return -EFAULT;
				}
			}
			return rtn;
		}
		break;
	case LYIPC_CMD_READ_PEND:		///< 读等待
		{
			struct chan_ctrl_rw ctrl;
	
			if(copy_from_user(&ctrl, argp, sizeof(ctrl)))
			{
				return -EFAULT;
			}
		
			if(ctrl.handle <= 0 || NULL == ctrl.buf || 0 == ctrl.len)
			{
				PLOG("ctrl.handle:%d, ctrl.buf:%08x, ctrl.len:%d\r\n", ctrl.handle, \
					   (unsigned int)ctrl.buf, ctrl.len);
				return -EFAULT;
			}
			
			rtn =  read_channel(pdev, &ctrl, 1);
			
			if(0 == rtn)
			{
				if(copy_to_user(argp, &ctrl, sizeof(ctrl)))
				{
					return -EFAULT;
				}
			}
			return rtn;
		}
		break;

	default:
		break;
	}

	return -EFAULT;
}

static int lyipc_open(struct inode *inode, struct file *file)
{
	return nonseekable_open(inode, file);
}

static int lyipc_close(struct inode *inode, struct file *file)
{
	return 0;
}

// file operations struct
static struct file_operations lyipc_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = lyipc_ioctl,
	.open = lyipc_open,
	.release = lyipc_close,
};

static int __init drv_lyipc_init(void)
{
    int ret, result;

    printk(KERN_INFO"LY IPC DRIVER:%d.%d.%d-%c\n", IPCDRV_MAJOR, IPCDRV_MINOR, IPCDRV_SMALL, IPCDRV_STATE);

 	lyipc_dev = kmalloc(sizeof(lyipc_dev_t), GFP_KERNEL);
    if(!lyipc_dev)
    {
        printk(KERN_ALERT"init ipc driver failed.\n");
        return -ENOMEM;
    }
	memset(lyipc_dev, 0, sizeof(lyipc_dev_t));
	mutex_init(&lyipc_dev->glock);
	lyipc_dev->serial_cnt = LYIPC_INITSERCNT;

	lyipc_dev->devt = MKDEV(lyipc_major, 0);
	lyipc_dev->class = class_create(THIS_MODULE, LYIPC_DRVNAME);
	if (IS_ERR(lyipc_dev->class))
	{
		printk(KERN_ERR "ipc: faile to create device class\n");
		goto CREATECLASS_FAIL;
	}
	
	if (lyipc_major)
	{
		result = register_chrdev_region(lyipc_dev->devt, 1, LYIPC_DRVNAME);
	}
	else 
    {
		result = alloc_chrdev_region(&lyipc_dev->devt, 0, 1, LYIPC_DRVNAME);
		lyipc_major = MAJOR(lyipc_dev->devt);
	} 
	if (result < 0) 
	{
		kfree(lyipc_dev);
		lyipc_dev = NULL;
		return result;
	}

	cdev_init(&lyipc_dev->cdev, &lyipc_fops); 
	lyipc_dev->cdev.owner = THIS_MODULE; 		
	lyipc_dev->cdev.ops = &lyipc_fops;		
	
	ret = cdev_add(&lyipc_dev->cdev, MKDEV(lyipc_major, 0), LYIPC_NR_DEVS);
	if(ret)
    {
		printk(KERN_ALERT "ipc failed to add dev.\n");
		goto ADD_DEV_FAIL;
	} 
	//creates a device and registers it with sysfs
	lyipc_dev->dev = device_create(lyipc_dev->class, NULL,MKDEV(MAJOR(lyipc_dev->devt),0),NULL,LYIPC_DRVNAME); 

	printk("driver lyipc inited, heap size = %d\n", (unsigned int)HEAP_SIZE);

    return 0;

ADD_DEV_FAIL:
    unregister_chrdev_region(MKDEV(lyipc_major, 0),1);
    class_destroy(lyipc_dev->class);
CREATECLASS_FAIL:
    kfree(lyipc_dev);
	lyipc_dev = NULL;
    return -ENOMEM;   
}


static void drv_lyipc_exit(void)
{
	int i;

    cdev_del(&lyipc_dev->cdev);		
    device_destroy(lyipc_dev->class,lyipc_dev->devt);
    unregister_chrdev_region(MKDEV(lyipc_major, 0),1);
    class_destroy(lyipc_dev->class);
	for(i=0; i<LYIPC_MAX_QUE; i++)
	{
		if(NULL != lyipc_dev->channel[i].pque)
		{
			destroy_channel(lyipc_dev, lyipc_dev->channel[i].handle);
		}
	}
    kfree(lyipc_dev);
}

module_init(drv_lyipc_init);
module_exit(drv_lyipc_exit);

/*
MODULE_DESCRIPTION("Lyipc Driver");
MODULE_AUTHOR("Linyang");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.1");
*/
MODULE_AUTHOR("Soft-Reuuimlla");
MODULE_DESCRIPTION("User mode CEDAR device interface");
MODULE_LICENSE("GPL");
