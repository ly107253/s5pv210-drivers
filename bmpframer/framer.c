#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>


#define  FILE  "/dev/fb0"      // 设备文件
 

 /*计算机颜色16进制表示符*/
#define  WHITE    0xFFFFFF     //白色
#define  RED      0xFF0000     //红色
#define  GREEN    0x00FF00     //绿色
#define  BLUE     0x0000FF     //蓝色
#define  YELLOW   0xFFFF00     //黄色
#define  BLACK    0x000000     //黑色	
#define  AQNA     0xAFDFE4     //水色
#define  NAVE     0x23238E     //海军蓝
#define  ORANGE   0xFF7F00     //橙色
#define  PURPLE   0x871F78     //紫色
#define  Qioke    0x6B4226     //巧克力色

struct fb_fix_screeninfo finfo = {0};       // 不可变信息结构体
struct fb_var_screeninfo vinfo = {0};       // 可变信息结构体
 static volatile unsigned int *pMap = NULL;  // 用来指向mmap映射得到的虚拟地址
 
  
  static inline void lcd_draw_pixel(unsigned int x, unsigned int y, unsigned int color);
 static void lcd_draw_background(unsigned int color);
 void lcd_draw_lline(const unsigned int x, const unsigned int y, const unsigned int length,   // 注意这个函数参数太多了，不应该这样设计，我们应该把这些参数放在一个结构体中，把结构体变量的指针传进来即可，这样效率高
                      const unsigned int width, const unsigned int color);
                      
 static void lcd_draw_image(const unsigned char *pData);      // pData是一个图片的数据数组： 这里是 1024*600 像素24位真彩色格式的图片，，数组的元素个数： 1024*600*3   我们需要将连续的3个1个字节长度的数据合成一个24位数据
 
  
 int main(void)
 {
      int fd = 0;
      
    /* 打开文件得到文件描述符 */
      fd = open(FILE, O_RDWR);
     if (0 > fd) {
         perror("open error");
         return -1;
      }
     printf("%s 打开成功\n", FILE);
      
      /* 操作文件 */
      if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo)) {
         perror("ioctl error");
         close(fd);
          return -1;
     }    
 
     if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo)) {
         perror("ioctl error");
          close(fd);
          return -1;
      }
         
       // 打印信息
      printf("不可变信息smem_start = 0x%x\n", finfo.smem_start);    
     printf("不可变信息smem_len = %ld\n", finfo.smem_len);    
      printf("可变信息xres = %d, yres = %d\n", vinfo.xres, vinfo.yres);
     printf("可变信息xres_virtual = %d, yres_virtual = %d\n", vinfo.xres_virtual, vinfo.yres_virtual);
     printf("可变信息xoffset = %d, yoffset = %d\n", vinfo.xoffset, vinfo.yoffset);
      
     /* 进行mmap映射 */
      pMap = mmap(NULL, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
      if (NULL == pMap) {
          perror("mmap error");
         return -1;
     }
     
     /* 背景填充 */
     lcd_draw_background(YELLOW);
     
      /* 关闭文件 */
      close(fd);    
     
    return 0;
}
 
 /*填充像素点*/
 static inline void lcd_draw_pixel(unsigned int x, unsigned int y, unsigned int color)
 {
    *(unsigned int *)((unsigned int)pMap + (vinfo.xres*x + y)*4) = color;
 }

 /*填充LCD背景*/
 static void lcd_draw_background(unsigned int color)
 {
      unsigned int i = 0;
      unsigned int j = 0;
      
      for (i = 1; i <= vinfo.yres; ++i)
     {
          for (j = 0; j <= vinfo.xres; ++j)
             lcd_draw_pixel(i, j, color);
      }
 }
 
 //  画线函数
 void lcd_draw_lline(const unsigned int x, const unsigned int y, const unsigned int length,
                      const unsigned int width, const unsigned int color)              
 {
     volatile unsigned int i = 0;
    volatile unsigned int j = 0;
     
     for (i = x; i < width+x; i++)
    {
         for (j = y; j < length+y; j++)
         {
            lcd_draw_pixel(i, j, color);
         }
    }    
 } 

//  24位真彩色图片显示函数
 static void lcd_draw_image(const unsigned char *pData)
{
     unsigned int i = 0;
     unsigned int j = 0;
    unsigned int color = 0;
     unsigned int p = 0;
    
     for (i = 0; i < vinfo.yres; i++)
    {
        for (j = 0; j < vinfo.xres; j++)
        {
            color = (pData[p+0] << 16) | (pData[p+1] << 8) | (pData[p+2] << 0);
           lcd_draw_pixel(i, j, color);
          p = p+3;
        }
    }
 }