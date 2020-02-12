//包含头文件
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

//宏定义
#define Address 0x50                  //at24c02地址

#define I2C_RETRIES   0x0701
#define I2C_TIMEOUT   0x0702
#define I2C_SLAVE     0x0703       //IIC从器件的地址设置
#define I2C_BUS_MODE   0x0780

typedef unsigned char uint8;

uint8 rbuf[8] = {0x00}; //读出缓存
uint8 wbuf[8] = {0x01,0x05,0x06,0x04,0x01,0x01,0x03,0x0d}; //写入缓存
int fd = -1;

//函数声明
static uint8 AT24C02_Init(void);
static uint8 i2c_write(int fd, uint8 reg, uint8 val);
static uint8 i2c_read(int fd, uint8 reg, uint8 *val);
static uint8 printarray(uint8 Array[], uint8 Num);


//at24c02初始化
static uint8 AT24C02_Init(void)
{
  fd = open("/dev/i2c-0", O_RDWR);   //允许读写
  
  if(fd < 0)
    {
        perror("Can't open /dev/nrf24l01 \n"); //打开iic设备文件失败
        exit(1);
    }
    
    printf("open /dev/i2c/0 success !\n");   //打开iic设备文件成功
    
  if(ioctl(fd, I2C_SLAVE, Address)<0) {    //设置iic从器件地址
                printf("fail to set i2c device slave address!\n");
                close(fd);
                return -1;
        }
        
  printf("set slave address to 0x%x success!\n", Address);
        
//    if(ioctl(fd, I2C_BUS_MODE, 1)<0)     //设置iic总线模式
  //          printf("set bus mode fail!\n");
   // else
     //       printf("set bus mode ok!\n");
    
    return(1);
}

/*
uint8 AT24C02_Write(uint8 *nData, uint8 Reg, uint8 Num)
{
  write(fd, &Reg, 1);  //
  usleep(100);              //延时100us
  write(fd, nData, Num);
  usleep(1000*4);             //延时 4ms
  
  return(1);
}

uint8 AT24C02_Read(uint8 nData[], uint8 Reg, uint8 Num)
{
  write(fd, &Reg, 1); 
  usleep(100);              //延时100us
  read(fd, nData, Num);
  usleep(1000*4);             //延时 4ms
  
  return(1);
}
*/

//at24c02写入一字节
static uint8 i2c_write(int fd, uint8 reg, uint8 val)
{
        int retries;
        uint8 data[2];

        data[0] = reg;
        data[1] = val;
        for(retries=5; retries; retries--) {
                if(write(fd, data, 2)==2)
                        return 0;
                usleep(1000*10);
        }
        return -1;
}

//at24c02读取一字节
static uint8 i2c_read(int fd, uint8 reg, uint8 *val)
{
        int retries;

        for(retries=5; retries; retries--)
                if(write(fd, &reg, 1)==1)
                        if(read(fd, val, 1)==1)
                                return 0;
        return -1;
}

//输出数组
static uint8 printarray(uint8 Array[], uint8 Num)
{
  uint8 i;
  
  for(i=0;i<Num;i++)
  {
    printf("Data [%d] is %d \n", i ,Array[i]);
  }
  
  return(1);
}

//主函数
int main(int argc, char *argv[])
{
  int i;
 
  AT24C02_Init();
  usleep(1000*100);
    for(i=0; i<sizeof(rbuf); i++)
            if(i2c_read(fd, i, &rbuf[i]))
                    break;
  printarray(rbuf ,8);
  printf("Before Write Data \n");
  sleep(1);
    for(i=0; i<sizeof(rbuf); i++)
            if(i2c_write(fd, i, wbuf[i]))
                    break;
  printarray(wbuf ,8);
  printf("Writing Data \n");
  sleep(1);
    for(i=0; i<sizeof(rbuf); i++)
            if(i2c_read(fd, i, &rbuf[i]))
                    break;
  printarray(rbuf ,8);
  printf("After Write Data \n");
  
  close(fd);
}
