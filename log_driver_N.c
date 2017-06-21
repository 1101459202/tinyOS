{//***********************************驱动开发************************************

{//？什么是驱动
	驱动是直接和硬件打交道的软件程序
	
	--------
	| 应用 |
	--------
	    |
	---------   
	| 驱动  |
	---------
	    |
	---------    
	| 硬件  |
	---------
	 
}

{/*如何去学习 linux驱动开发
  linux 驱动涉及的软硬件知识多，架构复杂。
  不要纠结于细节 （如为弄明白某段代码，不断深入下去）
  最好从使用的角度， 宏观上，框架上进行把握。
  多做实验，通过实验的现象来进行理解会比较快。
}

{//？学完后要获得什么样能力   
 1. 熟悉linux驱动架构和开发流程
 2. 掌握驱动代码跟踪分析
 3. 能独立开发简单的驱动
}

{//===========================linux内核模块的开发========================
{//？什么是内核模块	  
  可在运行时添加到内核中的代码被称为“模块”。
	{//？内核模块和应用程序有何不同
     	   应用            --             模块
    从头到尾单任务 	            预先注册进内核，被动的被调用的              
	     应用空间                    内核空间，权限大
	   段错误对系统危害小          段错误常会导致系统崩溃
	}
}

{//？为何要用内核模块	
  1、减小内核体积，因为模块本身不被编译到内核镜像里面。
  2、可以在内核中添加或删除功能而不用重新编译内核
}

{//？重点在那	
	掌握如何去创建和使用一个内核模块
}

{//？一个最简单的内核模块如何实现和使用的	 重点 三要素
	//---/led/step1/fs4412_led_drv.c
	#include <linux/kernel.h>
	#include <linux/module.h>
	
	MODULE_LICENSE("Dual BSD/GPL");  //模块许可声明
	
	int led_init(void)
	{
		printk("Led init\n");
		return 0;
	}
	
	void led_exit(void)
	{
		printk("Led exit\n");
	}
	
	module_init(led_init);  //模块加载入口声明
	module_exit(led_exit);	//模块卸载入口声明
  
  //---/led/step1/Makefile  
	KERNELDIR ?= /home/linux/store/linux-3.14-fs4412   //对应内核源码目录(根据自己的内核目录进行修改），

	//---使用
	$make                //编译生成模块文件fs4412_led_drv.ko （编译之前要保证内核有编译成功）
	$file fs4412_led_drv.ko       //查看一下格式，看编译的是 电脑的(Intel 80386)， 还是板子的（ARM)
  #insmod fs4412_led_drv.ko     /*加载模块                        
                       可能保错 insmod: can't insert 'fs4412_led_drv.ko': invalid module format 
                       原因就是格式不对（file fs4412_led_drv.ko可查看）如编译的是电脑的hello.ko 却放到板子上运行。
                       也可能是和内核不匹配 
                       */
  #lsmod | grep fs4412_led_drv  /*检测是否已加载
  			lsmod 实际是读取/proc/modules 显示如下        
           fs4412_led_drv 2046 0 - Live 0xbf004000 (O)
       */
  #rmmod fs4412_led_drv         /*卸载模块     
                        在板子上可能报错 rmmod: chdir(/lib/modules): No such file or directory 
                                         rmmod: chdir(3.14.0): No such file or directory 
                        创建 /lib/modules/3.14.0 目录 即可
                        
                       */
  #dmesg               //查看打印的信息
  
}	

}

{//==============================字符设备驱动============================ 重中之重
{/*什么是字符设备
	 从设备数据传递的特点看， 如果是按字节流顺序传递过来的，就像生产流水线，如串口，键盘，鼠标等。
 我们把它们抽象为字符设备。
}

{//？重点在那
	掌握如何去创建和使用一个简单的字符设备
}

{//？如何实现和使用一个字符设备 
	{1. 注册设备号 */
	 	{//---led/step2/fs4412_led_drv.c
			#include <linux/kernel.h>
			#include <linux/module.h>
			#include <linux/fs.h>	  //for MKDEV register_chrdev_region
			
			MODULE_LICENSE("Dual BSD/GPL");
			
			#define LED_MA 500  //主设备号 用于区分不同种类的设备  
			                    //某些主设备号已经静态地分配给了大部分公用设备。见Documentation/devices.txt 。 这里我们常使用250
			#define LED_MI 0   //次设备号 用于区分同一类型的多个设备
			#define LED_NUM 1  //有多少个设备
			
			
			static int s5pv210_led_init(void)
			{
				dev_t devno = MKDEV(LED_MA, LED_MI); 
				int ret;
			
				ret = register_chrdev_region(devno, LED_NUM, "newled");   /*注册字符设备号(静态分配)
					                                            为一个字符驱动获取一个或多个设备编号
					                                            dev_id:       分配的起始设备编号(常常是0）
					                                            DEVICE_NUM：  请求的连续设备编号的总数(不能太大，避免别的主设备号冲突)
					                                            DEVICE_NAME： 是应当连接到这个编号范围的设备的名字 
					                                            alloc_chrdev_region  可进行动态分配                                           
			                                                    */
				if (ret < 0) {
					printk("register_chrdev_region\n");
					return ret;
				}
			
				printk("Led init\n");
			
				return 0;
			}
			
			static void s5pv210_led_exit(void)
			{
				dev_t devno = MKDEV(LED_MA, LED_MI);
				unregister_chrdev_region(devno, LED_NUM);  //取消注册
				printk("Led exit\n");
			}
			
			module_init(s5pv210_led_init);
			module_exit(s5pv210_led_exit);
		}		 		
		//---使用
		insmod fs4412_led_drv.ko
		cat /proc/devices //有newled 表示该注册成功
	}
 /*  
	{2. 初始化字符设备
		//---led/step3/fs4412_led_drv.c
			#include <linux/kernel.h>
			#include <linux/module.h>
			#include <linux/fs.h>
			#include <linux/cdev.h>  //字符设备头文件
			
			MODULE_LICENSE("Dual BSD/GPL");
			
			#define LED_MA 500
			#define LED_MI 0
			#define LED_NUM 1
			
			struct cdev cdev;
			
			static int s5pv210_led_open(struct inode *inode, struct file *file)
			{
				return 0;
			}
				
			static int s5pv210_led_release(struct inode *inode, struct file *file)
			{
				return 0;
			}
				
			struct file_operations s5pv210_led_fops = { //文件操作
				.owner = THIS_MODULE,
				.open = s5pv210_led_open,
				.release = s5pv210_led_release,
			};
			
			static int s5pv210_led_init(void)
			{
				dev_t devno = MKDEV(LED_MA, LED_MI); 
				int ret;
			
				ret = register_chrdev_region(devno, LED_NUM, "newled");
				if (ret < 0) {
					printk("register_chrdev_region\n");
					return ret;
				}
			
				cdev_init(&cdev, &s5pv210_led_fops);//字符设备初始化
				ret = cdev_add(&cdev, devno, LED_NUM); //添加字符设备到系统中
				if (ret < 0) {
					printk("cdev_add\n");
					goto err1;
				}
			
				printk("Led init\n");
			
				return 0;
				//---goto 出错处理， 顺序申请，逆序释放，避免资源回收不完全（如内存泄露）
			err1:
				unregister_chrdev_region(devno, LED_NUM);
				return ret;
			}
			
			static void s5pv210_led_exit(void)
			{
				dev_t devno = MKDEV(LED_MA, LED_MI);
			
				cdev_del(&cdev);
				unregister_chrdev_region(devno, LED_NUM);
				printk("Led exit\n");
			}
			
			module_init(s5pv210_led_init);
			module_exit(s5pv210_led_exit);		
			
		  //---led/step3/fs4412_led_app.c  
			#include <stdio.h>
			#include <fcntl.h>
			#include <unistd.h>
			#include <stdlib.h>
			#include <sys/ioctl.h>
			
			
			int main(int argc, char **argv)
			{
				int fd;
			
				fd = open("/dev/led", O_RDWR); /* 应用open是如何调用到驱动的open的
	                                                 open      ->      sys_open     ->         inode           ->  driver .open 
	                                                 应用       |      系统调用     |         文件系统         |   驱动      
	                                        */
				if (fd < 0) {
					perror("open");
					exit(1);
				}
				printf("open led ok\n");  //注意要加\n 否则打印信息可能没有
			
				return 0;
			}		  			
				
		 //---使用
		  #mknod /dev/led c 500 0  //创建设备文件，应用才能访问它. ( ls -l /dev 可以看到很多其它设备文件) 
      #./a.out  //运行会看到  open led ok                       
	 			
	}
/*	 
	{3. 实现需要的文件操作 //重点 难点
		{//---led/step4/fs4412_led_drv.cc
			#include <linux/kernel.h>
			#include <linux/module.h>
			#include <linux/fs.h>
			#include <linux/cdev.h>
			
			#include <asm/io.h>
			#include <asm/uaccess.h> // for copy_to_user
			
			MODULE_LICENSE("Dual BSD/GPL");
			
			#define LED_MAGIC 'L'   //幻数：0~0xff的数。用于区分不同的驱动, 见Documentation/ioctl/ioctl-number.txt
			
			#define LED_ON	_IOW(LED_MAGIC, 0, int)   //加幻数方式来定义命令，防止不同驱动间命令错乱
			#define LED_OFF	_IOW(LED_MAGIC, 1, int)
					
			#define LED_MA 500
			#define LED_MI 0
			#define LED_NUM 1
			
			#define FS4412_GPX2CON	0x11000C40
			#define FS4412_GPX2DAT	0x11000C44
			
			static unsigned int *gpx2con;
			static unsigned int *gpx2dat;
			
			struct cdev cdev;
			
			static int s5pv210_led_open(struct inode *inode, struct file *file)
			{
				return 0;
			}
				
			static int s5pv210_led_release(struct inode *inode, struct file *file)
			{
				return 0;
			}
				
				
			//ioctl 用于完成特殊操作				
			static long s5pv210_led_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
			{
				int nr;
			
				switch (cmd) {
					case LED_ON:
						printk("driver led on\n");
						writel(readl(gpx2dat) | 1 << 7, gpx2dat);
						break;
					case LED_OFF:
					  printk("driver led off\n");
						writel(readl(gpx2dat) & ~(1 << 7), gpx2dat);
						break;
				}
			
				return 0;
			}
				
		  // file_operations 中 定义了针对文件的一系列操作方法   不是每个都需实现	
			struct file_operations s5pv210_led_fops = {
				.owner = THIS_MODULE,
				.open = s5pv210_led_open,
				.release = s5pv210_led_release,
				.unlocked_ioctl = s5pv210_led_unlocked_ioctl,
			};
			
			static int s5pv210_led_init(void)
			{
				dev_t devno = MKDEV(LED_MA, LED_MI); 
				int ret;
			
				ret = register_chrdev_region(devno, LED_NUM, "newled");
				if (ret < 0) {
					printk("register_chrdev_region\n");
					return ret;
				}

				cdev_init(&cdev, &s5pv210_led_fops);
				cdev.owner = THIS_MODULE;
				ret = cdev_add(&cdev, devno, LED_NUM);
				if (ret < 0) {
					printk("cdev_add\n");
					goto err1;
				}
			*/
				gpx2con = ioremap(FS4412_GPX2CON, 4); /* 动态映射 物理地址 到内核虚拟地址
																									phys_addr  起始物理地址
																									size       映射范围大小，
																									返回值     映射后的内核虚拟地址 
																							 */	
				if (gpx2con == NULL) {
					printk("ioremap gpx2con\n");
					ret = -ENOMEM;
					goto err2;
				}
			
				gpx2dat = ioremap(FS4412_GPX2DAT, 4);
				if (gpx2dat == NULL) {
					printk("ioremap gpx2dat\n");
					ret = -ENOMEM;
					goto err3;
				}
			
				writel((readl(gpx2con) & ~(0xf << 28)) | (0x1 << 28), gpx2con);
				writel(readl(gpx2dat) & ~(0x1<<7), gpx2dat);
			
				printk("Led init\n");
			
				return 0;
			err3:
				iounmap(gpx2con);
			err2:
				cdev_del(&cdev);
			err1:
				unregister_chrdev_region(devno, LED_NUM);
				return ret;
			}
			
			static void s5pv210_led_exit(void)
			{
				dev_t devno = MKDEV(LED_MA, LED_MI);
			
				iounmap(gpx2dat);
				iounmap(gpx2con);
				cdev_del(&cdev);
				unregister_chrdev_region(devno, LED_NUM);
				printk("Led exit\n");
			}
			
			module_init(s5pv210_led_init);
			module_exit(s5pv210_led_exit);
		}	
			
		{//---led/step4/fs4412_led_app.c
				#include <stdio.h>
				#include <fcntl.h>
				#include <unistd.h>
				#include <stdlib.h>
				#include <sys/ioctl.h>
				
				#define LED_MAGIC 'L'
				
				#define LED_ON	_IOW(LED_MAGIC, 0, int)
				#define LED_OFF	_IOW(LED_MAGIC, 1, int)
				
				int main(int argc, char **argv)
				{
					int fd;
				
					fd = open("/dev/led", O_RDWR);
					if (fd < 0) {
						perror("open");
						exit(1);
					}
				
					while(1)
					{
						ioctl(fd, LED_ON);
						usleep(100000);
						ioctl(fd, LED_OFF);
						usleep(100000);
					}
				
					return 0;
				}
		}	  
  }
 
}

}
/*
{//=======================linux设备驱动中的并发控制=======================
{//？什么是并发，为何要使用并发机制
	并发即是 多件事情同时在执行
	
  只有使这些设备都并发的执行才能满足性能要求。  
  
  例：
      如果系统上挂10个设备，每个设备都请求，如果是串行顺序执行，
      可能一个设备要等较长时间，系统才能响应它    
}
	
{//？如何实现并发
	CPU是顺序的读入指令执行的， 如何来实现并发呢？	
	例：
	   当你A先生的业务时，B女士来个电话，你接完电话后继续做A的事。
	   如何你切换足够快，即在A,B的忍耐时间内。 感觉就像在并发执行一样。
	   
	使用中断的方式可实现并发    //中断： 类似于接电话，打断当前执行，等处理完后，返回再接着执行。
	     
}

{//？为什么要进行并发控制
	并发会导致竞态的发生。 //竞态： 一个资源被并发执行的任务同时访问时，就会出现竞争。此时的状态就是竞态。
	需保证一个资源在一个时间只能被一个任务访问，才能避免混乱。即并发控制	
}	

{//？如何进行并发控制
 采用互斥机制对并发进行控制  //互斥： 共享资源被某任务访问时，别的任务禁止访问。
	                                                                                             
 互斥机制：
	 {//中断屏蔽     
	 	 使用禁止中断方式，避免中断的影响 
	*/ 	 
	 	 local_irq_disable       //屏蔽中断
	// 	 临界区代码              //临界区： 访问共享资源的代码区域
	 	 local_irq_enable        //开中断
/*	 	 
	 	 注： 
	 	    1。屏蔽中断的时间尽量短，否则会造成别的中断延时过久，甚至丢失,最好能采有屏蔽指定中断的方式
	 	 		  disable_irq(int irq); //屏蔽指定中断号的中断
			    enable_irq(int irq);  //使能某中断号的中断   
			  2。常需保存当前的中断状态，便于恢复   
			    用local_irq_save(flags);  代替local_irq_disable
			    用local_irq_restore(flags) 代替 local_irq_enable			  
   }	      
	 {//原子操作
	 	 并发中不被打断的最小单元
	 	
	  {//---hello_atomic/hello.c         实现设备只被一个进程打开
	  	#include <asm/atomic.h>
	  	
		  static atomic_t hello_atomic = ATOMIC_INIT(1);    //定义原子变量hello_atomic ,并初始化为1 
	    static int hello_open(struct inode *inode,struct file *file)
	    {
	      if (!atomic_dec_and_test(&hello_atomic))  {  //  atomic_dec_and_test表示原子变量自减一，并测试是否为了零,如果为零返回真
	      	 //当已open过，还未release时，再次open , hello_atomic为0, 再减一不为零，!atomic_dec_and_test 成立  
	         atomic_inc(&hello_atomic);       //原子变量加一，恢复自减前状态
	         return  - EBUSY;   //已经被打开
	      }
	      //当第一次被open时， hello_atomic为1  , !atomic_dec_and_test 不成立， 正常打开
	      printk("hello open \n");
	      return 0;
	    } 
	    static int hello_release(struct inode *inode,struct file *file)
	    { 
	      atomic_inc(&hello_atomic);   //释放设备    
	      printk("hello release\n");
	      return 0;
	    }
    }	
	 	
   }   
	 {//自旋锁	spinlock  (cpu自旋 类似while循环）
	 	 cpu自旋死循环空转CPU 等待释放锁,  耗资源， 适用于锁持有时间小于睡眠唤醒时间场合	
	 	 进程递归获得锁时，会导致死锁
	 	 	
	 	 {//---hello_spinlock/hello.c
 			static DEFINE_SPINLOCK(hello_spinlock); 
 			static int hello_resource = 1;
 			
 			static int hello_open(struct inode *inode,struct file *file)
 			{
 			  spin_lock(&hello_spinlock);  //还可和中断屏蔽合用 spin_lock_irq    
 			  if(hello_resource == 0)        //为避免死机， spin_try_lock
 			  {
 			     spin_unlock(&hello_spinlock); 
 			     return  - EBUSY; 
 			  }
 			  hello_resource--;
 			  spin_unlock(&hello_spinlock); 
 			
 			
 			  printk("hello open \n");
 			  return 0;
 			} 
 			static int hello_release(struct inode *inode,struct file *file)
 			{
 			  spin_lock(&hello_spinlock);
 			  hello_resource++; 
 			  spin_unlock(&hello_spinlock);
 			
 			  printk("hello release\n");
 			  return 0;
 			}
     }
     
   }   
	 {//信号量   
 	  进程当中用于并发互斥和，资源的计数。
 	  相对于自旋锁，信号量会睡眠，仅能用于进程中 
    //---hello_semaphore/hello.c
  	#include <linux/semaphore.h>
    static DEFINE_SEMAPHORE(hello_semlock);   //定义一个初始值为一的信号量
       
    static int hello_open(struct inode *inode,struct file *file)
    {            
      if (down(&hello_semlock))  /* 获得打开锁*/
      {
         return  - EBUSY;  /* 设备忙*/
      }
      
      printk("hello open \n");
      return 0;
    }
    static int hello_release(struct inode *inode,struct file *file)
    {
      up(&hello_semlock);  /* 释放打开锁*/
      printk("hello release\n");
      return 0;
    }	  
   }    
 	 {//互斥体  用得最多
  //	 互斥体是专门用来做互斥的， 和二元的信号量类似， 
  	
		struct mutex my_mutex;  /* 定义mutex */
		
		static struct mutex lock;
		mutex_init(&my_mutex);  /* 初始化mutex */
		
		mutex_lock(&my_mutex);  /* 获取mutex */
		.../* 临界资源*/
		mutex_unlock(&my_mutex); /* 释放mutex */
   }	
   
  
}

{/*并发机制使用场合
	三个并发：    进程    中断    CPU
	
	1. 中断屏蔽的使用场合  //用于 中断和中断间。进程 和 中断 竞争时
 	   当有中断处理程序访问共享资源的时候。
	2. 原子操作的使用场合  //用于 进程间 中断间 综合竞争时  （信号量 中断屏蔽等都是基于原子操作来实现的）
	   只使用于共享资源为一个变量的操作的情况
	3. 自旋锁的使用场合   //用于 多CPU 竞争时， （尽量不用，因有CPU死掉的风险）
	   多CPU的情况下 
	   在临界区代码运行时间比较短的情况。
	4. 信号量互斥体的使用场合   //用于 进程间竞争时  （它不能防止中断的打断，故要防止中断打断，虚与中断屏蔽混合使用）
	   临界区代码运行时间比较长的情况。  
	   当锁持有的时间较长的时候，优先使用信号量。
	   1注意： 中断里不能使用信号量。 因为中断不能睡眠。
}	
			
}

{//====================Linux设备驱动中的阻塞与非阻塞I/O===================
{//？驱动中阻塞和非阻塞,异步通知是什么
	
	  ----------
	  |        |                  --------
	  |  进程  |  <-------------->| 设备 |
	  |        |                  --------
	  ----------
	
   阻塞：     进程等待设备资源可用， 等待过程中， 进程休眠（CPU切换去执行别的进程）。 
   非阻塞：   不等待，轮询设备 
   异步通知： 让设备主动去通知进程。
   
   
   注： 提出阻塞，非阻塞，异步通知概念，这里是为了解决 进程和设备间是如何打交道的
}
   
{//？如何在驱动中实现阻塞I/O
  当有多个进程阻塞睡眠时，可用排队的方式，当造成阻塞的条件消失（如有资源释放了)，去唤醒阻塞在排队的进程.  
  
  {//？什么是等待队列
    以队列为基础数据结构，与进程调度机制紧密结合，能够用于实现内核中的异步事件通知机制，同步对系统资源的访问。
    
    如：
  	   当某进程不满足某条件时(如资源), 可把进程暂时睡眠，放入队列中，
  	   等条件满足时(如某资源被释放出来了)，再把进程唤醒去执行
  }
  
  {//？如何使用等待队列，在驱动中实现阻塞
		#cd  /root/driver_example/waitqueue
		#make clean
		#make 
		#insmod hello.ko   //这里不用再 mknod /dev/hello c 250 0 因设备节点用 device_create已自动创建
		#cat /dev/hello  &    // 加上& 表示该进程后台执行   
		打开另一终端
		#echo hello > /dev/hello
		可以发现在前面终端会打印出hello
		#rmmod hello
			
	  {//---driver_example/waitqueue/hello.c
      #include <linux/sched.h>
     static wait_queue_head_t queue;   //定义等待队列
     static char *gbuf=NULL;
     static int glen=0;
     static ssize_t hello_read (struct file *filp, char __user *buf, size_t count,
                     loff_t *f_pos)
     {
     	
     	  /*
     	    wait_event_interruptible  内部有一for循环，
     	    1. 当条件满足时，且无信号需处理时， 跳出循环，返回0

     	    2. 如果有信号未处理，返回非零值
     	       return -ERESTARTSYS，  
     	       系统调用看到ERESTARTSYS，则在处理完信号后，又会再次触发前次的调用（即又调用了一次hello_read）
   	    
     	    3. 当条件不满足时，且无信号需处理，则睡眠等待    	    
     	       	        
     	  */
        if (wait_event_interruptible(queue, glen!=0))   //把调用该函数的进程（cat /dev/hello）, 放入等待队列queue中，等
        {
           return -ERESTARTSYS;  //表示信号函数处理完毕后重新执行信号函数前的某个系统调用.
                                 //即等待队列在等待中，如果有信号，先执行完信号后，再执行一次系统调用，继续等待
        }
                
        glen =0;
        ...
     }
    
     static ssize_t hello_write(struct file *filp, const char __user *buf, size_t count,
                     loff_t *f_pos)
     {
     	  glen = count;
        wake_up_interruptible(&queue);  //唤醒在queue等待队列上等待的所有进程 , 这里指 cat /dev/hello 进程
        ...
     }
    
     static int hello_init(void)
     {
     	 gbuf = kmalloc(GBUF_MAX, GFP_KERNEL);
    	 init_waitqueue_head(&queue);
    	 ...
     }
    
		}
		

	}
}

{//？如何在驱动中实非阻塞I/O
	 应用：
	    用select/poll       // I/O多路复用 
	 驱动：
	    实现.poll 函数。 


  ---------------------------
  | 应用                    
  |         select/poll    
  ---------------------------
               |
  ---------------------------
  |内核     sys_poll       //poll系统调用,轮询poll_table中的驱动.poll， 当设备可读或可写则返回
  |            |            
  |          .poll             //驱动实现的.poll
  ---------------------------

   /*原理：
       select/poll 会调用  sys_poll(系统调用)  ，再调用驱动中的.poll
 
    sys_poll它会循环监听 poll_table上的设备是否就绪（由其关联的等待队列queue触发），如果就绪则返回
    如不就绪，则 schedule (让渡内核去执行别的进程)， 返回后继续监测。
    如果发生阻塞，会发生在sys_poll中，
    
   */


	{//------实验 poll------------
		#cd  /root/driver_example/poll
		#make clean
		#make 
		#insmod hello.ko
		#mknod /dev/hello c 250 0
		#arm-none-linux-gnueabi-gcc test.c
		#./a.out &  //后台运行    等待有数据输入
		打开另一终端
		#echo hello > /dev/hello //写入数据， 原终端显示Poll monitor:can be read
		#cat /dev/hello //读走内容，原终端等待有数据输入
	}	
	
	{//---poll/test.c    //功能：  查询设备，看设备是否可读写
		#include <sys/types.h>
		#include <sys/stat.h>
		#include <stdio.h>
		#include <fcntl.h>
		#include <unistd.h>
		#include <sys/time.h>
		
		int main()
		{
		  int fd, num;
		  fd_set rfds,wfds;
		
		  fd = open("/dev/hello", O_RDONLY);
		  if (fd < 0)
		  {
		     printf("Device open failure\n");
		  }
			
			
		  while (1)
		  {
				FD_ZERO(&rfds);
				FD_ZERO(&wfds);
				FD_SET(fd, &rfds);
				FD_SET(fd, &wfds);

		     			
				select(fd + 1, &rfds , &wfds, NULL, NULL);  //  I/O多路复用  最终调用驱动的.poll 函数
			
			  //数据可获得
				if (FD_ISSET(fd, &rfds))
				{
			     printf("Poll monitor:can be read\n");	  			
			     read();    
				}
			
			  //数据可写入
				if (FD_ISSET(fd, &wfds))
				{
			     printf("Poll monitor:can be write\n");
			     // write();  
				}

		  }
		}  
	}

	{//---poll/hello.c
	 #include <linux/poll.h>
		
   static unsigned int hello_poll(struct file *filp, poll_table *wait)
   {
       unsigned int mask = 0;

       poll_wait(filp, &queue, wait);// 把当前进程添加到wait（poll_table 设备监控列表）中，并关联相应的等待队列queue
                                     //当有多个驱动添加到poll_table，表示可以同时监控多个驱动设备的变化 
                                     //poll_wait函数并不阻塞， 真正的阻塞动作是在poll系统调用中完成  
       if (glen != 0)
       {
          mask |= POLLIN | POLLRDNORM; /*标示数据可获得*/
       }       
			 if (glen < GBUF_MAX)
			 {
			    mask |= POLLOUT | POLLWRNORM; /*标示数据可写入*/
			 }                           
      return mask;      //通过立即返回标志
   } 		

		static struct file_operations hello_fops = {
		  .poll = hello_poll,
		};

	}
	
}

{//？如何在驱动中实现异步通知
	#cd  /root/driver_example/async
	#make clean
	#make 
	#insmod hello.ko
	#mknod /dev/hello c 250 0
	#arm-none-linux-gnueabi-gcc test.c
	#./a.out &   //等待中  后台运行
	打开另一终端
	#echo hello > /dev/hello //写数据， 触发异步信号
	                          //原终端会显示receive a signal from globalfifo,signalnum:29
	
	{//--/async/test.c
   #include <sys/types.h>
   #include <sys/stat.h>
   #include <stdio.h>
   #include <fcntl.h>
   #include <unistd.h>
   #include <signal.h>
   
   void input_handler(int signum) //接收到异步读信号后的动作
   {
     printf("receive a signal from globalfifo,signalnum:%d\n",signum);
     
   }
   
   int main()
   {
     int fd = 0;
     int oflags;
   
     fd = open("/dev/hello",O_RDWR, S_IRUSR | S_IWUSR);
     if (fd < 0)
     {
        printf("Device open failure\n");
     }
   
     //启动信号驱动机制
     signal(SIGIO, input_handler); //让input_handler()处理SIGIO信号
     fcntl(fd, F_SETOWN, getpid());  //设置设备文件的所有者为本进程
     oflags = fcntl(fd, F_GETFL);    // 会调用 驱动中的 .fasync
     fcntl(fd, F_SETFL, oflags | FASYNC);  //FASYNC  设置支持异步通知模式 
           
      while (1)
      {
         sleep(100);
      }  
   }
	}
	{//--/async/hello.c
    struct fasync_struct *async_queue; /* 异步结构体指针，用于读 */
    
    static ssize_t hello_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos)
    {
       kill_fasync(&async_queue, SIGIO, POLL_IN);  //发送SIGIO 异步通知信号
       return count;
    }
    
    static int hello_fasync(int fd, struct file *filp, int mode)
    {
       return fasync_helper(fd, filp, mode, &async_queue);  //处理标志的变更
    }
    
    static struct file_operations hello_fops = {
      .owner = THIS_MODULE,
      .write = hello_write,
      .fasync = hello_fasync,
    };
	}
}

}

{//========================统一设备模型 之 平台设备======================= 设备树（把硬件信息从驱动中分离出来）
{//？linux设备模型 是什么
	为了管理各种复杂多样的设备， 而提出的框架模型，它是采用了面向对象的方式，进行分类分层。
	
   //---面向对象
   Kobject //用kobject构成对象的基础( 就像基类作为模型中的最原始的数据元，其它结构都集成于它)     
   
   device  //描述对象的属性
   driver  //控制对象的行为
   bus     //把device driver连接起来的通讯渠道。   
}

{//？为什么要用 linux设备模型
	使用设备模型是为了更好的管理设备。 
	而设备种类繁多，怎么去管理呢？
	我们知道处理复杂问题最难也是最有效方式，就是把它们间的关系脉络理清楚，进行分类分层。
	linux内核已经帮我把这最难的一步做了， 这就是设备模型 
}

{//？如何实现 linux统一设备模型
  //---设备间层次关系直观反映出来 sysfs ( sysfs文件系统存在于内存中，是一个虚拟文件系统，其提供了kobject对象层次的视图)
   /sys
     |--block   //包含所有的块设备，进入到block目录下，会发现下面全是link文件，link到sys/device/目录下的一些设备
     |--device  //包含系统所有的设备，并根据设备挂接的总线类型组织成层次结构
     |--bus     //包含系统中所有的总线类型
     |--class   //系统中的设备类型
     |--module  
     |--dev
         |--block
         |--char  	
	
	
  {//---设备继承框图
  	
									           kobject
									              |
									              |
									              |-------------------------------------------------------------------------
									              |                                  |                                     |
									    struct device                          struct cdev                       struct block_device        
									        struct kobject kobj;                 struct kobject kobj;                      |
									        struct bus_type * bus;               struct file_operations *ops;              |      
									        struct device_driver *driver;        dev_t dev;                                |                                                                                  |
									   	          |                                  |                                     | 
									   	          |                       ------------------------                         |         
									   	          |                       |    |      |          |                     ----------  
									   	          |                     led   key  touchscreen   tty                   |        |  
									   	          |                                              |                 RamDisk      IDE 
									   	          |                                             uart
									   	          |  
									   	          |
									   	          |
							 --------------------------------  	            
		   	       |                              |
		   	       |                              |  
		struct platform_device                   I2C                                 
		    const char  * name;                                        
		    u32     id;                                     
		    struct device   dev;                 
		    u32     num_resources;                      
		    struct resource * resource;                   

    
		struct my_device{
			 platform_device   *mev;
			 ....
		}	    
      
  }	
	
	
	
	//设备模型中的基类
	struct kobject 
		
	//设备描述
	struct device {                      
	  struct kobject kobj;         																																				
	  struct bus_type * bus;       																																				     
	  struct device_driver *driver;	
	  void    *driver_data;  																																			
	};


	//驱动描述
	struct device_driver {
	  const char              *name;
	  struct bus_type         *bus;
	  int (*probe) (struct device *dev);
	  int (*remove) (struct device *dev); 
	};
	
	//总线描述
	struct bus_type {
	     const char         * name;/*总线类型名称*/
	     int     (*match)(struct device * dev, struct device_driver * drv); //函数会自动通过名字去匹配设备device 和驱动driver
	     int     (*uevent)(struct device *dev, char **envp,
	     int     (*probe)(struct device * dev);
	     int     (*remove)(struct device * dev);
	};
		                    
}

{//平台设备 platform_device
      soc系统中集成了许多设备，它并不实际挂接到某类总线上，如I2C,USB等。 于是linux虚拟出platform总线，
   通过名字来匹配设备和驱动。 集成在soc系统中的设备都叫platform设备，为便于统一管理，它抽象出了
   资源的概念，使用时仅需get_resource.
   
   优点：
     1隔离了硬件信息和驱动， 做到板相关的代码和驱动分离。               ----》     修改 扩展 
        如果硬件有改动或移植， 仅需修改一下platform中关于资源的描述就可以了。
     2符合 总线 驱动 设备 的统一设备模型, 方便管理   
	
	struct resource {
		resource_size_t start;//资源开始位置
		resource_size_t end;  //资源结束位置
		const char *name;     
		unsigned long flags;  //资源的类型  IORESOURCE_MEM (内存地址)     IORESOURCE_IRQ (中断）
	};	
	
	struct platform_device {
        const char      * name; //平台设备的名称
        u32             id;
        struct device   dev; //内嵌的device  实现继承
        u32             num_resources;
        struct resource * resource;  //平台设备的资源
  };
	struct platform_driver {
	        int (*probe)(struct platform_device *);
	        int (*remove)(struct platform_device *);
	        struct device_driver driver; //内嵌的device_driver 实现继承
	};	
	
	{//----arch/arm/mach-smdkc100.c         老的platform_device方式 -> 板级配置文件
		#if  defined(CONFIG_DM9000)
		static struct resource dm9000_resources[] = {
			[0] = {
			.start       = 0x5000000,      
			.end         = 0x5000000 + 0x3,
			.flags       = IORESOURCE_MEM,
			},
			[1] = {
			.start      = 0x5000000 + 0x4,
			.end        = 0x5000000 + 0x4 +0x3,
			.flags      = IORESOURCE_MEM,
			},
			[2] = {
			    	.start = IRQ_EINT(6),
			    	.end   = IRQ_EINT(6),
			.flags       = IORESOURCE_IRQ,
			},
		};
		 			 
		static struct platform_device s5pc100_device_dm9000 = {
		.name        = "dm9000",
		.id        = -1,
		.num_resources        = ARRAY_SIZE(dm9000_resources),
		.resource        = dm9000_resources,
		};
		#endif		
			
		static struct platform_device *smdkc100_devices[] __initdata = {
			&s3c_device_i2c0,
			&samsung_device_pwm,
			&s3c_device_wdt,
			&s3c_device_rtc,
			&s5pc100_device_dm9000
		};


		static void __init smdkc100_machine_init(void)
		{
			platform_add_devices(smdkc100_devices, ARRAY_SIZE(smdkc100_devices));
		}		
		
		
		MACHINE_START(SMDKC100, "SMDKC100")
			/* Maintainer: Byungho Min <bhmin@samsung.com> */
			.atag_offset	= 0x100,
			.init_irq	= s5pc100_init_irq,
			.map_io		= smdkc100_map_io,
			.init_machine	= smdkc100_machine_init,
			.init_time	= samsung_timer_init,
			.restart	= s5pc100_restart,
		MACHINE_END
		
	}	
	
	{//----arch/arm/boot/dts/exynos4412-fs4412.dts   新的platform_device方式 -> 设备树
		srom-cs1@5000000 {   
			compatible = "simple-bus";
			#address-cells = <1>;
			#size-cells = <1>;
			reg = <0x5000000 0x1000000>;
			ranges;
	
			ethernet@5000000 {
				compatible = "davicom,dm9000";
				reg = <0x5000000 0x2 0x5000004 0x2>;
				interrupt-parent = <&gpx0>;
				interrupts = <6 4>;
				davicom,no-eeprom;
				mac-address = [00 0a 2d a6 55 a2];
			};
		};		
	}		

	{//----dm9000.c    platform_driver	 (通过 .compatible = "davicom,dm9000" 找到platform_device)
			#ifdef CONFIG_OF
			static const struct of_device_id dm9000_of_matches[] = {
				{ .compatible = "davicom,dm9000", },
				{ /* sentinel */ }
			};
			MODULE_DEVICE_TABLE(of, dm9000_of_matches);
			#endif
			
			static struct platform_driver dm9000_driver = {
				.driver	= {
					.name    = "dm9000",
					.owner	 = THIS_MODULE,
					.of_match_table = of_match_ptr(dm9000_of_matches),
				},
				.probe   = dm9000_probe,
				.remove  = dm9000_drv_remove,
			};
			
			module_platform_driver(dm9000_driver);
	}
	
	{//? 用设备树方式实现LED灯控制  
			1、	进入内核源码修改 arch/arm/boot/dts/exynos4412-fs4412.dts文件,添加如下内容：
				fs4412-led {
					compatible = "fs4412,led"; 
					reg = <0x11000c40 1>,<0x11000c44 1>;
				};
			2、更改驱动 把字符设备cdev 移入驱动 里的平台设备框架中，并更改资源获取方式为IORESOURCE_MEM
				led_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
				if (led_res == NULL) {
					printk("No resource !\n");
					return -ENODEV;
				}
				printk("led_res->start=%x\n",led_res->start);
			
				led_res2 = platform_get_resource(pdev, IORESOURCE_MEM, 1);
				if (led_res2 == NULL) {
					printk("No resource !\n");
					return -ENODEV;
				}
				printk("led_res2->start=%x\n",led_res2->start);	
	}		
	
}	

}

{//============================Linux中断机制============================= 
{//？为何要用到中断机制  
   如果没有中断，CPU就只有用轮询的方式，那样会增加CPU负担，且轮询有时间间隔，
 而中断马上打断CPU当前运行的任务，先进行中断处理后再返回原来的任务执行。响应快。
}

{//？linux的中断机制是如何实现的，它和别的系统的中断机制有何区别
	 相同的： 中断上下文切换，如保留现场，中断向量表跳转，调用中断处理程序，恢复现场等。
	 linux 特有的： 上半部，下半部
	 
	 中断号                                 
	    |   |   |
	  ---------------
	  |  中断控制器 |
	  ---------------
	         |
       ---------
	     |上半部 |                          //如： 用 request_irq申请的 中断处理程序
	     ---------   
	         |  
	       -----
	       |   | -- HI_SOFTIRQ
	       |下 | -- TIMER_SOFTIRQ
	       |   | -- NET_TX_SOFTIRQ
	       |半 | -- NET_RX_SOFTIRQ
	       |   |
	       |部 | -- TASKLET_SOFTIRQ       //用于中断推迟执行，运行于中断的上下文，不能睡眠
	       |   |
	       |---|
	         |
	   -----------------      
  	 |                |  --   task   (时间片调度任务  对应应用中的进程)
	   |   内核 调度    |  --   workqueue   //工作队列，可用于任务的推迟执行，运行于进程上下文， 可睡眠
	   |                |  --   ksoftirq   
	   -----------------      


   {//--中断使用
			   disable_irq(int irq); //屏蔽指定中断号的中断
			   enable_irq(int irq);  //使能某中断号的中断
			   
				request_irq(unsigned int irq,   //硬件中断号
				            irq_handler_t handler,  //中断处理函数
				            unsigned long flags,  //中断标志
	    							const char *name,  //中断名称 
	    							void *dev)			   
			   			   
			   void free_irq(unsigned int irq,void *dev_id);  //释放指定的中断， irq表示中断号  dev_id中断共享时用，一般为NULL
			      
			   //中断中耗时部分的推迟处理 (可选)
		      推迟处理我们一般用tasklet  和workqueue
		      DECLARE_TASKLET (xxx_tasklet, xxx_do_tasklet, 0) 注册tasklet
		      tasklet_schedule(&xxx_tasklet)  触发tasklet		 
		      
		      static void xxx_do_tasklet(unsigned long arg)       
	 }

}

{//？如何使用中断
	1、	进入内核源码修改 arch/arm/boot/dts/exynos4412-fs4412.dts文件,添加如下内容：
		fs4412-key {
			compatible = "fs4412,key";
			interrupt-parent = <&gpx1>;  /*对应arch\arm\boot\dts\exynos4x12-pinctrl.dtsi 中的
											gpx1: gpx1 {
												gpio-controller;
												#gpio-cells = <2>;
									
												interrupt-controller;
												interrupt-parent = <&gic>;
												interrupts = <0 24 0>, <0 25 0>, <0 26 0>, <0 27 0>,
													     <0 28 0>, <0 29 0>, <0 30 0>, <0 31 0>;
												#interrupt-cells = <2>;
											};	
			                             */
			interrupts = <1 2>;<2,2>  /* 1 对应 gpx1 第二组中断 <0 25 0> 。  2 对应触发方式（ 详见 Documentation/devicetree/bindings/arm/gic.txt  2 = high-to-low edge triggered）
			         25由来：电路图（K2->UART_RING->XEINT9）-》芯片手册(9.2 Interrupt Source的   25  57  – EINT[9] External Interrupt）			                             
			                      */   
		};
	2、	重新编译dts文件并拷贝到/tftpboot目录下
	$ make dtbs
	$ cp arch/arm/boot/dts/exynos4412-fs4412.dtb /tftpboot
	3、	拷贝模块源码目录
	将实验代码中的fs4412_key目录拷贝到/home/linux目录下并进入该目录。
	4、	编译模块
	$ make
	5、	将ko文件和测试程序拷贝到根文件系统中
	$ cp *.ko  /source/rootfs 
	6、	板子上插入模块
	# sudo  insmod  fs4412_key.ko
	7、	测试功能
	按key2查看现象
	  irqno = 168
  	
  {//---fs4412_key.c
  	#include <linux/kernel.h>
		#include <linux/module.h>
		#include <linux/platform_device.h>
		#include <linux/of.h>
		#include <linux/interrupt.h>
		#include <linux/workqueue.h>
		MODULE_LICENSE("GPL");
		
		struct resource *key1_res;
		struct work_struct my_wq;
		
		void my_delay_work(unsigned long arg)
		{
			 printk(" used to do need more time things jiffies=%d",jiffies);
	  }
		
		irqreturn_t key_interrupt(int irqno, void *devid)
		{
			printk("irqno = %d\n", irqno);
		  schedule_work(&my_wq);
			return IRQ_HANDLED; 
		}
		
		int fs4412_dt_probe(struct platform_device *pdev) 
		{
			int ret; 
			int irqflags;
		
			printk("match OK\n");
		
			key1_res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
			if (key1_res == NULL) {
				printk("No resource !\n");
				return -ENODEV;
			}
		
			irqflags = IRQF_DISABLED | (key1_res->flags & IRQF_TRIGGER_MASK);  // key1_res->flags 时中断触发方式 对应exynos4412-fs4412.dts中fs4412-key  interrupts = <1 2>中的2   
				
			ret = request_irq(key1_res->start,  //硬件中断号  对应exynos4412-fs4412.dts中fs4412-key  interrupts = <1 2>中的1  
			                  key_interrupt,  //中断处理函数
			                  irqflags,  //中断标志   IRQF_DISABLED 在表示中断处理时屏蔽掉其它所有中断 
			                  "key",  //中断名称  在cat /proc/interrupts  克看到
			                  NULL);
			if (ret < 0) {
				printk("failed request irq: irqno = irq_res->start");
				return ret; 
			}
		
		  INIT_WORK(&my_wq,(void *)my_delay_work);
			return 0;
		}
		
		int fs4412_dt_remove(struct platform_device *pdev)
		{
			free_irq(key1_res->start, NULL);
			printk("remove OK\n");
			return 0;
		}
		
		static const struct of_device_id fs4412_dt_of_matches[] = {
			{ .compatible = "fs4412,key"},
			{ /* nothing to done! */},
		};
		
		MODULE_DEVICE_TABLE(of, fs4412_dt_of_matches);
		
		struct platform_driver fs4412_dt_driver = {
			.driver = {
				.name = "fs4412-dt",
				.owner = THIS_MODULE,
				.of_match_table = of_match_ptr(fs4412_dt_of_matches),
			},
			.probe = fs4412_dt_probe,
			.remove = fs4412_dt_remove,
		};
		
		module_platform_driver(fs4412_dt_driver);  		
  }		   
  
}

}

{//=============================杂项小知识点=============================
	
{//------------------------------时间相关--------------------------- jiffies ,长短延时 ，定时器
jiffies:
	  全局变量jiffies用来记录自系统启动以来产生的节拍的总数      // Linux核心每隔固定周期会发出一次timer interrupt (IRQ 0)，Jiffies会相应的加一
	  启动时，内核将该变量初始化为0，此后，每次时钟中断处理程序都会增加该变量的值。
	  一秒内时钟中断的次数等于Hz，所以jiffies一秒内增加的值也就是HZ
	  
短延时：  //接近或短于一个jiffies
    udelay  mdelay 是软循环忙等待，消耗CPU时间， 这里测下来1个jiffies = 4ms 所以，使用时延时小于4ms的可用它们。
    
长延时：        
    用查询jiffies的方法   如 time_after
    内核调度超时           schedule_timeout(signed long timeout)     
    基于等待队列的睡眠超时 sleep_on_timeout(wait_queue_head_t *q, long timeout)   //最后也是调用内核schedule
     

{//-----定时器例子
		#cd time
		#make
		#insmod hello.ko
		#gcc test.c
		#./a.out 
		   递增显示
		   seconds after open /dev/hello:1
	     seconds after open /dev/hello:2
	     seconds after open /dev/hello:3
     	
	{//---time/hello.c
	  atomic_t sec_count;
	  struct timer_list s_timer; /*设备要使用的定时器*/
	  
	  /*定时器处理函数*/
	  static void second_timer_handle(unsigned long arg)
	  {
	    mod_timer(&s_timer,jiffies + HZ); //modify a timer's timeout
	    atomic_inc(&sec_count);
	    printk("current jiffies is %ld\n", jiffies);
	  }
	  
	  static int hello_open (struct inode *inode, struct file *file)
	  {
	    /*初始化定时器*/
	    init_timer(&s_timer);
	    s_timer.function = &second_timer_handle;
	    s_timer.expires = jiffies + HZ;   //超时值是一个jiffies值，当jiffies值大于超时值timer->expires时，timer->function函数就会被运行
	  
	    add_timer(&s_timer); /*添加（注册）定时器*/
	    atomic_set(&sec_count,0);//计数清0
	    return 0;
	  }
	
	  ssize_t hello_read (struct file *filp, char *buff, size_t count, loff_t *offp)
	  {
	     int counter; 
	     counter = atomic_read(&sec_count);
	     if (put_user(counter,(int *)buff))   //Write a simple value into user space.     
	       return -EFAULT;
	     return sizeof(unsigned int);
	  }
  }

}
}	

{//------------------------linux内核内存的使用----------------------
	
	应用	
		 ------------------------------------------
		 |          malloc       mmap             |    
		 ------------------------------------------	 	
		                    |
		 ------------------------------------------
		 |        open  read  write  ioctl        |    
		 ------------------------------------------	 	
		                    |
	=================================================
	内核                  |	
		 ------------------------------------------
		 |          系统调用  system call         | 
		 ------------------------------------------
		                    |  
		 ------------------------------------------
		 |             文件系统   fs              | 
		 ------------------------------------------
		                    |
	   |---------------------------   ----------- //vmalloc  物理上不连续  会睡眠， 效率低，一般不得已在分配大块内存时才使用        
	   | kmalloc   get_free_pages |	  |         | //kmalloc(<128k) 基于kmem_cache  物理连续  常用GFP_KERNEL， 中断时要用GFP_ATOMIC（不可睡眠）  适合分配小于128K的   
		 ----------------------------   | vmalloc | //get_free_pages(n)  < 4M                                                                                                 
		 |       kmalloc_cache      |   |         | //kmem_cache_alloc  基于slab  适合分配和释放数据结构。 （slab是反复分配同一大小的，会保留原来的数据，不再重初始化）                                      
		 ------------------------------------------
		                    |
		 ------------------------------------------
		 |                 MMU                    |
		 ------------------------------------------
		 |         设备缓存 Buffer Cache          |
		 ------------------------------------------
		                    |
		 ------------------------------------------
		 |   ioremap(动态映射)   map_dec(静态)    |     - gpio_led 0xc000050
		 ------------------------------------------	     
	=================================================	  
	硬件                  |                     
		 ------------------------------------------
		 |                  设备                  |    - 0xe00001c0
		 ------------------------------------------ 		

}

{//-------------------------io内存空间 和io端口空间-----------------
	
 //---I/O 内存空间:  I/O 和内存统一编址  如arm powerpc等
   ---------------------
   | request_mem_region |     //申请内存区域， 非必须的，但使用它较安全，系统会自动进行互斥处理
   ----------------------   
   |      ioremap       |     //动态映射硬件物理地址到内核地址空间     
   ----------------------
   | ioread32 iowrite32 |     //I/O 内存对应的读写函数 
   | readl     writel   |
   ----------------------
   |      iounmap       |     //解除ioremap的动态映射
   ----------------------  
   | release_mem_region |     //释放申请的内存区域
   ----------------------

 //---I/O 端口空间:  I/O 和内存独立编址  如 x86
   ---------------------
   |   request_region   |     
   ----------------------     
   |     inb   outb     |  
   ----------------------
   |   release_region   |
   ----------------------	
}	
	
{//--------------------在/dev 下自动创建设备节点--------------------(管理设备文件方法演变   mkdnod  -- > devfs(动态 内核)  --> udev(真实 用户) ) 
		//？什么是devfs,它有何优缺点
		  是由Linux2.4 引入的，它使得设备驱动能管理自己，如创建删除/dev，修改权限等。 
		   优:
		    1. 无需手动mknod ，它可devfs_mk_cdev 等自动生成
		    2. 不需要指定主次设备号，传0的主设备号，它会动态获取。 
		   缺:
		   	1. 不确定的设备映射,
		   	2. /dev下文件太多，且不是表示当前系统实际的设备，
		   
		//？什么是udev,它有何优缺点  
		  是由linux2.6 引入的用于替代devfs。    
		   优:
		   	1. 稳定的设备映射
		    2. /dev下只包含系统真实存在的设备
		    3. 无需手动mknod，用device_create会自动创建
				 
				#include <linux/kernel.h>      
        struct class *my_class;	
        
        static int s5pv210_led_init(void)
       {  
        		.....      
							/* creating your own class */
						my_class = class_create(THIS_MODULE, "farsight_class");
						if (IS_ERR(my_class)) {
							printk("Err: failed in creating class.\n");
							return ;
						}
					
						/* register your own device in sysfs, and this will cause udevd to create corresponding device node */
						device_create(my_class,NULL, devno, NULL,"farsight_dev");      
			}
				
				  
}	

{//-----------------------------map映射机制-------------------------
//---静态映射
  map_dec      //静态映射 硬件物理地址 到内核虚拟地址空间     内核启动时会自动完成该映射 
  
//---动态映射	
	void *ioremap(phys_addr_t addr, unsigned long size)  /* 动态映射 物理地址 到内核地址
																									       phys_addr  起始物理地址
																									       size       映射范围大小，
																									       返回值     映射后的虚拟地址 
																									      */	
{//---mmap 文件映射到内存  
	//？mmap是什么
	{
	   mmap是把文件内容映射到进程的虚拟内存空间, 通过对这段内存的读取和修改，来实现对文件的读取和修改,而不需要再调用read，write等操作。
	
	   void* mmap (  void * addr ,   //指定映射的起始地址, 通常设为NULL, 由系统指定 
	                 size_t len ,    //映射到内存的文件长度
	                 int prot ,      //映射区的保护方式, PROT_EXEC: 映射区可被执行 PROT_READ: 可读  PROT_WRITE: 可写
	                 int flags ,     //MAP_SHARED:写入映射区的数据会复制回文件, 且允许其他映射该文件的进程共享。
	                                 //MAP_PRIVATE:对映射区的写入操作会产生一个映射区的复制(copy-on-write), 对此区域所做的修改不会写回原文件。
	                 int fd ,        //由open返回的文件描述符, 代表要映射的文件
	                 off_t offset    //以文件开始处的偏移量, 必须是分页大小的整数倍, 通常为0, 表示从文件头开始映射
	               )   
	}
	
	{//？为什么用mmap
		1. 用mmap替代文件操作(read,write)访问设备，效率高.   它是用户空间访问内核空间的一种高效率方式。
	      mmap        --   映射文件到虚拟内存 
	      read,write  --   copy_to_user  拷贝
	      对比read write，mmap无需每次都copy_to_user拷贝一次. 它仅需一次映射，以后都直接操作内存buf,效率高。
		2. 用共享内存方式，方便进程间通讯，且效率高。 
	       采用共享内存通信的一个显而易 见的好处是效率高，因为进程可以直接读写内存，而不需要任何数据的拷贝。
	     对于像管道和消息队列等通信方式，则需要在内核和用户空间进行四次的数据拷贝，
	     而 共享内存则只拷贝两次数据[1]：一次从输入文件到共享内存区，另一次从共享内存区到输出文件		      
	}
	
	{//？mmap 怎么用
		#include <stdio.h>
		#include<sys/types.h>
		#include<sys/stat.h>
		#include<fcntl.h>
		#include<unistd.h>
		#include<sys/mman.h>
		
		int main()
		{
		 int fd;
		 char *start;
		 //char buf[100];
		 char *buf;
		 
		 /*打开文件*/
		 fd = open("/dev/memdev0",O_RDWR);     
		 buf = (char *)malloc(100);
		 memset(buf, 0, 100);
		 start=mmap(NULL,100,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
		 
		 /* 读出数据 */
		 strcpy(buf,start);
		 sleep (1);
		 printf("buf 1 = %s\n",buf); 
		 
		
		 /* 写入数据 */
		 strcpy(start,"Buf Is Not Null!");
		 memset(buf, 0, 100);
		 strcpy(buf,start);
		 sleep (1);
		 printf("buf 2 = %s\n",buf);
	
		 munmap(start,100); /*解除映射*/
		 free(buf);
		 close(fd); 
		 return 0; 
		}	
	}
}

}

}

{//===============================驱动运用=============================== (第六天考试)
//目的： 利用字符设备框架 和 平台设备框架（设备树）  来控制设备

{//？如何进行linux内核驱动开发
	1. 看电路图 芯片手册  //弄清楚CPU是通过哪些寄存器来控制设备的。   
	2. 套用linux驱动框架  //常用平台设备框架 （里面常嵌入其他的Linux设备类型，如字符设备）
	3. 编写控制程序和调试 //如实现read write ioctrl等功能
}

{//led灯控制 
实验二十一 LED灯驱动编写及测试
【实验内容】
编写一个字符驱动程序操作目标板的LED的亮灭
【实验目的】
掌握GPIO的操作
【实验平台】
	主机：Ubuntu 12.04
	目标机：FS4412
	目标机内核版本：3.14.0
	交叉编译器版本：arm-none-linux-gnueabi-gcc-4.6.4
【实验步骤】
1、	拷贝模块源码目录
将实验代码中的fs4412_led目录拷贝到/home/linux目录下并进入该目录。
2、	编译模块
$ make
3、	编译应用程序
$ arm-none-linux-gnueabi-gcc  test.c  -o  test
4、	将ko文件和测试程序拷贝到根文件系统中
$ cp *.ko test /source/rootfs 
5、	板子上插入模块
# sudo  insmod  fs4412_led.ko
6、	创建设备节点
# mknod  /dev/led  c  500  0
7、	测试功能
# ./test
查看灯的状态
	
}	

{//key键盘控制 
实验二十三 键盘驱动编写
【实验内容】
编写一个字符驱动程序捕捉键盘的动作
【实验目的】
掌握GPIO的操作及linux下中断的使用
【实验平台】
	主机：Ubuntu 12.04
	目标机：FS4412
	目标机内核版本：3.14.0
	交叉编译器版本：arm-none-linux-gnueabi-gcc-4.6.4
【实验步骤】
1、	进入内核源码修改 arm/arm/boot/dts/exynos4412-fs4412.dts文件,添加如下内容：
	fs4412-key {
		compatible = "fs4412,key";
		interrupt-parent = <&gpx1>;
		interrupts = <1 2>, <2 2>;
	};
2、	重新编译dts文件并拷贝到/tftpboot目录下
$ make dtbs
$ cp arch/arm/boot/dts/exynos4412-fs4412.dtb /tftpboot
3、	拷贝模块源码目录
将实验代码中的fs4412_key目录拷贝到/home/linux目录下并进入该目录。
4、	编译模块
$ make
5、	将ko文件和测试程序拷贝到根文件系统中
$ cp *.ko  /source/rootfs 
6、	板子上插入模块
# sudo  insmod  fs4412_key.ko
7、	测试功能
按key2或key3查看现象
	
	
}	

{//pwm控制
实验二十二 PWM驱动编写
【实验内容】
编写一个字符驱动程序操作蜂鸣器发声
【实验目的】
掌握GPIO的操作及linux下pwm timer的使用
【实验平台】
	主机：Ubuntu 12.04
	目标机：FS4412
	目标机内核版本：3.14.0
	交叉编译器版本：arm-none-linux-gnueabi-gcc-4.6.4
【实验步骤】
1、	拷贝模块源码目录
将实验代码中的fs4412_led目录拷贝到/home/linux目录下并进入该目录。
2、	编译模块
$ make
3、	编译应用程序
$ arm-none-linux-gnueabi-gcc pwm_music.c -o pwm_music
4、	将ko文件和测试程序拷贝到根文件系统中
$ cp *.ko pwm_music /source/rootfs 
5、	板子上插入模块
# sudo  insmod  fs4412_pwm.ko
6、	创建设备节点
# mknod  /dev/pwm  c  500  0
7、	测试功能
# ./pwm_music	
	
}		

}

{//================================I2C设备===============================
{//？什么是I2C设备
   硬件：
      CLK  ----- 时钟线
      DATA ----- 数据线    
      
   i2c是一种同步半双工的通信总线，使用i2c总线的设备称为I2C设备。  
}	             
 
{//I2C驱动框架   (2C控制器 和 I2C总线, 通常系统已做好了，不需我们修改)
                            Adapter      
                              |
                              | 
                              |Bus（algorithm）
                      -----------------
                      |       |       |  Driver
                   Client  Client Client  
                   
                   
                   I2c-core 提供管理支持
            

  Adapter     -----   适配器,相当于 I2C控制器
  Bus         -----   I2C总线，涉及 algorithm
  Client      -----   挂载到i2c总线上的i2c从设备
  Driver      -----   Client的驱动     
  
  I2C-core    -----   I2C核心，提供管理支持（如adapter client driver 的添加注册)     	
 
}

{//实验二十六 I2C驱动编写及测试
	【实验内容】
	编写一个基于i2c总线的E2PROM的驱动
	【实验目的】
	掌握I2C总线的操作方法
	【实验平台】
		主机：Ubuntu 12.04
		目标机：FS4412
		目标机内核版本：3.14.0
		交叉编译器版本：arm-none-linux-gnueabi-gcc-4.6.4
	【实验步骤】
	注意：在实验过程中"$"后的操作在主机上，"#"后的操作在开发板上
	1、	进入内核源码修改 arcg/arm/boot/dts/exynos4412-fs4412.dts文件,添加如下内容：
		i2c@138B0000 {   //对应 29 Inter-Integrated Circuit  的I2C 5  -->  0x138B_0000
			samsung,i2c-sda-delay = <100>;
			samsung,i2c-max-bus-freq = <20000>;
			pinctrl-0 = <&i2c5_bus>;   //看电路图 I2C_SDA5  I2C_SCL5  --> GPB2  GPB3  --> 见 exynos4x12-pinctrl.dtsi 的 i2c5_bus
 			pinctrl-names = "default";
			status = "okay";
	
			mpu6050-3-axis@68 {
				compatible = "invensense,mpu6050";  //要与驱动里的名字一致
				reg = <0x68>;   /*i2c slave address  见mpu6050 的芯片手册 PS-MPU-6000A-00v3.4.pdf 的  9.2 I2C Interface
				                     The slave address of the MPU-60X0 is b110100X which is 7 bits long. 
				                     The LSB bit of the 7 bit address is determined by the logic level on pin AD0. 
				                     This allows two MPU-60X0s to be connected to the same I2C bus. 
				                     When used in this configuration, the address of the one of the devices should be b1101000 (pin AD0 is logic low)
				                      and the address of the other should be b1101001 (pin AD0 is logic high).
				                   因电路图中 AD0 结地 故 
				                   i2c设备的地址是 b1101000 -->0x68
				                 */ 
				interrupt-parent = <&gpx3>;  //见电路 GYRO_INT -> GPX3_3
				interrupts = <3 2>;  //3 对应 gpx3 的3号管脚 2表示中断的触发方式为 下降沿  
			};

		};
	2、	重新编译dts文件并拷贝到/tftpboot目录下
	$ make dtbs
	$ cp arch/arm/boot/dts/exynos4412-fs4412.dtb /tftpboot
	3、	拷贝模块源码目录
	将实验代码中的mpu6050目录拷贝到/home/linux目录下并进入该目录。
	4、	编译模块
	$ make	
	5、	编译应用程序
	$ arm-none-linux-gnueabi-gcc  test.c  -o  test
	6、	将ko文件和测试程序拷贝到根文件系统中
	$ cp *.ko test /source/rootfs 
	7、	板子上插入模块
	# sudo  insmod  mpu6050.ko
	8、	创建设备节点
	# mknod  /dev/mpu6050  c  500  0
	
	9、	测试功能
	# ./test 
	旋转板子，查看终端显示
	
	//------驱动代码分析 见 mpu6050.c
	static int mpu6050_read_byte(struct i2c_client *client, unsigned char reg)
	{
		int ret;
	
		char txbuf[1] = { reg };
		char rxbuf[1];
	
		struct i2c_msg msg[2] = {   //设定 I2C消息格式 
			{client->addr, 0, 1, txbuf},       //0         指定i2c设备里的寄存器地址   txbuf 存放其地址
			{client->addr, I2C_M_RD, 1, rxbuf} //I2C_M_RD  表示读i2c设备里该寄存器的数据  
		};
	
		ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg)); //传输I2C消息
		if (ret < 0) {
			printk("ret = %d\n", ret);
			return ret;
		}
	
		return rxbuf[0];
  }
  
} 
	
}

{//================================块设备================================

{//？什么是块设备
	能够随机访问固定大小（如512字节）数据片（chunk）的设备被称作块设备

	//块设备和字符设备区别：
	             块设备      字符设备
	  访问单位   固定大小      无        //块设备访问最小单位是扇区sector（常见的是512字节）
	  随机访问   支持          不支持    //块设备为提升效率，对I/O请求有对应的缓冲区，故能随机访问；而字符设备无缓冲区，只能按字节流先后顺序访问
	  直接访问   不可          可        //
	  驱动程序   复杂        相对简单    //虽然块设备较复杂，但是linux系统中块设备支持已相当成熟，驱动开发中较少涉及， 掌握它的原理即可      
}		

{//块设备原理
	//-----磁盘设备硬件原理
    磁盘 -->磁头 -> 磁道 -> 扇区 -> 柱面（多磁盘叠加后，磁道形成柱面）
   
	//-----软件层次框
	  应用	  
	        open read 
	  =======================
	  内核      |	  
	     ----------------
	     | System  Call |
	     ----------------	
	            |	  
	     --------------------------
	     |     VFS                | //虚拟文件系统， 它是linux下所有文件系统的虚类接口。  ext2 jffs2 rootfs(ram fs) 等是它的具体实现
	     --------------------------	
	            |
	     --------------------
	     |    File  System  |   // ext2, ext3, jffs2 等
	     --------------------      
	            |	  	     
	            |           
	  	 -------------- 
	  	 |Buffer Cache|   //缓冲区是为了加快块设备读写， 优先读缓冲区，没有时才通过File  System 去读取
	  	 --------------    /* buffer_head 缓冲区头信息
						 |				 struct  buffer_head  {
						 |				    unsigned long    b_state;          //缓冲区状态标志
						 |				    struct buffer_head    *b_this_page;//页面中的缓冲区
						 |				    struct page    *b_page;            //存储缓冲区的页面
						 |				    sector_t    b_blocknr;         //逻辑块号
						 |				    size_t    b_size;              //块大小
						 |				    char    *b_data;               //页面中的缓冲区
						 |				    struct block_device    *b_bdev; //块设备
						 |				    bh_end_io_t    *b_end_io;       //I/O完成方法
						 |				     void    *b_private;          //完成方法数据
						 |				    struct list_head   b_assoc_buffers;  //相关映射链表
						 |				    struct address_space   *b_assoc_map;     //mapping this buffer is  associated with 
						 |				    atomic_t    b_count;           //缓冲区使用计数
						 |				}; 
	  	       |          */
             |      
	     -----------------------
	     | generic block layer |   //通用块层
	     ----------------------- /*  bio 是块I/O操作的基本容器 。  通常一个bio对应一个I/O请求 ，一个请求可以包含多个bio 
	            |            struct bio      代表了正在现场的（活动的）以片段（segment）链表形式组织的块I/O操作，
							|						 {
							|							    sector_t bi_sector;  //要传输的第一个扇区 
							|							    struct bio *bi_next; // 下一个bio 
							|							    struct block_device	*bi_bdev;
							|							    unsigned long bi_flags; // 状态、命令等 
							|							    unsigned long bi_rw; // 低位表示READ/WRITE，高位表示优先级 
							|							    unsigned short bi_vcnt; // bio_vec数量 
							|						      unsigned short bi_idx; // 当前bvl_vec索引 
							|						      unsigned short bi_phys_segments; //不相邻的物理段的数目
							|						      .....
							|						}
	            |                */
	            |	              
	     ----------------
	     | I/O schedule |    //通过I/O调度算法（如电梯算法），使得性能最大化
	     ----------------    
	            |
	            |  Request Queue /*  I/O 请求队列，它允许使用多个I/O调度器，以最优方式向驱动提交I/O请求。
							|						struct request_queue 
							|						{
							|						   spinlock_t *queue_lock;// 保护队列结构体的自旋锁
							|						  unsigned long nr_requests; /// 最大的请求数量 
							|					    unsigned short max_sectors;  //最大的扇区数 
							|					    unsigned short hardsect_size;  // 硬件扇区尺寸
							|				      	…
							|					  }
              |
	            |           struct request
							|						{
							|								struct list_head queuelist; //链表结构
							|								unsigned long flags; // REQ_
							|								sector_t sector; // 要传送输的下一个扇区
							|								unsigned long nr_sectors; //要传送的扇区数目														
							|								unsigned int current_nr_sectors; //当前要传送的扇区数目
							|								sector_t hard_sector; //要完成的下一个扇区
							|								unsigned long hard_nr_sectors; //要被完成的扇区数目															
							|								unsigned int hard_cur_sectors; //当前要被完成的扇区数目
							|						 		char *buffer; //传送的缓冲，内核虚拟地址
							|						    …
							|						}
	            |                */ 
	     ----------------
	     | block driver |   //通过系统提供的接口函数（如  blk_init_queue blk_alloc_queue elv_next_request( blk_start_queue）,完成对块设备驱动. 
	     ----------------   //驱动只是提出I/O请求，具体何时获取数据由系统绝对
	            |
	  ======================
	  硬件      |
	     ----------------
	     |Disk Flash... |   //块设备有内存，Flash,硬盘disk , U盘等
	     ----------------
}	 
	
{//？如何高效访问块设备 
	request_queue  -->  request  -->  bio  -->  bio_vec  //难点： 为何要用到它们， 它们间的关系是什么
	
	
  //类比举例
    零售     <=================>  字符设备 //应用简单的一次就能获取到数据
                顾客  --- 应用
               零售店 --- 字符设备 
  
  
  
    大宗采购 <=================> 块设备    //应用从硬盘，flash等设备上是获取大量的数据， 效率很关键    	
             大采购商 --- 应用          
                |          |
                |          |
   成都/北京...的订单  request_queue   //request_queue缓存请求，通过 I/O调度算法（如电梯算法），使得资源配置使用最佳，
                |          |              
               公司     硬盘块设备   
                |          |
         成都分公司订单  request   //指定从哪里获取数据（如从哪个扇区，获取多少个扇区数据）
                |          |
    武侯/锦江 ..仓库       bio    //指定数据具体位置（request中可包含多个bio）
	              |          | 
	       2和5号车发货   bio_vec   //指定数据传输到那个内存区域   （内存区域常是离散的，故bio下要包含多个bio_vec）  
	
}			
	
{//实例 用内存虚拟磁盘
  {//---使用
	 	1.	将代码拷贝到虚拟机中
		2.	进入到源码目录，打开Makefile，修改KERNELDIR为移植好的内核源码目录
		3.	执行make命令编译驱动模块
		4.	拷贝驱动模块.ko文件到根文件系统
		5.	打开串口终端，开发板上电，待系统启动并通过NFS挂载好根文件系统后，敲回车键激活终端
		6.	
		7.	在终端中执行insmod fsdsk.ko命令，加载驱动模块，出现以下信息为正常情况，因为没有分区表
			fsdska: unknown partition table
		8.	执行cat /proc/devices命令，查看块设备申请到的主设备号
		[root@farsight /]# cat /proc/devices 
		……
		254 fsdsk
			9.	执行cat /proc/partitions命令，查看当前系统中的分区
		[root@farsight /]# cat /proc/partitions 
			major minor  #blocks  name
			
			  31        0       1024 mtdblock0
			  31        1       3072 mtdblock1
			  31        2       4096 mtdblock2
			  31        3     253952 mtdblock3
			 254        0       8192 fsdska
		10.	执行fdisk /dev/fsdska命令，进行磁盘分区。此时会打印如下信息：
			[root@farsight /]# fdisk /dev/fsdska 
			...
		
			Command (m for help):
		11.	在上面的提示符后面输入n后回车，表示要新建一个分区，此时会打印如下信息
		Command action
		   e   extended
		   p   primary partition (1-4)
		12.	输入p并回车，表示要建立一个主分区，此时输出如下信息
			Partition number (1-4):
		13.	输入1并回车，表示要建立一个编号为1的主分区
		14.	在接下来的两次输入中直接敲回车，表示使用默认的分区开始柱面和结束柱面，此时会打印出如下信息
			First cylinder (1-256, default 1): Using default value 1
			Last cylinder or +size or +sizeM or +sizeK (1-256, default 256): Using default value 256
			
			Command (m for help): 
		15.	最后输入w并回车，表示保存分区信息。输出以下信息表示分区成功
			Command (m for help): w
			The partition table has been altered!
			
			Calling ioctl() to re-read partition table
			 fsdska: fsdska1
		16.	新创建的分区为fsdska1，使用cat /proc/partitions命令，可以查看新的分区信息
			[root@farsight /]# cat /proc/partitions 
			major minor  #blocks  name
			
			  31        0       1024 mtdblock0
			  31        1       3072 mtdblock1
			  31        2       4096 mtdblock2
			  31        3     253952 mtdblock3
			 254        0       8192 fsdska
			 254        1       8184 fsdska1
		17.	执行mkfs.ext2 /dev/fsdska1命令，格式化新创建的分区，输出如下信息表示格式化成功
				[root@farsight /]# mkfs.ext2 /dev/fsdska1
				Filesystem label=
				OS type: Linux
				Block size=1024 (log=0)
				Fragment size=1024 (log=0)
				2048 inodes, 8184 blocks
				409 blocks (5%) reserved for the super user
				First data block=1
				Maximum filesystem blocks=262144
				1 block groups
				8192 blocks per group, 8192 fragments per group
				2048 inodes per group
		18.	执行mount /dev/fsdska1 /mnt命令，将新格式化的分区挂载到/mnt目录
		19.	执行touch /mnt/a.txt命令在/mnt目录下创建a.txt文件
		20.	执行echo 1234567890 > /mnt/a.txt命令，向该文件写入一些数据
		21.	执行cat /mnt/a.txt命令，读取文件的内容，该内容和写入的保持一致，表明文件读写成功
		22.	执行umount /mnt命令，卸载分区
		23.	执行rmmod fsdsk命令，卸载驱动 	
	}
		  
  {//---fsdsk.c   实现最简功能  （request中仅一个bio, 一个bio_vec情况，使用磁盘自带的电梯调度机制 I/O  Schedule）
		#include <linux/init.h>
		#include <linux/kernel.h>
		#include <linux/module.h>
		
		#include <linux/fs.h>
		#include <linux/slab.h>
		#include <linux/genhd.h>
		#include <linux/blkdev.h>
		#include <linux/hdreg.h>
		#include <linux/vmalloc.h>
		
		
		#define FSDSK_MINORS		4   //硬盘的分区数
		#define FSDSK_HEADS		4
		#define	FSDSK_SECTORS		16      //磁盘的扇区数
		#define FSDSK_CYLINDERS		256   //柱面数（多个磁盘的磁道叠加形成柱面）
		#define FSDSK_SECTOR_SIZE	512   //磁盘扇区大小
		#define FSDSK_SECTOR_TOTAL	(FSDSK_HEADS * FSDSK_SECTORS * FSDSK_CYLINDERS)
		#define FSDSK_SIZE		(FSDSK_SECTOR_TOTAL * FSDSK_SECTOR_SIZE)
		
		static int fsdsk_major = 0;
		static char fsdsk_name[] = "fsdsk";
		
		struct fsdsk_dev
		{
			int size;
			u8 *data;
			spinlock_t lock;
			struct gendisk *gd;
			struct request_queue *queue;
		};
		
		static struct fsdsk_dev *bdev = NULL;
		
		static int fsdsk_open(struct block_device *bdev, fmode_t mode)
		{
			return 0;
		}
		
		static int fsdsk_release(struct gendisk *gd, fmode_t mode)
		{
			return 0;
		}
		
		static int fsdsk_ioctl(struct block_device *bdev, fmode_t mode, unsigned cmd, unsigned long arg)
		{
			return 0;
		}
		
		//获取磁盘几何信息
		static int fsdsk_getgeo(struct block_device *bdev, struct hd_geometry *geo)
		{
			geo->cylinders = FSDSK_CYLINDERS;
			geo->heads = FSDSK_HEADS;
			geo->sectors = FSDSK_SECTORS;
			geo->start = 0;
			return 0;
		}
		
		static void fsdsk_request(struct request_queue *q)
		{
			struct request *req;
			unsigned long offset;
			unsigned long nbytes;
			struct fsdsk_dev *bdev;
		
			bdev = q->queuedata;
			req = blk_fetch_request(q);					// 从请求队列中取出第一个请求
			while (req != NULL) {						// 判断是否所有的请求都已处理完成		
				offset = blk_rq_pos(req) * FSDSK_SECTOR_SIZE;			// 得到起始扇区号，将其映射成内存的偏移的字节地址
				nbytes = blk_rq_cur_sectors(req) * FSDSK_SECTOR_SIZE;	// 得到当前要访问的扇区数，转换成字节数
		
				if ((offset + nbytes) > bdev->size) {
					printk (KERN_NOTICE "Beyond-end write (%ld %ld)\n", offset, nbytes);
					return;
				}
		
				if (rq_data_dir(req) == WRITE)
					memcpy(bdev->data + offset, req->buffer, nbytes);
				else
					memcpy(req->buffer, bdev->data + offset, nbytes);
		
				if (!__blk_end_request_cur(req, 0)) {					// 结束当前请求，取出下一个请求
					req = NULL;
				}
			}
		}
		
		static struct block_device_operations fsdsk_fops = {
			.owner = THIS_MODULE,
			.open = fsdsk_open,
			.release = fsdsk_release,
			.ioctl = fsdsk_ioctl,
			.getgeo = fsdsk_getgeo,
		};
		
		static int __init fsdsk_init(void)
		{
			// 申请主设备号，如果参数为0，表示自动分配一个主设备号
			fsdsk_major = register_blkdev(fsdsk_major, fsdsk_name);
			if (fsdsk_major <= 0) {register_blkdev
				printk(KERN_WARNING "fsdsk: unable to get major number\n");
				return -EBUSY;
			}
		
			// 分配设备的结构体内存空间
			bdev = kmalloc(sizeof(struct fsdsk_dev), GFP_KERNEL);
			if (!bdev) {
				printk(KERN_NOTICE "kmalloc failure\n");
				goto unreg_dev;
			}
			memset(bdev, 0, sizeof(struct fsdsk_dev));
		
			// 对虚拟的磁盘大小进行赋值
			bdev->size = FSDSK_SIZE;
			// 分配磁盘空间，用内存来虚拟磁盘
			bdev->data = vmalloc(bdev->size);
			if (!bdev->data) {
				printk(KERN_NOTICE "vmalloc failure\n");
				goto free_dev;
			}
		
			spin_lock_init(&bdev->lock);
			bdev->queue = blk_init_queue(fsdsk_request, &bdev->lock);/* 初始化一个请求队列，用来排队上层对磁盘的请求，但是，
			          请求的排队是由上层执行的，我们只是提供 fsdsk_request这样一个函数来从请求队列中取出请求，并且处理请求。
			          所以fops里面不需要读写的接口fsdsk_request这样一个请求处理函数是由上层在一个合适的时间调用的
								对于这个请求队列，我们提供了一个自旋锁，用来保护这个请求队列。
								我们的fsdsk_request函数处于一个原子上下文当中。内核在决定调用这个函数之前，先要对自旋锁上锁
								这样意味着在这个函数中不能调用能引起休眠的这一些操作
																			           */
			blk_queue_logical_block_size(bdev->queue, FSDSK_SECTOR_SIZE);	// 告诉内核，我们实际设备的物理扇区大小，上层将会把访问的数据大小对齐到实际的物理扇区大小上
			bdev->queue->queuedata = bdev;
		
			bdev->gd = alloc_disk(FSDSK_MINORS);							// 分配gendisk结构体，注意！！！！！ 这个结构体只能动态分配
			if (!bdev->gd) {
				printk (KERN_NOTICE "alloc_disk failure\n");
				goto free_data;
			}
		
			bdev->gd->major = fsdsk_major;
			bdev->gd->first_minor = 0;
			bdev->gd->fops = &fsdsk_fops;
			bdev->gd->queue = bdev->queue;
			bdev->gd->private_data = bdev;
			snprintf(bdev->gd->disk_name, 32, "fsdsk%c", 'a');
			set_capacity(bdev->gd, FSDSK_SECTOR_TOTAL);
			add_disk(bdev->gd);				// 注意!!!!!!!该操作只能在所有初始化完成之后进行。因为该操作执行后，就表示驱动可以去处理请求了
			return 0;
		
		free_data:
			if (bdev->data)
				vfree(bdev->data);
		free_dev:
			if (bdev)
				kfree(bdev);
		unreg_dev:
			unregister_blkdev(fsdsk_major, fsdsk_name);
			return -EFAULT;
		}
		
		static void __exit fsdsk_exit(void)
		{
			if (bdev->gd) {
				del_gendisk(bdev->gd);
				put_disk(bdev->gd);
			}
			if (bdev->queue) {
				blk_cleanup_queue(bdev->queue);
			}
			if (bdev->data)
				vfree(bdev->data);
			kfree(bdev);
			unregister_blkdev(fsdsk_major, "fsdsk");
		}
		
		module_init(fsdsk_init);
		module_exit(fsdsk_exit);
		
		MODULE_LICENSE("Dual BSD/GPL");
		MODULE_AUTHOR("Kevin Jiang <coreteker@gmail.com>");
		MODULE_DESCRIPTION("This is an example for Linux block device driver");	
	}

		
}		

}

{//===============================网络设备===============================
{//网络设备原理
	
	  
	  应用
	                             ---------
	          Socket             |  数据  |
	            |                --------- 
	            |                   ||      //数据是双向传输的
	  ====================================  //数据复制    
	  内核      |                   ||     
	            |                   ||       
	     ----------------           ||      //通过对数据包头的添加删除贯穿整个协议栈 
	     |              |         数据包    //为提升效率仅有两次数据复制，数据调整通过sk_buf中指针来移动位置  
	     |              |        =========   
	     |   传输层     |        |-------|    
	     | （TCP/UDP)   |        | UDP头 | 	     
	     |              |        |-------| 	     
	     |              |        | 数据  | 	     
	     |              |        |-------| 		     
	     |              |        =========     
	     ----------------				    ||
              |                   ||            sk_buf        net_device  //网络设备（为实现无论网络设备硬件如何变化，上层协议不变，抽象出来的）  见include/linux/netdevice.h
	     ----------------           ||          =========       ===========        
	     |              |         数据包        | ....  |       |         |        
	     |              |        =========      |       |       |  配置   |        
	     |              |        |-------|<---  | head  |       |  状态   |        
	     |    网络      |        | IP头  |      |       |       |  统计   |        
	     |   （IP）     |        |-------|<--- 	|data   | ----- |  链表   |  
	     |              |        | UDP头 | 	    |       |       |  流控   |        
	     |              |        |-------| 	    |       |       |函数指针 | //通过函数指针，实现接口抽象（即接口可关联不同的实现）       
	     |              |        | 数据  | 	    |       |       ===========     
	     |              |        |-------| <--- | tail  |            
	     |              |        ========= <--- | end   |        
	     ----------------			      ||          =========            
              |                   || 
	     ----------------           ||
	     |              |         数据包
	     |              |        =========
	     |              |        |-------| 
	     |  数据链路层  |        | MAC头 | 
	     | （Ethernet)  |        |-------|
	     |              |        | IP头  | 
	     |              |        |-------| 	     
	     |              |        | UDP头 | 	     
	     |              |        |-------| 	     
	     |              |        | 数据  | 	     
	     |              |        |-------| 		     
	     |              |        =========       
	     ----------------			      ||
	            |                   ||        
	            |                   ||
	     -------------------------------
	     |                             | dm9000_probe //初始化
	     |           驱动程序          | dm9000_start_xmit  //网卡发送数据包
	     |                             | dm9000_interrupt-->dm9000_rx->netif_rx //通过中断方式来获取数据
	     -------------------------------   
	            |
	            |  
	  =====================================  // 数据复制
	  硬件      |                   ||
	     ----------------        ----------
	     |    物理层    |        | 适配器 |
	     | (网络适配器) |  	     | 缓冲区 |
	     |网卡(如dm9000)|        ----------
	     | PHY 物理层   |
	     ----------------	       
	
}	

{//实例 dm9000 驱动分析
 {//硬件
 	  //见芯片手册  DM9000AE.pdf
		 	  
		 	          --------DM90000内部框图----------   //DM9000硬件集成了PHY MAC ，实现了物理层 数据链路层的功能
		 	          |   -------    -----  --------  |   //PHY:  物理接口收发器，它实现物理层（定义数据传送与所需的电与光信号、线路状态、数据编码等）  
		  TX+/-  ---|   | PHY | -- |MII|--|  MAC |  |   //MII:  媒体独立接口  连接PHY 和MAC , 实现 MAC不变情况下，适应不同的PHY设备
		 	RX+/-  ---|   -------    -----  --------  |   //MAC:  (Media Access Control) 属于数据链路层（数据帧的构建、数据差错检查、传送控制、向网络层提供标准的数据接口）。 
		 	          |         -------               |  
		 	          |         |SRAM |               |   //SRAM:  芯片内部自带16K SARM(3KB用来发送，13KB用来接收).     
		 	          |         -------               |
		 	          |--------------------------------
		
		//见电路图 
		 	         
 }
 	
	{//----arch/arm/boot/dts/exynos4412-fs4412.dts   新的platform_device方式 -> 设备树		
		srom-cs2@5000000 {   /*网卡的基地址
			电路图 Xm0cs1  -->芯片手册 SROM Controller 19.3 I/O Description 的 nGCS[3:0] Bank selection signal  Xm0CSn_x 
			               -->3 Memory Map  的  0x0500_0000 0x0600_0000 16 MB  Bank1 of SMC				             
			                   */
			compatible = "simple-bus"; //"simple-bus"指SoC片上总线（类似platform的bus）， 是虚拟总线
			#address-cells = <1>;
			#size-cells = <1>;
			reg = <0x5000000 0x1000000>;  //对应芯片手册 3 Memory Map 的0x0500_0000  和 16 MB  （0x1000000就是16M）
			ranges;

			ethernet@5000000 {
				compatible = "davicom,dm9000";  //内核通过该名字来匹配驱动 （对应 drivers/net/ethernet/davicom/dm9000.c 里的 platform_driver的 .name = "dm9000")
				reg = <0x5000000 0x2 0x5000004 0x2>; /*dm9000内部有256个寄存器，通过两组接口和外面打交道 （地址寄存器和数据寄存器）
		0x5000000      对应    Xm0ADDR2 -- CMD=0  表示指向的是地址寄存器 （指定操作的是256个寄存器中的那个）
		      0x2      对应    数据线宽度2个字节（16位) -->  电路 Xm0DATA0~Xm0DATA15.  （如果是64位的总线，就用4）
		0x5000004      对应		 Xm0ADDR2 -- CMD=1  --->0x4  指定数据寄存器  
				   */
				interrupt-parent = <&gpx0>;  //中断控制器继承于gpx0
				interrupts = <6 4>; /*详见 Documentation/devicetree/bindings/arm/gic.txt
				6  对应中断源 DM9000_IRQ -> XEINT6 。 
				4 = active high level-sensitive 高电平触发中断
				  */					  
				davicom,no-eeprom;
				mac-address = [00 0a 2d a6 55 a2];  //网卡物理地址
			};
			
		};		
		
			
	}	 	
 	
 	
 {// drivers/net/dm9000.c 源码分析

			/*
			 *   Write a byte to I/O port
			 */
			
			static void
			iow(board_info_t * db, int reg, int value)   //db->io_addr 获取 见dm9000_probe（它从平台设备中获取资源，用ioremap 映射地址）
			{
				writeb(reg, db->io_addr);
				writeb(value, db->io_data);
			}			
			
			
			static void dm9000_timeout(struct net_device *dev) //传输超时时调用该函数，超时时间由watchdog设定。
			{
			}



			/*
			 *  Hardware start transmission.
			 *  Send a packet to media from the upper layer. 
			 */
			static int
			dm9000_start_xmit(struct sk_buff *skb, struct net_device *dev) //向硬件发送数据包（被上层调用）
			{
			 unsigned long flags;
			 board_info_t *db = netdev_priv(dev);
			
			 dm9000_dbg(db, 3, "%s:\n", __func__);
			/*一次最多只能发送两个数据包。即网卡SRAM中只能存在两个待发送的数据包。
			 DM9000A内部有一个4K Dword SRAM，其中3KB是作为发送，16KB作为接收，
			 其中0x0000~0x0BFF是传说中的TX buffer(TX buffer中只能存放两个包)，0x0C00~0x3FFF是RX buffer。
			 因此在写内存操作时，当IMR的第7位被设置，如果到达了地址的结尾比如到了3KB，则回卷到0。相似的方式，
			 在读操作中，当IMR的第7位被设置如果到达了地址的结尾比如16K，则回卷到0x0C00。
			*/
			 if (db->tx_pkt_cnt > 1)
			  return 1;
			
			 spin_lock_irqsave(&db->lock, flags);
			
			 /* Move data to DM9000 TX RAM */
			 /*
			 MWCMD（F8H）：存储器读地址自动增加的读数据命令.
			 7-0：MWCMD：写数据到发送SRAM中，之后指向内部SRAM的读指针自动
			 增加1、2或4，根据处理器的操作模式而定（8位、16位或32位）。 
			 */
			 writeb(DM9000_MWCMD, db->io_addr);
			
			 (db->outblk)(db->io_data, skb->data, skb->len);
			 dev->stats.tx_bytes += skb->len;
			
			 db->tx_pkt_cnt++;//记录写入网卡SRAM中待发送的数据包的数量
			 /* TX control: First packet immediately send, second packet queue */
			 if (db->tx_pkt_cnt == 1) {
			  /* Set TX length to DM9000 */
			  iow(db, DM9000_TXPLL, skb->len);
			  iow(db, DM9000_TXPLH, skb->len >> 8);
			//如果写入网卡SRAM中的数据包只有一个，则将数据包的长度写入TXPLL和TXPLH中
			  /* Issue TX polling command */
			//TCR（02H）：发送控制寄存器。0：TXREQ：TX（发送）请求。发送完成后自动清零该位。
			  iow(db, DM9000_TCR, TCR_TXREQ); /* Cleared after TX complete */
			  dev->trans_start = jiffies; /* save the time stamp */
			 } else {
			  /* Second packet */
			  db->queue_pkt_len = skb->len; //如果当前写入的数据包不是的一个，则把该数据包的长度存入db->queue_pkt_len中		
			  netif_stop_queue(dev); //只能同时存在两个待发数据包。调用函数告知网络系统不要再启动发送
			 }
			
			 spin_unlock_irqrestore(&db->lock, flags);
			
			 /* free this SKB */
			/*
			每个数据包写入网卡SRAM后都要释放skb。
			如果有两个数据包要将第二个数据包的长度存入db->queue_pkt_len = skb->len
			*/
			 dev_kfree_skb(skb);
			
			 return 0;
			}


			/*
			 *  Received a packet and pass to upper layer
			 */ 
			static void
			dm9000_rx(struct net_device *dev)  //接收一个数据包存入缓存skb，并通过函数netif_rx将该缓存交给上层
			{
			 board_info_t *db = netdev_priv(dev);
			 struct dm9000_rxhdr rxhdr;
			 struct sk_buff *skb;
			 u8 rxbyte, *rdptr;
			 bool GoodPacket;
			 int RxLen;
			
			 /* Check packet ready or not */
			 do {
			  /*
			  MRCMDX（F0H）：存储器地址不变的读数据命令.
			  始终读取数据包的第一个字节，直到读到01H（即有效数据包）为止。
			  */
			  ior(db, DM9000_MRCMDX); /* Dummy read */
			
			  /* Get most updated data */
			  rxbyte = readb(db->io_data);
			
			  /* Status check: this byte must be 0 or 1 */
			  if (rxbyte > DM9000_PKT_RDY) {
			   dev_warn(db->dev, "status check fail: %d\n", rxbyte);
			   iow(db, DM9000_RCR, 0x00); /* Stop Device   DM9000_RCR 定义见 drivers/net/dm9000.h*/     
			   iow(db, DM9000_ISR, IMR_PAR); /* Stop INT request */
			   return;
			  }
			//如果数据包第一字节为00H则为无效数据包。
			  if (rxbyte != DM9000_PKT_RDY)
			   return;
			
			  /* A packet ready now  & Get status/length */
			  GoodPacket = true;
			  //MRCMD（F2H）：存储器读地址自动增加的读数据命令。
			  writeb(DM9000_MRCMD, db->io_addr);
			//读取数据包的前四字节，即有效标志，接受状态，数据包长度。存于结构体rxhdr中
			  (db->inblk)(db->io_data, &rxhdr, sizeof(rxhdr));
			
			  RxLen = le16_to_cpu(rxhdr.RxLen);
			
			  if (netif_msg_rx_status(db))
			   dev_dbg(db->dev, "RX: status %02x, length %04x\n",
			    rxhdr.RxStatus, RxLen);
			
			  /* Packet Status check */
			  if (RxLen < 0x40) {//一个数据包的长度应大于64字节
			   GoodPacket = false;
			   if (netif_msg_rx_err(db))
			    dev_dbg(db->dev, "RX: Bad Packet (runt)\n");
			  }
			
			  if (RxLen > DM9000_PKT_MAX) {//数据包长度不应大于1.5K
			   dev_dbg(db->dev, "RST: RX Len:%x\n", RxLen);
			  }
			
			  /* rxhdr.RxStatus is identical to RSR register. */
			  //rxhdr.RxStatus的值既是RSR（06H）：接收状态寄存器
			  //对各种错误进行判断和记录。
			  if (rxhdr.RxStatus & (RSR_FOE | RSR_CE | RSR_AE |
			          RSR_PLE | RSR_RWTO |
			          RSR_LCS | RSR_RF)) {
			   GoodPacket = false;
			   if (rxhdr.RxStatus & RSR_FOE) {
			    if (netif_msg_rx_err(db))
			     dev_dbg(db->dev, "fifo error\n");
			    dev->stats.rx_fifo_errors++;
			   }
			   if (rxhdr.RxStatus & RSR_CE) {
			    if (netif_msg_rx_err(db))
			     dev_dbg(db->dev, "crc error\n");
			    dev->stats.rx_crc_errors++;
			   }
			   if (rxhdr.RxStatus & RSR_RF) {
			    if (netif_msg_rx_err(db))
			     dev_dbg(db->dev, "length error\n");
			    dev->stats.rx_length_errors++;
			   }
			  }
			
			  /* Move data from DM9000 */
			  //如果是一个好的数据包则分配skb结构体，和足够缓存，并将数据读入缓存
			  if (GoodPacket
			      && ((skb = dev_alloc_skb(RxLen + 4)) != NULL)) {
			   skb_reserve(skb, 2);
			   rdptr = (u8 *) skb_put(skb, RxLen - 4);
			
			   /* Read received packet from RX SRAM */
			
			   (db->inblk)(db->io_data, rdptr, RxLen);
			   dev->stats.rx_bytes += RxLen;
			
			   /* Pass to upper layer */
			/*
			这个函数抽取协议标识( ETH_P_IP, 在这个情况下 )从以太网头; 
			它也赋值 skb->mac.raw, 从报文 data (使用 skb_pull)去掉硬件头部,
			 并且设置 skb->pkt_type. 最后一项在 skb 分配是缺省为 PACKET_HOST
			 (指示报文是发向这个主机的), eth_type_trans 改变它来反映以太网目的地址: 
			 如果这个地址不匹配接收它的接口地址, pkt_type 成员被设为 
			 PACKET_OTHERHOST. 结果, 除非接口处于混杂模式或者内核打开了报文转发, 
			 netif_rx 丢弃任何类型为 PACKET_OTHERHOST 的报文. 
			 
			//union { /* ... */// } h; 
			//union { /* ... */ } nh; 
			//union { /*... */} mac; 
			//指向报文中包含的各级的头的指针.包含在结构体struct net_device *dev;中
			
			*/   
			   skb->protocol = eth_type_trans(skb, dev);
			/*
			递交 socket 缓存给上层. 实际上 netif_rx 返回一个整数; 
			NET_RX_SUCCESS(0) 意思是报文成功接收; 任何其他值指示错误. 
			有 3 个返回值 (NET_RX_CN_LOW, NET_RX_CN_MOD, 和 NET_RX_CN_HIGH )
			指出网络子系统的递增的拥塞级别; NET_RX_DROP 意思是报文被丢弃
			*/
			   netif_rx(skb);
			   dev->stats.rx_packets++;
			
			  } else {
			   /* need to dump the packet's data */
			//如果该数据包是坏的，则清除该数据包的数据
			   (db->dumpblk)(db->io_data, RxLen);
			  }
			 } while (rxbyte == DM9000_PKT_RDY);//如果是有效数据包则退出
			}
			
			
			/*在一个数据包发送完，一个数据包接收到，网络链路状态改变，触发中断，调用该中断处理函数。
			  在非中断模式下，被函数dm9000_poll_controller调用。
			
			  如果数据包发完，会做一些准备工作，准备发后续的包
			  如果硬件有接收到合法的数据包，则调用dm9000_rx 将数据传到网络的上层
			*/
			static irqreturn_t dm9000_interrupt(int irq, void *dev_id)
			{
			 struct net_device *dev = dev_id;
			 board_info_t *db = netdev_priv(dev);
			 int int_status;
			 unsigned long flags;
			 u8 reg_save;
			
			 dm9000_dbg(db, 3, "entering %s\n", __func__);
			
			 /* A real interrupt coming */
			
			 /* holders of db->lock must always block IRQs */
			 spin_lock_irqsave(&db->lock, flags);
			
			 /* Save previous register address */
			 reg_save = readb(db->io_addr);
			
			 /* Disable all interrupts */
			 //IMR（FFH）：终端屏蔽寄存器。屏蔽所有中断。
			 iow(db, DM9000_IMR, IMR_PAR);
			
			 /* Got DM9000 interrupt status */
			 /*
			 ISR（FEH）：终端状态寄存器.
			 ISR寄存器各状态写1清除.
			 */
			 int_status = ior(db, DM9000_ISR); /* Got ISR */
			 iow(db, DM9000_ISR, int_status); /* Clear ISR status */
			
			 if (netif_msg_intr(db))
			  dev_dbg(db->dev, "interrupt status %02x\n", int_status);
			
			 /* Received the coming packet */
			 //一个中断号可能被多个中断源触发，所以要判断是哪个中断源。
			 if (int_status & ISR_PRS)
			  dm9000_rx(dev);    //调用dm9000_rx  接收网络数据包
			
			 /* Trnasmit Interrupt check */
			 if (int_status & ISR_PTS)
			  dm9000_tx_done(dev, db);
			//DM9000E在链路状态发生改变时不触发中断
			 if (db->type != TYPE_DM9000E) {
			  if (int_status & ISR_LNKCHNG) {
			   /* fire a link-change request */
			   schedule_delayed_work(&db->phy_poll, 1);
			  }
			 }
			
			 /* Re-enable interrupt mask */
			 //重新时使能相应中断
			 iow(db, DM9000_IMR, db->imr_all);
			
			 /* Restore previous register address */
			 writeb(reg_save, db->io_addr);
			
			 spin_unlock_irqrestore(&db->lock, flags);
			
			 return IRQ_HANDLED;
			}

      //NAPI 	当有高速大量数据包时，屏蔽本中断号响应，用轮询方式接收数据包，避免频繁中断给CPU造成的负担  
			static void dm9000_poll_controller(struct net_device *dev)
			{
			        disable_irq(dev->irq);
			        dm9000_interrupt(dev->irq, dev);
			        enable_irq(dev->irq);
			}


		 /*
		 *  Open the interface.
		 *  The interface is opened whenever "ifconfig" actives it.
		 */
		static int
		dm9000_open(struct net_device *dev)  //打开网卡，向内核注册中断，复位并初始化dm9000，检查MII接口，使能传输等
		{
			board_info_t *db = netdev_priv(dev);
			unsigned long irqflags = db->irq_res->flags & IRQF_TRIGGER_MASK;
		
			if (netif_msg_ifup(db))
				dev_dbg(db->dev, "enabling %s\n", dev->name);
		
			/* If there is no IRQ type specified, default to something that
			 * may work, and tell the user that this is a problem */
		
			if (irqflags == IRQF_TRIGGER_NONE)
				dev_warn(db->dev, "WARNING: no IRQ resource flags set.\n");
		
			irqflags |= IRQF_SHARED;
		
			if (request_irq(dev->irq, dm9000_interrupt, irqflags, dev->name, dev))
				return -EAGAIN;
		
			/* Initialize DM9000 board */
			dm9000_reset(db);
			dm9000_init_dm9000(dev);
		
			/* Init driver variable */
			db->dbug_cnt = 0;
		
			mii_check_media(&db->mii, netif_msg_link(db), 1); //MII检测媒介状态，主要检测Link状态
			netif_start_queue(dev); //告诉上层网络驱动层驱动空间有缓冲区可用，开始发送数据包到驱动层
			
			dm9000_schedule_poll(db);//启动延时调度。这里是dm9000_poll_work用于检测网卡连接状态
		
			return 0;
		}


		static const struct net_device_ops dm9000_netdev_ops = {//网络设备操作函数(类似file_operations)
		        .ndo_open               = dm9000_open,
		        .ndo_stop               = dm9000_stop,
		        .ndo_start_xmit         = dm9000_start_xmit,
		        .ndo_tx_timeout         = dm9000_timeout,
		        .ndo_set_multicast_list = dm9000_hash_table,
		        .ndo_do_ioctl           = dm9000_ioctl,
		        .ndo_change_mtu         = eth_change_mtu,
		        .ndo_validate_addr      = eth_validate_addr,
		        .ndo_set_mac_address    = eth_mac_addr,
		#ifdef CONFIG_NET_POLL_CONTROLLER
		        .ndo_poll_controller    = dm9000_poll_controller,  
		#endif
		};


		/*
		 * Search DM9000 board, allocate space and register it
		 */
		static int __devinit
		dm9000_probe(struct platform_device *pdev)    //探测设备获得并保存资源信息，根据这些信息申请内存和中断，最后调用register_netdev注册这个网络设备
		{
			struct dm9000_plat_data *pdata = pdev->dev.platform_data;
			struct board_info *db;	/* Point a board information structure */
			struct net_device *ndev;
			const unsigned char *mac_src;
			int ret = 0;
			int iosize;
			int i;
			u32 id_val;
		
			/* Init network device */
			ndev = alloc_etherdev(sizeof(struct board_info)); //申请网络设备 net_device
			if (!ndev) {
				dev_err(&pdev->dev, "could not allocate device.\n");
				return -ENOMEM;
			}
		
			SET_NETDEV_DEV(ndev, &pdev->dev);   //将platform_device与net_device关联起来
		
			dev_dbg(&pdev->dev, "dm9000_probe()\n");
		
			/* setup board info structure */
			db = netdev_priv(ndev);
		
			db->dev = &pdev->dev;
			db->ndev = ndev;
		
			spin_lock_init(&db->lock);
			mutex_init(&db->addr_lock);
		
			INIT_DELAYED_WORK(&db->phy_poll, dm9000_poll_work);
		
		  //获取平台设备资源。包括DM9000地址寄存器地址，DM9000数据寄存器地址，和DM900所占用的中断号
			db->addr_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
			db->data_res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
			db->irq_res  = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
		
			if (db->addr_res == NULL || db->data_res == NULL ||
			    db->irq_res == NULL) {
				dev_err(db->dev, "insufficient resources\n");
				ret = -ENOENT;
				goto out;
			}
		
			db->irq_wake = platform_get_irq(pdev, 1);
			if (db->irq_wake >= 0) {
				dev_dbg(db->dev, "wakeup irq %d\n", db->irq_wake);
		
				ret = request_irq(db->irq_wake, dm9000_wol_interrupt,
						  IRQF_SHARED, dev_name(db->dev), ndev);
				if (ret) {
					dev_err(db->dev, "cannot get wakeup irq (%d)\n", ret);
				} else {
		
					/* test to see if irq is really wakeup capable */
					ret = set_irq_wake(db->irq_wake, 1);
					if (ret) {
						dev_err(db->dev, "irq %d cannot set wakeup (%d)\n",
							db->irq_wake, ret);
						ret = 0;
					} else {
						set_irq_wake(db->irq_wake, 0);
						db->wake_supported = 1;
					}
				}
			}
		
		 //申请地址寄存器IO内存区域并映射  
			iosize = resource_size(db->addr_res);
			db->addr_req = request_mem_region(db->addr_res->start, iosize,
							  pdev->name);
		
			if (db->addr_req == NULL) {
				dev_err(db->dev, "cannot claim address reg area\n");
				ret = -EIO;
				goto out;
			}
		
			db->io_addr = ioremap(db->addr_res->start, iosize);
		
			if (db->io_addr == NULL) {
				dev_err(db->dev, "failed to ioremap address reg\n");
				ret = -EINVAL;
				goto out;
			}
		  //申请数据寄存器IO内存区域并映射
			iosize = resource_size(db->data_res);
			db->data_req = request_mem_region(db->data_res->start, iosize,
							  pdev->name);
		
			if (db->data_req == NULL) {
				dev_err(db->dev, "cannot claim data reg area\n");
				ret = -EIO;
				goto out;
			}
		
			db->io_data = ioremap(db->data_res->start, iosize);
		
			if (db->io_data == NULL) {
				dev_err(db->dev, "failed to ioremap data reg\n");
				ret = -EINVAL;
				goto out;
			}
		
			/* fill in parameters for net-dev structure */
			ndev->base_addr = (unsigned long)db->io_addr;
			ndev->irq	= db->irq_res->start;
		
			/* ensure at least we have a default set of IO routines */
			dm9000_set_io(db, iosize);
		
			/* check to see if anything is being over-ridden */
			if (pdata != NULL) {
				/* check to see if the driver wants to over-ride the
				 * default IO width */
		
				if (pdata->flags & DM9000_PLATF_8BITONLY)
					dm9000_set_io(db, 1);
		
				if (pdata->flags & DM9000_PLATF_16BITONLY)
					dm9000_set_io(db, 2);
		
				if (pdata->flags & DM9000_PLATF_32BITONLY)
					dm9000_set_io(db, 4);
		
				/* check to see if there are any IO routine
				 * over-rides */
		
				if (pdata->inblk != NULL)
					db->inblk = pdata->inblk;
		
				if (pdata->outblk != NULL)
					db->outblk = pdata->outblk;
		
				if (pdata->dumpblk != NULL)
					db->dumpblk = pdata->dumpblk;
		
				db->flags = pdata->flags;
			}
		
		#ifdef CONFIG_DM9000_FORCE_SIMPLE_PHY_POLL
			db->flags |= DM9000_PLATF_SIMPLE_PHY;
		#endif
		
			dm9000_reset(db);
		
		  //读取设备ID，判断是否是驱动能够处理的网卡芯片
			/* try multiple times, DM9000 sometimes gets the read wrong */
			for (i = 0; i < 8; i++) {
				id_val  = ior(db, DM9000_VIDL);
				id_val |= (u32)ior(db, DM9000_VIDH) << 8;
				id_val |= (u32)ior(db, DM9000_PIDL) << 16;
				id_val |= (u32)ior(db, DM9000_PIDH) << 24;
		
				if (id_val == DM9000_ID)
					break;
				dev_err(db->dev, "read wrong id 0x%08x\n", id_val);
			}
		
			if (id_val != DM9000_ID) {
				dev_err(db->dev, "wrong id: 0x%08x\n", id_val);
				ret = -ENODEV;
				goto out;
			}
		
			/* Identify what type of DM9000 we are working on */
		
			id_val = ior(db, DM9000_CHIPR);
			dev_dbg(db->dev, "dm9000 revision 0x%02x\n", id_val);
		
			switch (id_val) {
			case CHIPR_DM9000A:
				db->type = TYPE_DM9000A;
				break;
			case CHIPR_DM9000B:
				db->type = TYPE_DM9000B;
				break;
			default:
				dev_dbg(db->dev, "ID %02x => defaulting to DM9000E\n", id_val);
				db->type = TYPE_DM9000E;
			}
		
			/* dm9000a/b are capable of hardware checksum offload */
			if (db->type == TYPE_DM9000A || db->type == TYPE_DM9000B) {
				db->can_csum = 1;
				db->rx_csum = 1;
				ndev->features |= NETIF_F_IP_CSUM;
			}
		
			/* from this point we assume that we have found a DM9000 */
		
			/* driver system function */
			ether_setup(ndev);
			
		   //设置网卡芯片的接口函数
			ndev->netdev_ops	= &dm9000_netdev_ops;
			ndev->watchdog_timeo	= msecs_to_jiffies(watchdog);
			ndev->ethtool_ops	= &dm9000_ethtool_ops;  //用于查询和设置网卡参数
		
			db->msg_enable       = NETIF_MSG_LINK;
			db->mii.phy_id_mask  = 0x1f;
			db->mii.reg_num_mask = 0x1f;
			db->mii.force_media  = 0;
			db->mii.full_duplex  = 0;
			db->mii.dev	     = ndev;
			db->mii.mdio_read    = dm9000_phy_read;
			db->mii.mdio_write   = dm9000_phy_write;
		
			mac_src = "eeprom";
		
			/* try reading the node address from the attached EEPROM */
			for (i = 0; i < 6; i += 2)
				dm9000_read_eeprom(db, i / 2, ndev->dev_addr+i);
		
			if (!is_valid_ether_addr(ndev->dev_addr) && pdata != NULL) {
				mac_src = "platform data";
				memcpy(ndev->dev_addr, pdata->dev_addr, 6);
			}
		
			if (!is_valid_ether_addr(ndev->dev_addr)) {
				/* try reading from mac */
				
				mac_src = "chip";
				for (i = 0; i < 6; i++)
					ndev->dev_addr[i] = ior(db, i+DM9000_PAR);
			}
		
			if (!is_valid_ether_addr(ndev->dev_addr))
				dev_warn(db->dev, "%s: Invalid ethernet MAC address. Please "
					 "set using ifconfig\n", ndev->name);
		
			platform_set_drvdata(pdev, ndev);
			ret = register_netdev(ndev);   //注册网络设备驱动
		
			if (ret == 0)
				printk(KERN_INFO "%s: dm9000%c at %p,%p IRQ %d MAC: %pM (%s)\n",
				       ndev->name, dm9000_type_to_char(db->type),
				       db->io_addr, db->io_data, ndev->irq,
				       ndev->dev_addr, mac_src);
			return 0;
		
		out:
			dev_err(db->dev, "not found (%d).\n", ret);
		
			dm9000_release_board(pdev, db);
			free_netdev(ndev);
		
			return ret;
		}
		
		
		static struct platform_driver dm9000_driver = { //把dm9000包装成 平台设备驱动
			.driver	= {
				.name    = "dm9000",
				.owner	 = THIS_MODULE,
				.pm	 = &dm9000_drv_pm_ops,
			},
			.probe   = dm9000_probe,
			.remove  = __devexit_p(dm9000_drv_remove),
		};
		
		static int __init dm9000_init(void)
		{
			printk(KERN_INFO "%s Ethernet Driver, V%s\n", CARDNAME, DRV_VERSION);
		
			return platform_driver_register(&dm9000_driver); //把dm9000封装成了平台设备,  当driver与device名字一致时，才注册成功，调用dm9000_probe    
			                                                 /*dm9000的平台设备定义见 arch/arm/mach-s5pc100/mach-smdkc100.c 的   s5pc100_device_dm9000
									#if  defined(CONFIG_DM9000)
									static struct resource dm9000_resources[] = {  dm9000内部有256个寄存器，通过两组接口和外面打交道 （地址寄存器和数据寄存器）
									[0] = {  对应电路图上cs1 --> 0x88000000   addr2 --cmd=0    表示指向的是地址寄存器 （指定操作的是256个寄存器中的那个）
									.start        = 0x88000000,     网卡像flash等直接挂接到CPU 的BANK上. 这里是SMC   Bank 1
									.end         = 0x88000000 + 0x3,  兼容64位及其以下的总线访问   
									.flags        = IORESOURCE_MEM,
									},
									[1] = {  对应电路图上cs1 --> 0x88000000   addr2 -- cmd=1 --->0x4  指定数据寄存器    
									.start        = 0x88000000 + 0x4,   
									.end        = 0x88000000 + 0x4 +0x3,   兼容64位及其以下的总线访问  
									.flags        = IORESOURCE_MEM,
									},
									[2] = {
									    	.start = IRQ_EINT(10),
									    	.end   = IRQ_EINT(10),
									.flags        = IORESOURCE_IRQ | IRQ_TYPE_LEVEL_HIGH,
									},
									};
									 
									static struct dm9000_plat_data s5pc100_dm9000_platdata = {
									.flags        =   DM9000_PLATF_16BITONLY,   指定总线宽度 (由电路图data0~data15 知道是16位总线）
									.dev_addr[0]  = 0x00,
									.dev_addr[1]  = 0x00,
									.dev_addr[2]  = 0x3e,
									.dev_addr[3]  = 0x26,
									.dev_addr[4]  = 0x0a,
									.dev_addr[5]  = 0x00,
									};
									 
									static struct platform_device s5pc100_device_dm9000 = {
									.name        = "dm9000",
									.id        = -1,
									.num_resources        = ARRAY_SIZE(dm9000_resources),
									.resource        = dm9000_resources,
									.dev = {
									.platform_data  = & s5pc100_dm9000_platdata,
									}
									};
									#endif
									
				          #if  defined(CONFIG_DM9000)
				            &s5pc100_device_dm9000,
				          #endif                                          
			                                               		                                                 
			                                                 */  
		}
		
		module_init(dm9000_init);
		
 }	
	
	
	
	
	
}	
	
}		
	
}


{//环境设置 

	   ------------------------           
     |板子    192.168.9.5    |
     ------------------------
                |
     ------------------------
     |电脑    192.168.9.119  |    //做中转用，和板子虚拟机IP要在同一网段， 且不能和它们IP一样     
     ------------------------
                |
	   ------------------------
     |虚拟机  192.168.9.110   |
     ------------------------

  setenv serverip 192.168.9.110    //注意它与虚拟机里 ubuntu 的ip要一致
  setenv ipaddr 192.168.9.5
	setenv gatewayip  192.168.9.1	
	setenv bootcmd tftp 41000000 uImage\;tftp 42000000 exynos4412-fs4412.dtb\;bootm 41000000 - 42000000
	setenv bootargs root=/dev/nfs nfsroot=192.168.9.110:/nfs/rootfs rw console=ttySAC2,115200 init=/linuxrc ip=192.168.9.5		
	
}	


{//作业
	
	1. 实现跑马灯效果
	
	2. 两个应用程序(同时）控制驱动, 一个实现三个灯闪烁5秒，另一个实现跑马灯效果。 
	
	3. 一个应用程序同时控制三个驱动， 一个驱动控制灯，一个控制蜂蜜器鸣叫。一个驱动（用于接收字符,字符1 ，灯亮，字符p,蜂蜜器叫）
	
	4. 实现按键k2按下，灯闪烁，按键k3按下，蜂鸣器叫
	
	
   熟悉平台设备框架 和设备树   

   用tasklet实现推迟处理  //参考workqueue
   
   实现按键key3的中断     //参考key2中断
   
   实现按键控制灯闪烁     //用设备树描述LED寄存器地址，用推迟处理处理 耗时的事情   	
}	    