{//***********************************��������************************************

{//��ʲô������
	������ֱ�Ӻ�Ӳ���򽻵����������
	
	--------
	| Ӧ�� |
	--------
	    |
	---------   
	| ����  |
	---------
	    |
	---------    
	| Ӳ��  |
	---------
	 
}

{/*���ȥѧϰ linux��������
  linux �����漰����Ӳ��֪ʶ�࣬�ܹ����ӡ�
  ��Ҫ������ϸ�� ����ΪŪ����ĳ�δ��룬����������ȥ��
  ��ô�ʹ�õĽǶȣ� ����ϣ�����Ͻ��а��ա�
  ����ʵ�飬ͨ��ʵ�����������������ȽϿ졣
}

{//��ѧ���Ҫ���ʲô������   
 1. ��Ϥlinux�����ܹ��Ϳ�������
 2. ��������������ٷ���
 3. �ܶ��������򵥵�����
}

{//===========================linux�ں�ģ��Ŀ���========================
{//��ʲô���ں�ģ��	  
  ��������ʱ��ӵ��ں��еĴ��뱻��Ϊ��ģ�顱��
	{//���ں�ģ���Ӧ�ó����кβ�ͬ
     	   Ӧ��            --             ģ��
    ��ͷ��β������ 	            Ԥ��ע����ںˣ������ı����õ�              
	     Ӧ�ÿռ�                    �ں˿ռ䣬Ȩ�޴�
	   �δ����ϵͳΣ��С          �δ��󳣻ᵼ��ϵͳ����
	}
}

{//��Ϊ��Ҫ���ں�ģ��	
  1����С�ں��������Ϊģ�鱾�������뵽�ں˾������档
  2���������ں�����ӻ�ɾ�����ܶ��������±����ں�
}

{//���ص�����	
	�������ȥ������ʹ��һ���ں�ģ��
}

{//��һ����򵥵��ں�ģ�����ʵ�ֺ�ʹ�õ�	 �ص� ��Ҫ��
	//---/led/step1/fs4412_led_drv.c
	#include <linux/kernel.h>
	#include <linux/module.h>
	
	MODULE_LICENSE("Dual BSD/GPL");  //ģ���������
	
	int led_init(void)
	{
		printk("Led init\n");
		return 0;
	}
	
	void led_exit(void)
	{
		printk("Led exit\n");
	}
	
	module_init(led_init);  //ģ������������
	module_exit(led_exit);	//ģ��ж���������
  
  //---/led/step1/Makefile  
	KERNELDIR ?= /home/linux/store/linux-3.14-fs4412   //��Ӧ�ں�Դ��Ŀ¼(�����Լ����ں�Ŀ¼�����޸ģ���

	//---ʹ��
	$make                //��������ģ���ļ�fs4412_led_drv.ko ������֮ǰҪ��֤�ں��б���ɹ���
	$file fs4412_led_drv.ko       //�鿴һ�¸�ʽ����������� ���Ե�(Intel 80386)�� ���ǰ��ӵģ�ARM)
  #insmod fs4412_led_drv.ko     /*����ģ��                        
                       ���ܱ��� insmod: can't insert 'fs4412_led_drv.ko': invalid module format 
                       ԭ����Ǹ�ʽ���ԣ�file fs4412_led_drv.ko�ɲ鿴���������ǵ��Ե�hello.ko ȴ�ŵ����������С�
                       Ҳ�����Ǻ��ں˲�ƥ�� 
                       */
  #lsmod | grep fs4412_led_drv  /*����Ƿ��Ѽ���
  			lsmod ʵ���Ƕ�ȡ/proc/modules ��ʾ����        
           fs4412_led_drv 2046 0 - Live 0xbf004000 (O)
       */
  #rmmod fs4412_led_drv         /*ж��ģ��     
                        �ڰ����Ͽ��ܱ��� rmmod: chdir(/lib/modules): No such file or directory 
                                         rmmod: chdir(3.14.0): No such file or directory 
                        ���� /lib/modules/3.14.0 Ŀ¼ ����
                        
                       */
  #dmesg               //�鿴��ӡ����Ϣ
  
}	

}

{//==============================�ַ��豸����============================ ����֮��
{/*ʲô���ַ��豸
	 ���豸���ݴ��ݵ��ص㿴�� ����ǰ��ֽ���˳�򴫵ݹ����ģ�����������ˮ�ߣ��紮�ڣ����̣����ȡ�
 ���ǰ����ǳ���Ϊ�ַ��豸��
}

{//���ص�����
	�������ȥ������ʹ��һ���򵥵��ַ��豸
}

{//�����ʵ�ֺ�ʹ��һ���ַ��豸 
	{1. ע���豸�� */
	 	{//---led/step2/fs4412_led_drv.c
			#include <linux/kernel.h>
			#include <linux/module.h>
			#include <linux/fs.h>	  //for MKDEV register_chrdev_region
			
			MODULE_LICENSE("Dual BSD/GPL");
			
			#define LED_MA 500  //���豸�� �������ֲ�ͬ������豸  
			                    //ĳЩ���豸���Ѿ���̬�ط�����˴󲿷ֹ����豸����Documentation/devices.txt �� �������ǳ�ʹ��250
			#define LED_MI 0   //���豸�� ��������ͬһ���͵Ķ���豸
			#define LED_NUM 1  //�ж��ٸ��豸
			
			
			static int s5pv210_led_init(void)
			{
				dev_t devno = MKDEV(LED_MA, LED_MI); 
				int ret;
			
				ret = register_chrdev_region(devno, LED_NUM, "newled");   /*ע���ַ��豸��(��̬����)
					                                            Ϊһ���ַ�������ȡһ�������豸���
					                                            dev_id:       �������ʼ�豸���(������0��
					                                            DEVICE_NUM��  ����������豸��ŵ�����(����̫�󣬱��������豸�ų�ͻ)
					                                            DEVICE_NAME�� ��Ӧ�����ӵ������ŷ�Χ���豸������ 
					                                            alloc_chrdev_region  �ɽ��ж�̬����                                           
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
				unregister_chrdev_region(devno, LED_NUM);  //ȡ��ע��
				printk("Led exit\n");
			}
			
			module_init(s5pv210_led_init);
			module_exit(s5pv210_led_exit);
		}		 		
		//---ʹ��
		insmod fs4412_led_drv.ko
		cat /proc/devices //��newled ��ʾ��ע��ɹ�
	}
 /*  
	{2. ��ʼ���ַ��豸
		//---led/step3/fs4412_led_drv.c
			#include <linux/kernel.h>
			#include <linux/module.h>
			#include <linux/fs.h>
			#include <linux/cdev.h>  //�ַ��豸ͷ�ļ�
			
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
				
			struct file_operations s5pv210_led_fops = { //�ļ�����
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
			
				cdev_init(&cdev, &s5pv210_led_fops);//�ַ��豸��ʼ��
				ret = cdev_add(&cdev, devno, LED_NUM); //����ַ��豸��ϵͳ��
				if (ret < 0) {
					printk("cdev_add\n");
					goto err1;
				}
			
				printk("Led init\n");
			
				return 0;
				//---goto ������ ˳�����룬�����ͷţ�������Դ���ղ���ȫ�����ڴ�й¶��
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
			
				fd = open("/dev/led", O_RDWR); /* Ӧ��open����ε��õ�������open��
	                                                 open      ->      sys_open     ->         inode           ->  driver .open 
	                                                 Ӧ��       |      ϵͳ����     |         �ļ�ϵͳ         |   ����      
	                                        */
				if (fd < 0) {
					perror("open");
					exit(1);
				}
				printf("open led ok\n");  //ע��Ҫ��\n �����ӡ��Ϣ����û��
			
				return 0;
			}		  			
				
		 //---ʹ��
		  #mknod /dev/led c 500 0  //�����豸�ļ���Ӧ�ò��ܷ�����. ( ls -l /dev ���Կ����ܶ������豸�ļ�) 
      #./a.out  //���лῴ��  open led ok                       
	 			
	}
/*	 
	{3. ʵ����Ҫ���ļ����� //�ص� �ѵ�
		{//---led/step4/fs4412_led_drv.cc
			#include <linux/kernel.h>
			#include <linux/module.h>
			#include <linux/fs.h>
			#include <linux/cdev.h>
			
			#include <asm/io.h>
			#include <asm/uaccess.h> // for copy_to_user
			
			MODULE_LICENSE("Dual BSD/GPL");
			
			#define LED_MAGIC 'L'   //������0~0xff�������������ֲ�ͬ������, ��Documentation/ioctl/ioctl-number.txt
			
			#define LED_ON	_IOW(LED_MAGIC, 0, int)   //�ӻ�����ʽ�����������ֹ��ͬ�������������
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
				
				
			//ioctl ��������������				
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
				
		  // file_operations �� ����������ļ���һϵ�в�������   ����ÿ������ʵ��	
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
				gpx2con = ioremap(FS4412_GPX2CON, 4); /* ��̬ӳ�� �����ַ ���ں������ַ
																									phys_addr  ��ʼ�����ַ
																									size       ӳ�䷶Χ��С��
																									����ֵ     ӳ�����ں������ַ 
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
{//=======================linux�豸�����еĲ�������=======================
{//��ʲô�ǲ�����Ϊ��Ҫʹ�ò�������
	�������� �������ͬʱ��ִ��
	
  ֻ��ʹ��Щ�豸��������ִ�в�����������Ҫ��  
  
  ����
      ���ϵͳ�Ϲ�10���豸��ÿ���豸����������Ǵ���˳��ִ�У�
      ����һ���豸Ҫ�Ƚϳ�ʱ�䣬ϵͳ������Ӧ��    
}
	
{//�����ʵ�ֲ���
	CPU��˳��Ķ���ָ��ִ�еģ� �����ʵ�ֲ����أ�	
	����
	   ����A������ҵ��ʱ��BŮʿ�����绰�������绰�������A���¡�
	   ������л��㹻�죬����A,B������ʱ���ڡ� �о������ڲ���ִ��һ����
	   
	ʹ���жϵķ�ʽ��ʵ�ֲ���    //�жϣ� �����ڽӵ绰����ϵ�ǰִ�У��ȴ�����󣬷����ٽ���ִ�С�
	     
}

{//��ΪʲôҪ���в�������
	�����ᵼ�¾�̬�ķ����� //��̬�� һ����Դ������ִ�е�����ͬʱ����ʱ���ͻ���־�������ʱ��״̬���Ǿ�̬��
	�豣֤һ����Դ��һ��ʱ��ֻ�ܱ�һ��������ʣ����ܱ�����ҡ�����������	
}	

{//����ν��в�������
 ���û�����ƶԲ������п���  //���⣺ ������Դ��ĳ�������ʱ����������ֹ���ʡ�
	                                                                                             
 ������ƣ�
	 {//�ж�����     
	 	 ʹ�ý�ֹ�жϷ�ʽ�������жϵ�Ӱ�� 
	*/ 	 
	 	 local_irq_disable       //�����ж�
	// 	 �ٽ�������              //�ٽ����� ���ʹ�����Դ�Ĵ�������
	 	 local_irq_enable        //���ж�
/*	 	 
	 	 ע�� 
	 	    1�������жϵ�ʱ�価���̣��������ɱ���ж���ʱ���ã�������ʧ,����ܲ�������ָ���жϵķ�ʽ
	 	 		  disable_irq(int irq); //����ָ���жϺŵ��ж�
			    enable_irq(int irq);  //ʹ��ĳ�жϺŵ��ж�   
			  2�����豣�浱ǰ���ж�״̬�����ڻָ�   
			    ��local_irq_save(flags);  ����local_irq_disable
			    ��local_irq_restore(flags) ���� local_irq_enable			  
   }	      
	 {//ԭ�Ӳ���
	 	 �����в�����ϵ���С��Ԫ
	 	
	  {//---hello_atomic/hello.c         ʵ���豸ֻ��һ�����̴�
	  	#include <asm/atomic.h>
	  	
		  static atomic_t hello_atomic = ATOMIC_INIT(1);    //����ԭ�ӱ���hello_atomic ,����ʼ��Ϊ1 
	    static int hello_open(struct inode *inode,struct file *file)
	    {
	      if (!atomic_dec_and_test(&hello_atomic))  {  //  atomic_dec_and_test��ʾԭ�ӱ����Լ�һ���������Ƿ�Ϊ����,���Ϊ�㷵����
	      	 //����open������δreleaseʱ���ٴ�open , hello_atomicΪ0, �ټ�һ��Ϊ�㣬!atomic_dec_and_test ����  
	         atomic_inc(&hello_atomic);       //ԭ�ӱ�����һ���ָ��Լ�ǰ״̬
	         return  - EBUSY;   //�Ѿ�����
	      }
	      //����һ�α�openʱ�� hello_atomicΪ1  , !atomic_dec_and_test �������� ������
	      printk("hello open \n");
	      return 0;
	    } 
	    static int hello_release(struct inode *inode,struct file *file)
	    { 
	      atomic_inc(&hello_atomic);   //�ͷ��豸    
	      printk("hello release\n");
	      return 0;
	    }
    }	
	 	
   }   
	 {//������	spinlock  (cpu���� ����whileѭ����
	 	 cpu������ѭ����תCPU �ȴ��ͷ���,  ����Դ�� ������������ʱ��С��˯�߻���ʱ�䳡��	
	 	 ���̵ݹ�����ʱ���ᵼ������
	 	 	
	 	 {//---hello_spinlock/hello.c
 			static DEFINE_SPINLOCK(hello_spinlock); 
 			static int hello_resource = 1;
 			
 			static int hello_open(struct inode *inode,struct file *file)
 			{
 			  spin_lock(&hello_spinlock);  //���ɺ��ж����κ��� spin_lock_irq    
 			  if(hello_resource == 0)        //Ϊ���������� spin_try_lock
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
	 {//�ź���   
 	  ���̵������ڲ�������ͣ���Դ�ļ�����
 	  ��������������ź�����˯�ߣ��������ڽ����� 
    //---hello_semaphore/hello.c
  	#include <linux/semaphore.h>
    static DEFINE_SEMAPHORE(hello_semlock);   //����һ����ʼֵΪһ���ź���
       
    static int hello_open(struct inode *inode,struct file *file)
    {            
      if (down(&hello_semlock))  /* ��ô���*/
      {
         return  - EBUSY;  /* �豸æ*/
      }
      
      printk("hello open \n");
      return 0;
    }
    static int hello_release(struct inode *inode,struct file *file)
    {
      up(&hello_semlock);  /* �ͷŴ���*/
      printk("hello release\n");
      return 0;
    }	  
   }    
 	 {//������  �õ����
  //	 ��������ר������������ģ� �Ͷ�Ԫ���ź������ƣ� 
  	
		struct mutex my_mutex;  /* ����mutex */
		
		static struct mutex lock;
		mutex_init(&my_mutex);  /* ��ʼ��mutex */
		
		mutex_lock(&my_mutex);  /* ��ȡmutex */
		.../* �ٽ���Դ*/
		mutex_unlock(&my_mutex); /* �ͷ�mutex */
   }	
   
  
}

{/*��������ʹ�ó���
	����������    ����    �ж�    CPU
	
	1. �ж����ε�ʹ�ó���  //���� �жϺ��жϼ䡣���� �� �ж� ����ʱ
 	   �����жϴ��������ʹ�����Դ��ʱ��
	2. ԭ�Ӳ�����ʹ�ó���  //���� ���̼� �жϼ� �ۺϾ���ʱ  ���ź��� �ж����εȶ��ǻ���ԭ�Ӳ�����ʵ�ֵģ�
	   ֻʹ���ڹ�����ԴΪһ�������Ĳ��������
	3. ��������ʹ�ó���   //���� ��CPU ����ʱ�� ���������ã�����CPU�����ķ��գ�
	   ��CPU������� 
	   ���ٽ�����������ʱ��Ƚ϶̵������
	4. �ź����������ʹ�ó���   //���� ���̼侺��ʱ  �������ܷ�ֹ�жϵĴ�ϣ���Ҫ��ֹ�жϴ�ϣ������ж����λ��ʹ�ã�
	   �ٽ�����������ʱ��Ƚϳ��������  
	   �������е�ʱ��ϳ���ʱ������ʹ���ź�����
	   1ע�⣺ �ж��ﲻ��ʹ���ź����� ��Ϊ�жϲ���˯�ߡ�
}	
			
}

{//====================Linux�豸�����е������������I/O===================
{//�������������ͷ�����,�첽֪ͨ��ʲô
	
	  ----------
	  |        |                  --------
	  |  ����  |  <-------------->| �豸 |
	  |        |                  --------
	  ----------
	
   ������     ���̵ȴ��豸��Դ���ã� �ȴ������У� �������ߣ�CPU�л�ȥִ�б�Ľ��̣��� 
   ��������   ���ȴ�����ѯ�豸 
   �첽֪ͨ�� ���豸����ȥ֪ͨ���̡�
   
   
   ע�� ������������������첽֪ͨ���������Ϊ�˽�� ���̺��豸������δ򽻵���
}
   
{//�������������ʵ������I/O
  ���ж����������˯��ʱ�������Ŷӵķ�ʽ�������������������ʧ��������Դ�ͷ���)��ȥ�����������ŶӵĽ���.  
  
  {//��ʲô�ǵȴ�����
    �Զ���Ϊ�������ݽṹ������̵��Ȼ��ƽ��ܽ�ϣ��ܹ�����ʵ���ں��е��첽�¼�֪ͨ���ƣ�ͬ����ϵͳ��Դ�ķ��ʡ�
    
    �磺
  	   ��ĳ���̲�����ĳ����ʱ(����Դ), �ɰѽ�����ʱ˯�ߣ���������У�
  	   ����������ʱ(��ĳ��Դ���ͷų�����)���ٰѽ��̻���ȥִ��
  }
  
  {//�����ʹ�õȴ����У���������ʵ������
		#cd  /root/driver_example/waitqueue
		#make clean
		#make 
		#insmod hello.ko   //���ﲻ���� mknod /dev/hello c 250 0 ���豸�ڵ��� device_create���Զ�����
		#cat /dev/hello  &    // ����& ��ʾ�ý��̺�ִ̨��   
		����һ�ն�
		#echo hello > /dev/hello
		���Է�����ǰ���ն˻��ӡ��hello
		#rmmod hello
			
	  {//---driver_example/waitqueue/hello.c
      #include <linux/sched.h>
     static wait_queue_head_t queue;   //����ȴ�����
     static char *gbuf=NULL;
     static int glen=0;
     static ssize_t hello_read (struct file *filp, char __user *buf, size_t count,
                     loff_t *f_pos)
     {
     	
     	  /*
     	    wait_event_interruptible  �ڲ���һforѭ����
     	    1. ����������ʱ�������ź��账��ʱ�� ����ѭ��������0

     	    2. ������ź�δ�������ط���ֵ
     	       return -ERESTARTSYS��  
     	       ϵͳ���ÿ���ERESTARTSYS�����ڴ������źź��ֻ��ٴδ���ǰ�εĵ��ã����ֵ�����һ��hello_read��
   	    
     	    3. ������������ʱ�������ź��账����˯�ߵȴ�    	    
     	       	        
     	  */
        if (wait_event_interruptible(queue, glen!=0))   //�ѵ��øú����Ľ��̣�cat /dev/hello��, ����ȴ�����queue�У���
        {
           return -ERESTARTSYS;  //��ʾ�źź���������Ϻ�����ִ���źź���ǰ��ĳ��ϵͳ����.
                                 //���ȴ������ڵȴ��У�������źţ���ִ�����źź���ִ��һ��ϵͳ���ã������ȴ�
        }
                
        glen =0;
        ...
     }
    
     static ssize_t hello_write(struct file *filp, const char __user *buf, size_t count,
                     loff_t *f_pos)
     {
     	  glen = count;
        wake_up_interruptible(&queue);  //������queue�ȴ������ϵȴ������н��� , ����ָ cat /dev/hello ����
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

{//�������������ʵ������I/O
	 Ӧ�ã�
	    ��select/poll       // I/O��·���� 
	 ������
	    ʵ��.poll ������ 


  ---------------------------
  | Ӧ��                    
  |         select/poll    
  ---------------------------
               |
  ---------------------------
  |�ں�     sys_poll       //pollϵͳ����,��ѯpoll_table�е�����.poll�� ���豸�ɶ����д�򷵻�
  |            |            
  |          .poll             //����ʵ�ֵ�.poll
  ---------------------------

   /*ԭ��
       select/poll �����  sys_poll(ϵͳ����)  ���ٵ��������е�.poll
 
    sys_poll����ѭ������ poll_table�ϵ��豸�Ƿ��������������ĵȴ�����queue����������������򷵻�
    �粻�������� schedule (�ö��ں�ȥִ�б�Ľ���)�� ���غ������⡣
    ��������������ᷢ����sys_poll�У�
    
   */


	{//------ʵ�� poll------------
		#cd  /root/driver_example/poll
		#make clean
		#make 
		#insmod hello.ko
		#mknod /dev/hello c 250 0
		#arm-none-linux-gnueabi-gcc test.c
		#./a.out &  //��̨����    �ȴ�����������
		����һ�ն�
		#echo hello > /dev/hello //д�����ݣ� ԭ�ն���ʾPoll monitor:can be read
		#cat /dev/hello //�������ݣ�ԭ�ն˵ȴ�����������
	}	
	
	{//---poll/test.c    //���ܣ�  ��ѯ�豸�����豸�Ƿ�ɶ�д
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

		     			
				select(fd + 1, &rfds , &wfds, NULL, NULL);  //  I/O��·����  ���յ���������.poll ����
			
			  //���ݿɻ��
				if (FD_ISSET(fd, &rfds))
				{
			     printf("Poll monitor:can be read\n");	  			
			     read();    
				}
			
			  //���ݿ�д��
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

       poll_wait(filp, &queue, wait);// �ѵ�ǰ������ӵ�wait��poll_table �豸����б��У���������Ӧ�ĵȴ�����queue
                                     //���ж��������ӵ�poll_table����ʾ����ͬʱ��ض�������豸�ı仯 
                                     //poll_wait�������������� ������������������pollϵͳ���������  
       if (glen != 0)
       {
          mask |= POLLIN | POLLRDNORM; /*��ʾ���ݿɻ��*/
       }       
			 if (glen < GBUF_MAX)
			 {
			    mask |= POLLOUT | POLLWRNORM; /*��ʾ���ݿ�д��*/
			 }                           
      return mask;      //ͨ���������ر�־
   } 		

		static struct file_operations hello_fops = {
		  .poll = hello_poll,
		};

	}
	
}

{//�������������ʵ���첽֪ͨ
	#cd  /root/driver_example/async
	#make clean
	#make 
	#insmod hello.ko
	#mknod /dev/hello c 250 0
	#arm-none-linux-gnueabi-gcc test.c
	#./a.out &   //�ȴ���  ��̨����
	����һ�ն�
	#echo hello > /dev/hello //д���ݣ� �����첽�ź�
	                          //ԭ�ն˻���ʾreceive a signal from globalfifo,signalnum:29
	
	{//--/async/test.c
   #include <sys/types.h>
   #include <sys/stat.h>
   #include <stdio.h>
   #include <fcntl.h>
   #include <unistd.h>
   #include <signal.h>
   
   void input_handler(int signum) //���յ��첽���źź�Ķ���
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
   
     //�����ź���������
     signal(SIGIO, input_handler); //��input_handler()����SIGIO�ź�
     fcntl(fd, F_SETOWN, getpid());  //�����豸�ļ���������Ϊ������
     oflags = fcntl(fd, F_GETFL);    // ����� �����е� .fasync
     fcntl(fd, F_SETFL, oflags | FASYNC);  //FASYNC  ����֧���첽֪ͨģʽ 
           
      while (1)
      {
         sleep(100);
      }  
   }
	}
	{//--/async/hello.c
    struct fasync_struct *async_queue; /* �첽�ṹ��ָ�룬���ڶ� */
    
    static ssize_t hello_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos)
    {
       kill_fasync(&async_queue, SIGIO, POLL_IN);  //����SIGIO �첽֪ͨ�ź�
       return count;
    }
    
    static int hello_fasync(int fd, struct file *filp, int mode)
    {
       return fasync_helper(fd, filp, mode, &async_queue);  //�����־�ı��
    }
    
    static struct file_operations hello_fops = {
      .owner = THIS_MODULE,
      .write = hello_write,
      .fasync = hello_fasync,
    };
	}
}

}

{//========================ͳһ�豸ģ�� ֮ ƽ̨�豸======================= �豸������Ӳ����Ϣ�������з��������
{//��linux�豸ģ�� ��ʲô
	Ϊ�˹�����ָ��Ӷ������豸�� ������Ŀ��ģ�ͣ����ǲ������������ķ�ʽ�����з���ֲ㡣
	
   //---�������
   Kobject //��kobject���ɶ���Ļ���( ���������Ϊģ���е���ԭʼ������Ԫ�������ṹ����������)     
   
   device  //�������������
   driver  //���ƶ������Ϊ
   bus     //��device driver����������ͨѶ������   
}

{//��ΪʲôҪ�� linux�豸ģ��
	ʹ���豸ģ����Ϊ�˸��õĹ����豸�� 
	���豸���෱�࣬��ôȥ�����أ�
	����֪����������������Ҳ������Ч��ʽ�����ǰ����Ǽ�Ĺ�ϵ��������������з���ֲ㡣
	linux�ں��Ѿ����Ұ������ѵ�һ�����ˣ� ������豸ģ�� 
}

{//�����ʵ�� linuxͳһ�豸ģ��
  //---�豸���ι�ϵֱ�۷�ӳ���� sysfs ( sysfs�ļ�ϵͳ�������ڴ��У���һ�������ļ�ϵͳ�����ṩ��kobject�����ε���ͼ)
   /sys
     |--block   //�������еĿ��豸�����뵽blockĿ¼�£��ᷢ������ȫ��link�ļ���link��sys/device/Ŀ¼�µ�һЩ�豸
     |--device  //����ϵͳ���е��豸���������豸�ҽӵ�����������֯�ɲ�νṹ
     |--bus     //����ϵͳ�����е���������
     |--class   //ϵͳ�е��豸����
     |--module  
     |--dev
         |--block
         |--char  	
	
	
  {//---�豸�̳п�ͼ
  	
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
	
	
	
	//�豸ģ���еĻ���
	struct kobject 
		
	//�豸����
	struct device {                      
	  struct kobject kobj;         																																				
	  struct bus_type * bus;       																																				     
	  struct device_driver *driver;	
	  void    *driver_data;  																																			
	};


	//��������
	struct device_driver {
	  const char              *name;
	  struct bus_type         *bus;
	  int (*probe) (struct device *dev);
	  int (*remove) (struct device *dev); 
	};
	
	//��������
	struct bus_type {
	     const char         * name;/*������������*/
	     int     (*match)(struct device * dev, struct device_driver * drv); //�������Զ�ͨ������ȥƥ���豸device ������driver
	     int     (*uevent)(struct device *dev, char **envp,
	     int     (*probe)(struct device * dev);
	     int     (*remove)(struct device * dev);
	};
		                    
}

{//ƽ̨�豸 platform_device
      socϵͳ�м���������豸��������ʵ�ʹҽӵ�ĳ�������ϣ���I2C,USB�ȡ� ����linux�����platform���ߣ�
   ͨ��������ƥ���豸�������� ������socϵͳ�е��豸����platform�豸��Ϊ����ͳһ�������������
   ��Դ�ĸ��ʹ��ʱ����get_resource.
   
   �ŵ㣺
     1������Ӳ����Ϣ�������� ��������صĴ�����������롣               ----��     �޸� ��չ 
        ���Ӳ���иĶ�����ֲ�� �����޸�һ��platform�й�����Դ�������Ϳ����ˡ�
     2���� ���� ���� �豸 ��ͳһ�豸ģ��, �������   
	
	struct resource {
		resource_size_t start;//��Դ��ʼλ��
		resource_size_t end;  //��Դ����λ��
		const char *name;     
		unsigned long flags;  //��Դ������  IORESOURCE_MEM (�ڴ��ַ)     IORESOURCE_IRQ (�жϣ�
	};	
	
	struct platform_device {
        const char      * name; //ƽ̨�豸������
        u32             id;
        struct device   dev; //��Ƕ��device  ʵ�ּ̳�
        u32             num_resources;
        struct resource * resource;  //ƽ̨�豸����Դ
  };
	struct platform_driver {
	        int (*probe)(struct platform_device *);
	        int (*remove)(struct platform_device *);
	        struct device_driver driver; //��Ƕ��device_driver ʵ�ּ̳�
	};	
	
	{//----arch/arm/mach-smdkc100.c         �ϵ�platform_device��ʽ -> �弶�����ļ�
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
	
	{//----arch/arm/boot/dts/exynos4412-fs4412.dts   �µ�platform_device��ʽ -> �豸��
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

	{//----dm9000.c    platform_driver	 (ͨ�� .compatible = "davicom,dm9000" �ҵ�platform_device)
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
	
	{//? ���豸����ʽʵ��LED�ƿ���  
			1��	�����ں�Դ���޸� arch/arm/boot/dts/exynos4412-fs4412.dts�ļ�,����������ݣ�
				fs4412-led {
					compatible = "fs4412,led"; 
					reg = <0x11000c40 1>,<0x11000c44 1>;
				};
			2���������� ���ַ��豸cdev �������� ���ƽ̨�豸����У���������Դ��ȡ��ʽΪIORESOURCE_MEM
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

{//============================Linux�жϻ���============================= 
{//��Ϊ��Ҫ�õ��жϻ���  
   ���û���жϣ�CPU��ֻ������ѯ�ķ�ʽ������������CPU����������ѯ��ʱ������
 ���ж����ϴ��CPU��ǰ���е������Ƚ����жϴ�����ٷ���ԭ��������ִ�С���Ӧ�졣
}

{//��linux���жϻ��������ʵ�ֵģ����ͱ��ϵͳ���жϻ����к�����
	 ��ͬ�ģ� �ж��������л����籣���ֳ����ж���������ת�������жϴ�����򣬻ָ��ֳ��ȡ�
	 linux ���еģ� �ϰ벿���°벿
	 
	 �жϺ�                                 
	    |   |   |
	  ---------------
	  |  �жϿ����� |
	  ---------------
	         |
       ---------
	     |�ϰ벿 |                          //�磺 �� request_irq����� �жϴ������
	     ---------   
	         |  
	       -----
	       |   | -- HI_SOFTIRQ
	       |�� | -- TIMER_SOFTIRQ
	       |   | -- NET_TX_SOFTIRQ
	       |�� | -- NET_RX_SOFTIRQ
	       |   |
	       |�� | -- TASKLET_SOFTIRQ       //�����ж��Ƴ�ִ�У��������жϵ������ģ�����˯��
	       |   |
	       |---|
	         |
	   -----------------      
  	 |                |  --   task   (ʱ��Ƭ��������  ��ӦӦ���еĽ���)
	   |   �ں� ����    |  --   workqueue   //�������У�������������Ƴ�ִ�У������ڽ��������ģ� ��˯��
	   |                |  --   ksoftirq   
	   -----------------      


   {//--�ж�ʹ��
			   disable_irq(int irq); //����ָ���жϺŵ��ж�
			   enable_irq(int irq);  //ʹ��ĳ�жϺŵ��ж�
			   
				request_irq(unsigned int irq,   //Ӳ���жϺ�
				            irq_handler_t handler,  //�жϴ�����
				            unsigned long flags,  //�жϱ�־
	    							const char *name,  //�ж����� 
	    							void *dev)			   
			   			   
			   void free_irq(unsigned int irq,void *dev_id);  //�ͷ�ָ�����жϣ� irq��ʾ�жϺ�  dev_id�жϹ���ʱ�ã�һ��ΪNULL
			      
			   //�ж��к�ʱ���ֵ��Ƴٴ��� (��ѡ)
		      �Ƴٴ�������һ����tasklet  ��workqueue
		      DECLARE_TASKLET (xxx_tasklet, xxx_do_tasklet, 0) ע��tasklet
		      tasklet_schedule(&xxx_tasklet)  ����tasklet		 
		      
		      static void xxx_do_tasklet(unsigned long arg)       
	 }

}

{//�����ʹ���ж�
	1��	�����ں�Դ���޸� arch/arm/boot/dts/exynos4412-fs4412.dts�ļ�,����������ݣ�
		fs4412-key {
			compatible = "fs4412,key";
			interrupt-parent = <&gpx1>;  /*��Ӧarch\arm\boot\dts\exynos4x12-pinctrl.dtsi �е�
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
			interrupts = <1 2>;<2,2>  /* 1 ��Ӧ gpx1 �ڶ����ж� <0 25 0> ��  2 ��Ӧ������ʽ�� ��� Documentation/devicetree/bindings/arm/gic.txt  2 = high-to-low edge triggered��
			         25��������·ͼ��K2->UART_RING->XEINT9��-��оƬ�ֲ�(9.2 Interrupt Source��   25  57  �C EINT[9] External Interrupt��			                             
			                      */   
		};
	2��	���±���dts�ļ���������/tftpbootĿ¼��
	$ make dtbs
	$ cp arch/arm/boot/dts/exynos4412-fs4412.dtb /tftpboot
	3��	����ģ��Դ��Ŀ¼
	��ʵ������е�fs4412_keyĿ¼������/home/linuxĿ¼�²������Ŀ¼��
	4��	����ģ��
	$ make
	5��	��ko�ļ��Ͳ��Գ��򿽱������ļ�ϵͳ��
	$ cp *.ko  /source/rootfs 
	6��	�����ϲ���ģ��
	# sudo  insmod  fs4412_key.ko
	7��	���Թ���
	��key2�鿴����
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
		
			irqflags = IRQF_DISABLED | (key1_res->flags & IRQF_TRIGGER_MASK);  // key1_res->flags ʱ�жϴ�����ʽ ��Ӧexynos4412-fs4412.dts��fs4412-key  interrupts = <1 2>�е�2   
				
			ret = request_irq(key1_res->start,  //Ӳ���жϺ�  ��Ӧexynos4412-fs4412.dts��fs4412-key  interrupts = <1 2>�е�1  
			                  key_interrupt,  //�жϴ�����
			                  irqflags,  //�жϱ�־   IRQF_DISABLED �ڱ�ʾ�жϴ���ʱ���ε����������ж� 
			                  "key",  //�ж�����  ��cat /proc/interrupts  �˿���
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

{//=============================����С֪ʶ��=============================
	
{//------------------------------ʱ�����--------------------------- jiffies ,������ʱ ����ʱ��
jiffies:
	  ȫ�ֱ���jiffies������¼��ϵͳ�������������Ľ��ĵ�����      // Linux����ÿ���̶����ڻᷢ��һ��timer interrupt (IRQ 0)��Jiffies����Ӧ�ļ�һ
	  ����ʱ���ں˽��ñ�����ʼ��Ϊ0���˺�ÿ��ʱ���жϴ�����򶼻����Ӹñ�����ֵ��
	  һ����ʱ���жϵĴ�������Hz������jiffiesһ�������ӵ�ֵҲ����HZ
	  
����ʱ��  //�ӽ������һ��jiffies
    udelay  mdelay ����ѭ��æ�ȴ�������CPUʱ�䣬 ���������1��jiffies = 4ms ���ԣ�ʹ��ʱ��ʱС��4ms�Ŀ������ǡ�
    
����ʱ��        
    �ò�ѯjiffies�ķ���   �� time_after
    �ں˵��ȳ�ʱ           schedule_timeout(signed long timeout)     
    ���ڵȴ����е�˯�߳�ʱ sleep_on_timeout(wait_queue_head_t *q, long timeout)   //���Ҳ�ǵ����ں�schedule
     

{//-----��ʱ������
		#cd time
		#make
		#insmod hello.ko
		#gcc test.c
		#./a.out 
		   ������ʾ
		   seconds after open /dev/hello:1
	     seconds after open /dev/hello:2
	     seconds after open /dev/hello:3
     	
	{//---time/hello.c
	  atomic_t sec_count;
	  struct timer_list s_timer; /*�豸Ҫʹ�õĶ�ʱ��*/
	  
	  /*��ʱ��������*/
	  static void second_timer_handle(unsigned long arg)
	  {
	    mod_timer(&s_timer,jiffies + HZ); //modify a timer's timeout
	    atomic_inc(&sec_count);
	    printk("current jiffies is %ld\n", jiffies);
	  }
	  
	  static int hello_open (struct inode *inode, struct file *file)
	  {
	    /*��ʼ����ʱ��*/
	    init_timer(&s_timer);
	    s_timer.function = &second_timer_handle;
	    s_timer.expires = jiffies + HZ;   //��ʱֵ��һ��jiffiesֵ����jiffiesֵ���ڳ�ʱֵtimer->expiresʱ��timer->function�����ͻᱻ����
	  
	    add_timer(&s_timer); /*��ӣ�ע�ᣩ��ʱ��*/
	    atomic_set(&sec_count,0);//������0
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

{//------------------------linux�ں��ڴ��ʹ��----------------------
	
	Ӧ��	
		 ------------------------------------------
		 |          malloc       mmap             |    
		 ------------------------------------------	 	
		                    |
		 ------------------------------------------
		 |        open  read  write  ioctl        |    
		 ------------------------------------------	 	
		                    |
	=================================================
	�ں�                  |	
		 ------------------------------------------
		 |          ϵͳ����  system call         | 
		 ------------------------------------------
		                    |  
		 ------------------------------------------
		 |             �ļ�ϵͳ   fs              | 
		 ------------------------------------------
		                    |
	   |---------------------------   ----------- //vmalloc  �����ϲ�����  ��˯�ߣ� Ч�ʵͣ�һ�㲻�����ڷ������ڴ�ʱ��ʹ��        
	   | kmalloc   get_free_pages |	  |         | //kmalloc(<128k) ����kmem_cache  ��������  ����GFP_KERNEL�� �ж�ʱҪ��GFP_ATOMIC������˯�ߣ�  �ʺϷ���С��128K��   
		 ----------------------------   | vmalloc | //get_free_pages(n)  < 4M                                                                                                 
		 |       kmalloc_cache      |   |         | //kmem_cache_alloc  ����slab  �ʺϷ�����ͷ����ݽṹ�� ��slab�Ƿ�������ͬһ��С�ģ��ᱣ��ԭ�������ݣ������س�ʼ����                                      
		 ------------------------------------------
		                    |
		 ------------------------------------------
		 |                 MMU                    |
		 ------------------------------------------
		 |         �豸���� Buffer Cache          |
		 ------------------------------------------
		                    |
		 ------------------------------------------
		 |   ioremap(��̬ӳ��)   map_dec(��̬)    |     - gpio_led 0xc000050
		 ------------------------------------------	     
	=================================================	  
	Ӳ��                  |                     
		 ------------------------------------------
		 |                  �豸                  |    - 0xe00001c0
		 ------------------------------------------ 		

}

{//-------------------------io�ڴ�ռ� ��io�˿ڿռ�-----------------
	
 //---I/O �ڴ�ռ�:  I/O ���ڴ�ͳһ��ַ  ��arm powerpc��
   ---------------------
   | request_mem_region |     //�����ڴ����� �Ǳ���ģ���ʹ�����ϰ�ȫ��ϵͳ���Զ����л��⴦��
   ----------------------   
   |      ioremap       |     //��̬ӳ��Ӳ�������ַ���ں˵�ַ�ռ�     
   ----------------------
   | ioread32 iowrite32 |     //I/O �ڴ��Ӧ�Ķ�д���� 
   | readl     writel   |
   ----------------------
   |      iounmap       |     //���ioremap�Ķ�̬ӳ��
   ----------------------  
   | release_mem_region |     //�ͷ�������ڴ�����
   ----------------------

 //---I/O �˿ڿռ�:  I/O ���ڴ������ַ  �� x86
   ---------------------
   |   request_region   |     
   ----------------------     
   |     inb   outb     |  
   ----------------------
   |   release_region   |
   ----------------------	
}	
	
{//--------------------��/dev ���Զ������豸�ڵ�--------------------(�����豸�ļ������ݱ�   mkdnod  -- > devfs(��̬ �ں�)  --> udev(��ʵ �û�) ) 
		//��ʲô��devfs,���к���ȱ��
		  ����Linux2.4 ����ģ���ʹ���豸�����ܹ����Լ����紴��ɾ��/dev���޸�Ȩ�޵ȡ� 
		   ��:
		    1. �����ֶ�mknod ������devfs_mk_cdev ���Զ�����
		    2. ����Ҫָ�������豸�ţ���0�����豸�ţ����ᶯ̬��ȡ�� 
		   ȱ:
		   	1. ��ȷ�����豸ӳ��,
		   	2. /dev���ļ�̫�࣬�Ҳ��Ǳ�ʾ��ǰϵͳʵ�ʵ��豸��
		   
		//��ʲô��udev,���к���ȱ��  
		  ����linux2.6 ������������devfs��    
		   ��:
		   	1. �ȶ����豸ӳ��
		    2. /dev��ֻ����ϵͳ��ʵ���ڵ��豸
		    3. �����ֶ�mknod����device_create���Զ�����
				 
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

{//-----------------------------mapӳ�����-------------------------
//---��̬ӳ��
  map_dec      //��̬ӳ�� Ӳ�������ַ ���ں������ַ�ռ�     �ں�����ʱ���Զ���ɸ�ӳ�� 
  
//---��̬ӳ��	
	void *ioremap(phys_addr_t addr, unsigned long size)  /* ��̬ӳ�� �����ַ ���ں˵�ַ
																									       phys_addr  ��ʼ�����ַ
																									       size       ӳ�䷶Χ��С��
																									       ����ֵ     ӳ���������ַ 
																									      */	
{//---mmap �ļ�ӳ�䵽�ڴ�  
	//��mmap��ʲô
	{
	   mmap�ǰ��ļ�����ӳ�䵽���̵������ڴ�ռ�, ͨ��������ڴ�Ķ�ȡ���޸ģ���ʵ�ֶ��ļ��Ķ�ȡ���޸�,������Ҫ�ٵ���read��write�Ȳ�����
	
	   void* mmap (  void * addr ,   //ָ��ӳ�����ʼ��ַ, ͨ����ΪNULL, ��ϵͳָ�� 
	                 size_t len ,    //ӳ�䵽�ڴ���ļ�����
	                 int prot ,      //ӳ�����ı�����ʽ, PROT_EXEC: ӳ�����ɱ�ִ�� PROT_READ: �ɶ�  PROT_WRITE: ��д
	                 int flags ,     //MAP_SHARED:д��ӳ���������ݻḴ�ƻ��ļ�, ����������ӳ����ļ��Ľ��̹���
	                                 //MAP_PRIVATE:��ӳ������д����������һ��ӳ�����ĸ���(copy-on-write), �Դ������������޸Ĳ���д��ԭ�ļ���
	                 int fd ,        //��open���ص��ļ�������, ����Ҫӳ����ļ�
	                 off_t offset    //���ļ���ʼ����ƫ����, �����Ƿ�ҳ��С��������, ͨ��Ϊ0, ��ʾ���ļ�ͷ��ʼӳ��
	               )   
	}
	
	{//��Ϊʲô��mmap
		1. ��mmap����ļ�����(read,write)�����豸��Ч�ʸ�.   �����û��ռ�����ں˿ռ��һ�ָ�Ч�ʷ�ʽ��
	      mmap        --   ӳ���ļ��������ڴ� 
	      read,write  --   copy_to_user  ����
	      �Ա�read write��mmap����ÿ�ζ�copy_to_user����һ��. ������һ��ӳ�䣬�Ժ�ֱ�Ӳ����ڴ�buf,Ч�ʸߡ�
		2. �ù����ڴ淽ʽ��������̼�ͨѶ����Ч�ʸߡ� 
	       ���ù����ڴ�ͨ�ŵ�һ���Զ��� ���ĺô���Ч�ʸߣ���Ϊ���̿���ֱ�Ӷ�д�ڴ棬������Ҫ�κ����ݵĿ�����
	     ������ܵ�����Ϣ���е�ͨ�ŷ�ʽ������Ҫ���ں˺��û��ռ�����Ĵε����ݿ�����
	     �� �����ڴ���ֻ������������[1]��һ�δ������ļ��������ڴ�������һ�δӹ����ڴ���������ļ�		      
	}
	
	{//��mmap ��ô��
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
		 
		 /*���ļ�*/
		 fd = open("/dev/memdev0",O_RDWR);     
		 buf = (char *)malloc(100);
		 memset(buf, 0, 100);
		 start=mmap(NULL,100,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
		 
		 /* �������� */
		 strcpy(buf,start);
		 sleep (1);
		 printf("buf 1 = %s\n",buf); 
		 
		
		 /* д������ */
		 strcpy(start,"Buf Is Not Null!");
		 memset(buf, 0, 100);
		 strcpy(buf,start);
		 sleep (1);
		 printf("buf 2 = %s\n",buf);
	
		 munmap(start,100); /*���ӳ��*/
		 free(buf);
		 close(fd); 
		 return 0; 
		}	
	}
}

}

}

{//===============================��������=============================== (�����쿼��)
//Ŀ�ģ� �����ַ��豸��� �� ƽ̨�豸��ܣ��豸����  �������豸

{//����ν���linux�ں���������
	1. ����·ͼ оƬ�ֲ�  //Ū���CPU��ͨ����Щ�Ĵ����������豸�ġ�   
	2. ����linux�������  //����ƽ̨�豸��� �����泣Ƕ��������Linux�豸���ͣ����ַ��豸��
	3. ��д���Ƴ���͵��� //��ʵ��read write ioctrl�ȹ���
}

{//led�ƿ��� 
ʵ���ʮһ LED��������д������
��ʵ�����ݡ�
��дһ���ַ������������Ŀ����LED������
��ʵ��Ŀ�ġ�
����GPIO�Ĳ���
��ʵ��ƽ̨��
	������Ubuntu 12.04
	Ŀ�����FS4412
	Ŀ����ں˰汾��3.14.0
	����������汾��arm-none-linux-gnueabi-gcc-4.6.4
��ʵ�鲽�衿
1��	����ģ��Դ��Ŀ¼
��ʵ������е�fs4412_ledĿ¼������/home/linuxĿ¼�²������Ŀ¼��
2��	����ģ��
$ make
3��	����Ӧ�ó���
$ arm-none-linux-gnueabi-gcc  test.c  -o  test
4��	��ko�ļ��Ͳ��Գ��򿽱������ļ�ϵͳ��
$ cp *.ko test /source/rootfs 
5��	�����ϲ���ģ��
# sudo  insmod  fs4412_led.ko
6��	�����豸�ڵ�
# mknod  /dev/led  c  500  0
7��	���Թ���
# ./test
�鿴�Ƶ�״̬
	
}	

{//key���̿��� 
ʵ���ʮ�� ����������д
��ʵ�����ݡ�
��дһ���ַ���������׽���̵Ķ���
��ʵ��Ŀ�ġ�
����GPIO�Ĳ�����linux���жϵ�ʹ��
��ʵ��ƽ̨��
	������Ubuntu 12.04
	Ŀ�����FS4412
	Ŀ����ں˰汾��3.14.0
	����������汾��arm-none-linux-gnueabi-gcc-4.6.4
��ʵ�鲽�衿
1��	�����ں�Դ���޸� arm/arm/boot/dts/exynos4412-fs4412.dts�ļ�,����������ݣ�
	fs4412-key {
		compatible = "fs4412,key";
		interrupt-parent = <&gpx1>;
		interrupts = <1 2>, <2 2>;
	};
2��	���±���dts�ļ���������/tftpbootĿ¼��
$ make dtbs
$ cp arch/arm/boot/dts/exynos4412-fs4412.dtb /tftpboot
3��	����ģ��Դ��Ŀ¼
��ʵ������е�fs4412_keyĿ¼������/home/linuxĿ¼�²������Ŀ¼��
4��	����ģ��
$ make
5��	��ko�ļ��Ͳ��Գ��򿽱������ļ�ϵͳ��
$ cp *.ko  /source/rootfs 
6��	�����ϲ���ģ��
# sudo  insmod  fs4412_key.ko
7��	���Թ���
��key2��key3�鿴����
	
	
}	

{//pwm����
ʵ���ʮ�� PWM������д
��ʵ�����ݡ�
��дһ���ַ����������������������
��ʵ��Ŀ�ġ�
����GPIO�Ĳ�����linux��pwm timer��ʹ��
��ʵ��ƽ̨��
	������Ubuntu 12.04
	Ŀ�����FS4412
	Ŀ����ں˰汾��3.14.0
	����������汾��arm-none-linux-gnueabi-gcc-4.6.4
��ʵ�鲽�衿
1��	����ģ��Դ��Ŀ¼
��ʵ������е�fs4412_ledĿ¼������/home/linuxĿ¼�²������Ŀ¼��
2��	����ģ��
$ make
3��	����Ӧ�ó���
$ arm-none-linux-gnueabi-gcc pwm_music.c -o pwm_music
4��	��ko�ļ��Ͳ��Գ��򿽱������ļ�ϵͳ��
$ cp *.ko pwm_music /source/rootfs 
5��	�����ϲ���ģ��
# sudo  insmod  fs4412_pwm.ko
6��	�����豸�ڵ�
# mknod  /dev/pwm  c  500  0
7��	���Թ���
# ./pwm_music	
	
}		

}

{//================================I2C�豸===============================
{//��ʲô��I2C�豸
   Ӳ����
      CLK  ----- ʱ����
      DATA ----- ������    
      
   i2c��һ��ͬ����˫����ͨ�����ߣ�ʹ��i2c���ߵ��豸��ΪI2C�豸��  
}	             
 
{//I2C�������   (2C������ �� I2C����, ͨ��ϵͳ�������ˣ����������޸�)
                            Adapter      
                              |
                              | 
                              |Bus��algorithm��
                      -----------------
                      |       |       |  Driver
                   Client  Client Client  
                   
                   
                   I2c-core �ṩ����֧��
            

  Adapter     -----   ������,�൱�� I2C������
  Bus         -----   I2C���ߣ��漰 algorithm
  Client      -----   ���ص�i2c�����ϵ�i2c���豸
  Driver      -----   Client������     
  
  I2C-core    -----   I2C���ģ��ṩ����֧�֣���adapter client driver �����ע��)     	
 
}

{//ʵ���ʮ�� I2C������д������
	��ʵ�����ݡ�
	��дһ������i2c���ߵ�E2PROM������
	��ʵ��Ŀ�ġ�
	����I2C���ߵĲ�������
	��ʵ��ƽ̨��
		������Ubuntu 12.04
		Ŀ�����FS4412
		Ŀ����ں˰汾��3.14.0
		����������汾��arm-none-linux-gnueabi-gcc-4.6.4
	��ʵ�鲽�衿
	ע�⣺��ʵ�������"$"��Ĳ����������ϣ�"#"��Ĳ����ڿ�������
	1��	�����ں�Դ���޸� arcg/arm/boot/dts/exynos4412-fs4412.dts�ļ�,����������ݣ�
		i2c@138B0000 {   //��Ӧ 29 Inter-Integrated Circuit  ��I2C 5  -->  0x138B_0000
			samsung,i2c-sda-delay = <100>;
			samsung,i2c-max-bus-freq = <20000>;
			pinctrl-0 = <&i2c5_bus>;   //����·ͼ I2C_SDA5  I2C_SCL5  --> GPB2  GPB3  --> �� exynos4x12-pinctrl.dtsi �� i2c5_bus
 			pinctrl-names = "default";
			status = "okay";
	
			mpu6050-3-axis@68 {
				compatible = "invensense,mpu6050";  //Ҫ�������������һ��
				reg = <0x68>;   /*i2c slave address  ��mpu6050 ��оƬ�ֲ� PS-MPU-6000A-00v3.4.pdf ��  9.2 I2C Interface
				                     The slave address of the MPU-60X0 is b110100X which is 7 bits long. 
				                     The LSB bit of the 7 bit address is determined by the logic level on pin AD0. 
				                     This allows two MPU-60X0s to be connected to the same I2C bus. 
				                     When used in this configuration, the address of the one of the devices should be b1101000 (pin AD0 is logic low)
				                      and the address of the other should be b1101001 (pin AD0 is logic high).
				                   ���·ͼ�� AD0 ��� �� 
				                   i2c�豸�ĵ�ַ�� b1101000 -->0x68
				                 */ 
				interrupt-parent = <&gpx3>;  //����· GYRO_INT -> GPX3_3
				interrupts = <3 2>;  //3 ��Ӧ gpx3 ��3�Źܽ� 2��ʾ�жϵĴ�����ʽΪ �½���  
			};

		};
	2��	���±���dts�ļ���������/tftpbootĿ¼��
	$ make dtbs
	$ cp arch/arm/boot/dts/exynos4412-fs4412.dtb /tftpboot
	3��	����ģ��Դ��Ŀ¼
	��ʵ������е�mpu6050Ŀ¼������/home/linuxĿ¼�²������Ŀ¼��
	4��	����ģ��
	$ make	
	5��	����Ӧ�ó���
	$ arm-none-linux-gnueabi-gcc  test.c  -o  test
	6��	��ko�ļ��Ͳ��Գ��򿽱������ļ�ϵͳ��
	$ cp *.ko test /source/rootfs 
	7��	�����ϲ���ģ��
	# sudo  insmod  mpu6050.ko
	8��	�����豸�ڵ�
	# mknod  /dev/mpu6050  c  500  0
	
	9��	���Թ���
	# ./test 
	��ת���ӣ��鿴�ն���ʾ
	
	//------����������� �� mpu6050.c
	static int mpu6050_read_byte(struct i2c_client *client, unsigned char reg)
	{
		int ret;
	
		char txbuf[1] = { reg };
		char rxbuf[1];
	
		struct i2c_msg msg[2] = {   //�趨 I2C��Ϣ��ʽ 
			{client->addr, 0, 1, txbuf},       //0         ָ��i2c�豸��ļĴ�����ַ   txbuf ������ַ
			{client->addr, I2C_M_RD, 1, rxbuf} //I2C_M_RD  ��ʾ��i2c�豸��üĴ���������  
		};
	
		ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg)); //����I2C��Ϣ
		if (ret < 0) {
			printk("ret = %d\n", ret);
			return ret;
		}
	
		return rxbuf[0];
  }
  
} 
	
}

{//================================���豸================================

{//��ʲô�ǿ��豸
	�ܹ�������ʹ̶���С����512�ֽڣ�����Ƭ��chunk�����豸���������豸

	//���豸���ַ��豸����
	             ���豸      �ַ��豸
	  ���ʵ�λ   �̶���С      ��        //���豸������С��λ������sector����������512�ֽڣ�
	  �������   ֧��          ��֧��    //���豸Ϊ����Ч�ʣ���I/O�����ж�Ӧ�Ļ�����������������ʣ����ַ��豸�޻�������ֻ�ܰ��ֽ����Ⱥ�˳�����
	  ֱ�ӷ���   ����          ��        //
	  ��������   ����        ��Լ�    //��Ȼ���豸�ϸ��ӣ�����linuxϵͳ�п��豸֧�����൱���죬���������н����漰�� ��������ԭ����      
}		

{//���豸ԭ��
	//-----�����豸Ӳ��ԭ��
    ���� -->��ͷ -> �ŵ� -> ���� -> ���棨����̵��Ӻ󣬴ŵ��γ����棩
   
	//-----�����ο�
	  Ӧ��	  
	        open read 
	  =======================
	  �ں�      |	  
	     ----------------
	     | System  Call |
	     ----------------	
	            |	  
	     --------------------------
	     |     VFS                | //�����ļ�ϵͳ�� ����linux�������ļ�ϵͳ������ӿڡ�  ext2 jffs2 rootfs(ram fs) �������ľ���ʵ��
	     --------------------------	
	            |
	     --------------------
	     |    File  System  |   // ext2, ext3, jffs2 ��
	     --------------------      
	            |	  	     
	            |           
	  	 -------------- 
	  	 |Buffer Cache|   //��������Ϊ�˼ӿ���豸��д�� ���ȶ���������û��ʱ��ͨ��File  System ȥ��ȡ
	  	 --------------    /* buffer_head ������ͷ��Ϣ
						 |				 struct  buffer_head  {
						 |				    unsigned long    b_state;          //������״̬��־
						 |				    struct buffer_head    *b_this_page;//ҳ���еĻ�����
						 |				    struct page    *b_page;            //�洢��������ҳ��
						 |				    sector_t    b_blocknr;         //�߼����
						 |				    size_t    b_size;              //���С
						 |				    char    *b_data;               //ҳ���еĻ�����
						 |				    struct block_device    *b_bdev; //���豸
						 |				    bh_end_io_t    *b_end_io;       //I/O��ɷ���
						 |				     void    *b_private;          //��ɷ�������
						 |				    struct list_head   b_assoc_buffers;  //���ӳ������
						 |				    struct address_space   *b_assoc_map;     //mapping this buffer is  associated with 
						 |				    atomic_t    b_count;           //������ʹ�ü���
						 |				}; 
	  	       |          */
             |      
	     -----------------------
	     | generic block layer |   //ͨ�ÿ��
	     ----------------------- /*  bio �ǿ�I/O�����Ļ������� ��  ͨ��һ��bio��Ӧһ��I/O���� ��һ��������԰������bio 
	            |            struct bio      �����������ֳ��ģ���ģ���Ƭ�Σ�segment��������ʽ��֯�Ŀ�I/O������
							|						 {
							|							    sector_t bi_sector;  //Ҫ����ĵ�һ������ 
							|							    struct bio *bi_next; // ��һ��bio 
							|							    struct block_device	*bi_bdev;
							|							    unsigned long bi_flags; // ״̬������� 
							|							    unsigned long bi_rw; // ��λ��ʾREAD/WRITE����λ��ʾ���ȼ� 
							|							    unsigned short bi_vcnt; // bio_vec���� 
							|						      unsigned short bi_idx; // ��ǰbvl_vec���� 
							|						      unsigned short bi_phys_segments; //�����ڵ�����ε���Ŀ
							|						      .....
							|						}
	            |                */
	            |	              
	     ----------------
	     | I/O schedule |    //ͨ��I/O�����㷨��������㷨����ʹ���������
	     ----------------    
	            |
	            |  Request Queue /*  I/O ������У�������ʹ�ö��I/O�������������ŷ�ʽ�������ύI/O����
							|						struct request_queue 
							|						{
							|						   spinlock_t *queue_lock;// �������нṹ���������
							|						  unsigned long nr_requests; /// ������������ 
							|					    unsigned short max_sectors;  //���������� 
							|					    unsigned short hardsect_size;  // Ӳ�������ߴ�
							|				      	��
							|					  }
              |
	            |           struct request
							|						{
							|								struct list_head queuelist; //����ṹ
							|								unsigned long flags; // REQ_
							|								sector_t sector; // Ҫ���������һ������
							|								unsigned long nr_sectors; //Ҫ���͵�������Ŀ														
							|								unsigned int current_nr_sectors; //��ǰҪ���͵�������Ŀ
							|								sector_t hard_sector; //Ҫ��ɵ���һ������
							|								unsigned long hard_nr_sectors; //Ҫ����ɵ�������Ŀ															
							|								unsigned int hard_cur_sectors; //��ǰҪ����ɵ�������Ŀ
							|						 		char *buffer; //���͵Ļ��壬�ں������ַ
							|						    ��
							|						}
	            |                */ 
	     ----------------
	     | block driver |   //ͨ��ϵͳ�ṩ�Ľӿں�������  blk_init_queue blk_alloc_queue elv_next_request( blk_start_queue��,��ɶԿ��豸����. 
	     ----------------   //����ֻ�����I/O���󣬾����ʱ��ȡ������ϵͳ����
	            |
	  ======================
	  Ӳ��      |
	     ----------------
	     |Disk Flash... |   //���豸���ڴ棬Flash,Ӳ��disk , U�̵�
	     ----------------
}	 
	
{//����θ�Ч���ʿ��豸 
	request_queue  -->  request  -->  bio  -->  bio_vec  //�ѵ㣺 Ϊ��Ҫ�õ����ǣ� ���Ǽ�Ĺ�ϵ��ʲô
	
	
  //��Ⱦ���
    ����     <=================>  �ַ��豸 //Ӧ�ü򵥵�һ�ξ��ܻ�ȡ������
                �˿�  --- Ӧ��
               ���۵� --- �ַ��豸 
  
  
  
    ���ڲɹ� <=================> ���豸    //Ӧ�ô�Ӳ�̣�flash���豸���ǻ�ȡ���������ݣ� Ч�ʺܹؼ�    	
             ��ɹ��� --- Ӧ��          
                |          |
                |          |
   �ɶ�/����...�Ķ���  request_queue   //request_queue��������ͨ�� I/O�����㷨��������㷨����ʹ����Դ����ʹ����ѣ�
                |          |              
               ��˾     Ӳ�̿��豸   
                |          |
         �ɶ��ֹ�˾����  request   //ָ���������ȡ���ݣ�����ĸ���������ȡ���ٸ��������ݣ�
                |          |
    ���/���� ..�ֿ�       bio    //ָ�����ݾ���λ�ã�request�пɰ������bio��
	              |          | 
	       2��5�ų�����   bio_vec   //ָ�����ݴ��䵽�Ǹ��ڴ�����   ���ڴ���������ɢ�ģ���bio��Ҫ�������bio_vec��  
	
}			
	
{//ʵ�� ���ڴ��������
  {//---ʹ��
	 	1.	�����뿽�����������
		2.	���뵽Դ��Ŀ¼����Makefile���޸�KERNELDIRΪ��ֲ�õ��ں�Դ��Ŀ¼
		3.	ִ��make�����������ģ��
		4.	��������ģ��.ko�ļ������ļ�ϵͳ
		5.	�򿪴����նˣ��������ϵ磬��ϵͳ������ͨ��NFS���غø��ļ�ϵͳ���ûس��������ն�
		6.	
		7.	���ն���ִ��insmod fsdsk.ko�����������ģ�飬����������ϢΪ�����������Ϊû�з�����
			fsdska: unknown partition table
		8.	ִ��cat /proc/devices����鿴���豸���뵽�����豸��
		[root@farsight /]# cat /proc/devices 
		����
		254 fsdsk
			9.	ִ��cat /proc/partitions����鿴��ǰϵͳ�еķ���
		[root@farsight /]# cat /proc/partitions 
			major minor  #blocks  name
			
			  31        0       1024 mtdblock0
			  31        1       3072 mtdblock1
			  31        2       4096 mtdblock2
			  31        3     253952 mtdblock3
			 254        0       8192 fsdska
		10.	ִ��fdisk /dev/fsdska������д��̷�������ʱ���ӡ������Ϣ��
			[root@farsight /]# fdisk /dev/fsdska 
			...
		
			Command (m for help):
		11.	���������ʾ����������n��س�����ʾҪ�½�һ����������ʱ���ӡ������Ϣ
		Command action
		   e   extended
		   p   primary partition (1-4)
		12.	����p���س�����ʾҪ����һ������������ʱ���������Ϣ
			Partition number (1-4):
		13.	����1���س�����ʾҪ����һ�����Ϊ1��������
		14.	�ڽ�����������������ֱ���ûس�����ʾʹ��Ĭ�ϵķ�����ʼ����ͽ������棬��ʱ���ӡ��������Ϣ
			First cylinder (1-256, default 1): Using default value 1
			Last cylinder or +size or +sizeM or +sizeK (1-256, default 256): Using default value 256
			
			Command (m for help): 
		15.	�������w���س�����ʾ���������Ϣ�����������Ϣ��ʾ�����ɹ�
			Command (m for help): w
			The partition table has been altered!
			
			Calling ioctl() to re-read partition table
			 fsdska: fsdska1
		16.	�´����ķ���Ϊfsdska1��ʹ��cat /proc/partitions������Բ鿴�µķ�����Ϣ
			[root@farsight /]# cat /proc/partitions 
			major minor  #blocks  name
			
			  31        0       1024 mtdblock0
			  31        1       3072 mtdblock1
			  31        2       4096 mtdblock2
			  31        3     253952 mtdblock3
			 254        0       8192 fsdska
			 254        1       8184 fsdska1
		17.	ִ��mkfs.ext2 /dev/fsdska1�����ʽ���´����ķ��������������Ϣ��ʾ��ʽ���ɹ�
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
		18.	ִ��mount /dev/fsdska1 /mnt������¸�ʽ���ķ������ص�/mntĿ¼
		19.	ִ��touch /mnt/a.txt������/mntĿ¼�´���a.txt�ļ�
		20.	ִ��echo 1234567890 > /mnt/a.txt�������ļ�д��һЩ����
		21.	ִ��cat /mnt/a.txt�����ȡ�ļ������ݣ������ݺ�д��ı���һ�£������ļ���д�ɹ�
		22.	ִ��umount /mnt���ж�ط���
		23.	ִ��rmmod fsdsk���ж������ 	
	}
		  
  {//---fsdsk.c   ʵ�������  ��request�н�һ��bio, һ��bio_vec�����ʹ�ô����Դ��ĵ��ݵ��Ȼ��� I/O  Schedule��
		#include <linux/init.h>
		#include <linux/kernel.h>
		#include <linux/module.h>
		
		#include <linux/fs.h>
		#include <linux/slab.h>
		#include <linux/genhd.h>
		#include <linux/blkdev.h>
		#include <linux/hdreg.h>
		#include <linux/vmalloc.h>
		
		
		#define FSDSK_MINORS		4   //Ӳ�̵ķ�����
		#define FSDSK_HEADS		4
		#define	FSDSK_SECTORS		16      //���̵�������
		#define FSDSK_CYLINDERS		256   //��������������̵Ĵŵ������γ����棩
		#define FSDSK_SECTOR_SIZE	512   //����������С
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
		
		//��ȡ���̼�����Ϣ
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
			req = blk_fetch_request(q);					// �����������ȡ����һ������
			while (req != NULL) {						// �ж��Ƿ����е������Ѵ������		
				offset = blk_rq_pos(req) * FSDSK_SECTOR_SIZE;			// �õ���ʼ�����ţ�����ӳ����ڴ��ƫ�Ƶ��ֽڵ�ַ
				nbytes = blk_rq_cur_sectors(req) * FSDSK_SECTOR_SIZE;	// �õ���ǰҪ���ʵ���������ת�����ֽ���
		
				if ((offset + nbytes) > bdev->size) {
					printk (KERN_NOTICE "Beyond-end write (%ld %ld)\n", offset, nbytes);
					return;
				}
		
				if (rq_data_dir(req) == WRITE)
					memcpy(bdev->data + offset, req->buffer, nbytes);
				else
					memcpy(req->buffer, bdev->data + offset, nbytes);
		
				if (!__blk_end_request_cur(req, 0)) {					// ������ǰ����ȡ����һ������
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
			// �������豸�ţ��������Ϊ0����ʾ�Զ�����һ�����豸��
			fsdsk_major = register_blkdev(fsdsk_major, fsdsk_name);
			if (fsdsk_major <= 0) {register_blkdev
				printk(KERN_WARNING "fsdsk: unable to get major number\n");
				return -EBUSY;
			}
		
			// �����豸�Ľṹ���ڴ�ռ�
			bdev = kmalloc(sizeof(struct fsdsk_dev), GFP_KERNEL);
			if (!bdev) {
				printk(KERN_NOTICE "kmalloc failure\n");
				goto unreg_dev;
			}
			memset(bdev, 0, sizeof(struct fsdsk_dev));
		
			// ������Ĵ��̴�С���и�ֵ
			bdev->size = FSDSK_SIZE;
			// ������̿ռ䣬���ڴ����������
			bdev->data = vmalloc(bdev->size);
			if (!bdev->data) {
				printk(KERN_NOTICE "vmalloc failure\n");
				goto free_dev;
			}
		
			spin_lock_init(&bdev->lock);
			bdev->queue = blk_init_queue(fsdsk_request, &bdev->lock);/* ��ʼ��һ��������У������Ŷ��ϲ�Դ��̵����󣬵��ǣ�
			          ������Ŷ������ϲ�ִ�еģ�����ֻ���ṩ fsdsk_request����һ�������������������ȡ�����󣬲��Ҵ�������
			          ����fops���治��Ҫ��д�Ľӿ�fsdsk_request����һ���������������ϲ���һ�����ʵ�ʱ����õ�
								�������������У������ṩ��һ���������������������������С�
								���ǵ�fsdsk_request��������һ��ԭ�������ĵ��С��ں��ھ��������������֮ǰ����Ҫ������������
								������ζ������������в��ܵ������������ߵ���һЩ����
																			           */
			blk_queue_logical_block_size(bdev->queue, FSDSK_SECTOR_SIZE);	// �����ںˣ�����ʵ���豸������������С���ϲ㽫��ѷ��ʵ����ݴ�С���뵽ʵ�ʵ�����������С��
			bdev->queue->queuedata = bdev;
		
			bdev->gd = alloc_disk(FSDSK_MINORS);							// ����gendisk�ṹ�壬ע�⣡�������� ����ṹ��ֻ�ܶ�̬����
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
			add_disk(bdev->gd);				// ע��!!!!!!!�ò���ֻ�������г�ʼ�����֮����С���Ϊ�ò���ִ�к󣬾ͱ�ʾ��������ȥ����������
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

{//===============================�����豸===============================
{//�����豸ԭ��
	
	  
	  Ӧ��
	                             ---------
	          Socket             |  ����  |
	            |                --------- 
	            |                   ||      //������˫�����
	  ====================================  //���ݸ���    
	  �ں�      |                   ||     
	            |                   ||       
	     ----------------           ||      //ͨ�������ݰ�ͷ�����ɾ���ᴩ����Э��ջ 
	     |              |         ���ݰ�    //Ϊ����Ч�ʽ����������ݸ��ƣ����ݵ���ͨ��sk_buf��ָ�����ƶ�λ��  
	     |              |        =========   
	     |   �����     |        |-------|    
	     | ��TCP/UDP)   |        | UDPͷ | 	     
	     |              |        |-------| 	     
	     |              |        | ����  | 	     
	     |              |        |-------| 		     
	     |              |        =========     
	     ----------------				    ||
              |                   ||            sk_buf        net_device  //�����豸��Ϊʵ�����������豸Ӳ����α仯���ϲ�Э�鲻�䣬��������ģ�  ��include/linux/netdevice.h
	     ----------------           ||          =========       ===========        
	     |              |         ���ݰ�        | ....  |       |         |        
	     |              |        =========      |       |       |  ����   |        
	     |              |        |-------|<---  | head  |       |  ״̬   |        
	     |    ����      |        | IPͷ  |      |       |       |  ͳ��   |        
	     |   ��IP��     |        |-------|<--- 	|data   | ----- |  ����   |  
	     |              |        | UDPͷ | 	    |       |       |  ����   |        
	     |              |        |-------| 	    |       |       |����ָ�� | //ͨ������ָ�룬ʵ�ֽӿڳ��󣨼��ӿڿɹ�����ͬ��ʵ�֣�       
	     |              |        | ����  | 	    |       |       ===========     
	     |              |        |-------| <--- | tail  |            
	     |              |        ========= <--- | end   |        
	     ----------------			      ||          =========            
              |                   || 
	     ----------------           ||
	     |              |         ���ݰ�
	     |              |        =========
	     |              |        |-------| 
	     |  ������·��  |        | MACͷ | 
	     | ��Ethernet)  |        |-------|
	     |              |        | IPͷ  | 
	     |              |        |-------| 	     
	     |              |        | UDPͷ | 	     
	     |              |        |-------| 	     
	     |              |        | ����  | 	     
	     |              |        |-------| 		     
	     |              |        =========       
	     ----------------			      ||
	            |                   ||        
	            |                   ||
	     -------------------------------
	     |                             | dm9000_probe //��ʼ��
	     |           ��������          | dm9000_start_xmit  //�����������ݰ�
	     |                             | dm9000_interrupt-->dm9000_rx->netif_rx //ͨ���жϷ�ʽ����ȡ����
	     -------------------------------   
	            |
	            |  
	  =====================================  // ���ݸ���
	  Ӳ��      |                   ||
	     ----------------        ----------
	     |    �����    |        | ������ |
	     | (����������) |  	     | ������ |
	     |����(��dm9000)|        ----------
	     | PHY �����   |
	     ----------------	       
	
}	

{//ʵ�� dm9000 ��������
 {//Ӳ��
 	  //��оƬ�ֲ�  DM9000AE.pdf
		 	  
		 	          --------DM90000�ڲ���ͼ----------   //DM9000Ӳ��������PHY MAC ��ʵ��������� ������·��Ĺ���
		 	          |   -------    -----  --------  |   //PHY:  ����ӿ��շ�������ʵ������㣨�������ݴ���������ĵ�����źš���·״̬�����ݱ���ȣ�  
		  TX+/-  ---|   | PHY | -- |MII|--|  MAC |  |   //MII:  ý������ӿ�  ����PHY ��MAC , ʵ�� MAC��������£���Ӧ��ͬ��PHY�豸
		 	RX+/-  ---|   -------    -----  --------  |   //MAC:  (Media Access Control) ����������·�㣨����֡�Ĺ��������ݲ���顢���Ϳ��ơ���������ṩ��׼�����ݽӿڣ��� 
		 	          |         -------               |  
		 	          |         |SRAM |               |   //SRAM:  оƬ�ڲ��Դ�16K SARM(3KB�������ͣ�13KB��������).     
		 	          |         -------               |
		 	          |--------------------------------
		
		//����·ͼ 
		 	         
 }
 	
	{//----arch/arm/boot/dts/exynos4412-fs4412.dts   �µ�platform_device��ʽ -> �豸��		
		srom-cs2@5000000 {   /*�����Ļ���ַ
			��·ͼ Xm0cs1  -->оƬ�ֲ� SROM Controller 19.3 I/O Description �� nGCS[3:0] Bank selection signal  Xm0CSn_x 
			               -->3 Memory Map  ��  0x0500_0000 0x0600_0000 16 MB  Bank1 of SMC				             
			                   */
			compatible = "simple-bus"; //"simple-bus"ָSoCƬ�����ߣ�����platform��bus���� ����������
			#address-cells = <1>;
			#size-cells = <1>;
			reg = <0x5000000 0x1000000>;  //��ӦоƬ�ֲ� 3 Memory Map ��0x0500_0000  �� 16 MB  ��0x1000000����16M��
			ranges;

			ethernet@5000000 {
				compatible = "davicom,dm9000";  //�ں�ͨ����������ƥ������ ����Ӧ drivers/net/ethernet/davicom/dm9000.c ��� platform_driver�� .name = "dm9000")
				reg = <0x5000000 0x2 0x5000004 0x2>; /*dm9000�ڲ���256���Ĵ�����ͨ������ӿں�����򽻵� ����ַ�Ĵ��������ݼĴ�����
		0x5000000      ��Ӧ    Xm0ADDR2 -- CMD=0  ��ʾָ����ǵ�ַ�Ĵ��� ��ָ����������256���Ĵ����е��Ǹ���
		      0x2      ��Ӧ    �����߿��2���ֽڣ�16λ) -->  ��· Xm0DATA0~Xm0DATA15.  �������64λ�����ߣ�����4��
		0x5000004      ��Ӧ		 Xm0ADDR2 -- CMD=1  --->0x4  ָ�����ݼĴ���  
				   */
				interrupt-parent = <&gpx0>;  //�жϿ������̳���gpx0
				interrupts = <6 4>; /*��� Documentation/devicetree/bindings/arm/gic.txt
				6  ��Ӧ�ж�Դ DM9000_IRQ -> XEINT6 �� 
				4 = active high level-sensitive �ߵ�ƽ�����ж�
				  */					  
				davicom,no-eeprom;
				mac-address = [00 0a 2d a6 55 a2];  //���������ַ
			};
			
		};		
		
			
	}	 	
 	
 	
 {// drivers/net/dm9000.c Դ�����

			/*
			 *   Write a byte to I/O port
			 */
			
			static void
			iow(board_info_t * db, int reg, int value)   //db->io_addr ��ȡ ��dm9000_probe������ƽ̨�豸�л�ȡ��Դ����ioremap ӳ���ַ��
			{
				writeb(reg, db->io_addr);
				writeb(value, db->io_data);
			}			
			
			
			static void dm9000_timeout(struct net_device *dev) //���䳬ʱʱ���øú�������ʱʱ����watchdog�趨��
			{
			}



			/*
			 *  Hardware start transmission.
			 *  Send a packet to media from the upper layer. 
			 */
			static int
			dm9000_start_xmit(struct sk_buff *skb, struct net_device *dev) //��Ӳ���������ݰ������ϲ���ã�
			{
			 unsigned long flags;
			 board_info_t *db = netdev_priv(dev);
			
			 dm9000_dbg(db, 3, "%s:\n", __func__);
			/*һ�����ֻ�ܷ����������ݰ���������SRAM��ֻ�ܴ������������͵����ݰ���
			 DM9000A�ڲ���һ��4K Dword SRAM������3KB����Ϊ���ͣ�16KB��Ϊ���գ�
			 ����0x0000~0x0BFF�Ǵ�˵�е�TX buffer(TX buffer��ֻ�ܴ��������)��0x0C00~0x3FFF��RX buffer��
			 �����д�ڴ����ʱ����IMR�ĵ�7λ�����ã���������˵�ַ�Ľ�β���絽��3KB����ؾ�0�����Ƶķ�ʽ��
			 �ڶ������У���IMR�ĵ�7λ��������������˵�ַ�Ľ�β����16K����ؾ�0x0C00��
			*/
			 if (db->tx_pkt_cnt > 1)
			  return 1;
			
			 spin_lock_irqsave(&db->lock, flags);
			
			 /* Move data to DM9000 TX RAM */
			 /*
			 MWCMD��F8H�����洢������ַ�Զ����ӵĶ���������.
			 7-0��MWCMD��д���ݵ�����SRAM�У�֮��ָ���ڲ�SRAM�Ķ�ָ���Զ�
			 ����1��2��4�����ݴ������Ĳ���ģʽ������8λ��16λ��32λ���� 
			 */
			 writeb(DM9000_MWCMD, db->io_addr);
			
			 (db->outblk)(db->io_data, skb->data, skb->len);
			 dev->stats.tx_bytes += skb->len;
			
			 db->tx_pkt_cnt++;//��¼д������SRAM�д����͵����ݰ�������
			 /* TX control: First packet immediately send, second packet queue */
			 if (db->tx_pkt_cnt == 1) {
			  /* Set TX length to DM9000 */
			  iow(db, DM9000_TXPLL, skb->len);
			  iow(db, DM9000_TXPLH, skb->len >> 8);
			//���д������SRAM�е����ݰ�ֻ��һ���������ݰ��ĳ���д��TXPLL��TXPLH��
			  /* Issue TX polling command */
			//TCR��02H�������Ϳ��ƼĴ�����0��TXREQ��TX�����ͣ����󡣷�����ɺ��Զ������λ��
			  iow(db, DM9000_TCR, TCR_TXREQ); /* Cleared after TX complete */
			  dev->trans_start = jiffies; /* save the time stamp */
			 } else {
			  /* Second packet */
			  db->queue_pkt_len = skb->len; //�����ǰд������ݰ����ǵ�һ������Ѹ����ݰ��ĳ��ȴ���db->queue_pkt_len��		
			  netif_stop_queue(dev); //ֻ��ͬʱ���������������ݰ������ú�����֪����ϵͳ��Ҫ����������
			 }
			
			 spin_unlock_irqrestore(&db->lock, flags);
			
			 /* free this SKB */
			/*
			ÿ�����ݰ�д������SRAM��Ҫ�ͷ�skb��
			������������ݰ�Ҫ���ڶ������ݰ��ĳ��ȴ���db->queue_pkt_len = skb->len
			*/
			 dev_kfree_skb(skb);
			
			 return 0;
			}


			/*
			 *  Received a packet and pass to upper layer
			 */ 
			static void
			dm9000_rx(struct net_device *dev)  //����һ�����ݰ����뻺��skb����ͨ������netif_rx���û��潻���ϲ�
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
			  MRCMDX��F0H�����洢����ַ����Ķ���������.
			  ʼ�ն�ȡ���ݰ��ĵ�һ���ֽڣ�ֱ������01H������Ч���ݰ���Ϊֹ��
			  */
			  ior(db, DM9000_MRCMDX); /* Dummy read */
			
			  /* Get most updated data */
			  rxbyte = readb(db->io_data);
			
			  /* Status check: this byte must be 0 or 1 */
			  if (rxbyte > DM9000_PKT_RDY) {
			   dev_warn(db->dev, "status check fail: %d\n", rxbyte);
			   iow(db, DM9000_RCR, 0x00); /* Stop Device   DM9000_RCR ����� drivers/net/dm9000.h*/     
			   iow(db, DM9000_ISR, IMR_PAR); /* Stop INT request */
			   return;
			  }
			//������ݰ���һ�ֽ�Ϊ00H��Ϊ��Ч���ݰ���
			  if (rxbyte != DM9000_PKT_RDY)
			   return;
			
			  /* A packet ready now  & Get status/length */
			  GoodPacket = true;
			  //MRCMD��F2H�����洢������ַ�Զ����ӵĶ��������
			  writeb(DM9000_MRCMD, db->io_addr);
			//��ȡ���ݰ���ǰ���ֽڣ�����Ч��־������״̬�����ݰ����ȡ����ڽṹ��rxhdr��
			  (db->inblk)(db->io_data, &rxhdr, sizeof(rxhdr));
			
			  RxLen = le16_to_cpu(rxhdr.RxLen);
			
			  if (netif_msg_rx_status(db))
			   dev_dbg(db->dev, "RX: status %02x, length %04x\n",
			    rxhdr.RxStatus, RxLen);
			
			  /* Packet Status check */
			  if (RxLen < 0x40) {//һ�����ݰ��ĳ���Ӧ����64�ֽ�
			   GoodPacket = false;
			   if (netif_msg_rx_err(db))
			    dev_dbg(db->dev, "RX: Bad Packet (runt)\n");
			  }
			
			  if (RxLen > DM9000_PKT_MAX) {//���ݰ����Ȳ�Ӧ����1.5K
			   dev_dbg(db->dev, "RST: RX Len:%x\n", RxLen);
			  }
			
			  /* rxhdr.RxStatus is identical to RSR register. */
			  //rxhdr.RxStatus��ֵ����RSR��06H��������״̬�Ĵ���
			  //�Ը��ִ�������жϺͼ�¼��
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
			  //�����һ���õ����ݰ������skb�ṹ�壬���㹻���棬�������ݶ��뻺��
			  if (GoodPacket
			      && ((skb = dev_alloc_skb(RxLen + 4)) != NULL)) {
			   skb_reserve(skb, 2);
			   rdptr = (u8 *) skb_put(skb, RxLen - 4);
			
			   /* Read received packet from RX SRAM */
			
			   (db->inblk)(db->io_data, rdptr, RxLen);
			   dev->stats.rx_bytes += RxLen;
			
			   /* Pass to upper layer */
			/*
			���������ȡЭ���ʶ( ETH_P_IP, ���������� )����̫��ͷ; 
			��Ҳ��ֵ skb->mac.raw, �ӱ��� data (ʹ�� skb_pull)ȥ��Ӳ��ͷ��,
			 �������� skb->pkt_type. ���һ���� skb ������ȱʡΪ PACKET_HOST
			 (ָʾ�����Ƿ������������), eth_type_trans �ı�������ӳ��̫��Ŀ�ĵ�ַ: 
			 ��������ַ��ƥ��������Ľӿڵ�ַ, pkt_type ��Ա����Ϊ 
			 PACKET_OTHERHOST. ���, ���ǽӿڴ��ڻ���ģʽ�����ں˴��˱���ת��, 
			 netif_rx �����κ�����Ϊ PACKET_OTHERHOST �ı���. 
			 
			//union { /* ... */// } h; 
			//union { /* ... */ } nh; 
			//union { /*... */} mac; 
			//ָ�����а����ĸ�����ͷ��ָ��.�����ڽṹ��struct net_device *dev;��
			
			*/   
			   skb->protocol = eth_type_trans(skb, dev);
			/*
			�ݽ� socket ������ϲ�. ʵ���� netif_rx ����һ������; 
			NET_RX_SUCCESS(0) ��˼�Ǳ��ĳɹ�����; �κ�����ֵָʾ����. 
			�� 3 ������ֵ (NET_RX_CN_LOW, NET_RX_CN_MOD, �� NET_RX_CN_HIGH )
			ָ��������ϵͳ�ĵ�����ӵ������; NET_RX_DROP ��˼�Ǳ��ı�����
			*/
			   netif_rx(skb);
			   dev->stats.rx_packets++;
			
			  } else {
			   /* need to dump the packet's data */
			//��������ݰ��ǻ��ģ�����������ݰ�������
			   (db->dumpblk)(db->io_data, RxLen);
			  }
			 } while (rxbyte == DM9000_PKT_RDY);//�������Ч���ݰ����˳�
			}
			
			
			/*��һ�����ݰ������꣬һ�����ݰ����յ���������·״̬�ı䣬�����жϣ����ø��жϴ�������
			  �ڷ��ж�ģʽ�£�������dm9000_poll_controller���á�
			
			  ������ݰ����꣬����һЩ׼��������׼���������İ�
			  ���Ӳ���н��յ��Ϸ������ݰ��������dm9000_rx �����ݴ���������ϲ�
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
			 //IMR��FFH�����ն����μĴ��������������жϡ�
			 iow(db, DM9000_IMR, IMR_PAR);
			
			 /* Got DM9000 interrupt status */
			 /*
			 ISR��FEH�����ն�״̬�Ĵ���.
			 ISR�Ĵ�����״̬д1���.
			 */
			 int_status = ior(db, DM9000_ISR); /* Got ISR */
			 iow(db, DM9000_ISR, int_status); /* Clear ISR status */
			
			 if (netif_msg_intr(db))
			  dev_dbg(db->dev, "interrupt status %02x\n", int_status);
			
			 /* Received the coming packet */
			 //һ���жϺſ��ܱ�����ж�Դ����������Ҫ�ж����ĸ��ж�Դ��
			 if (int_status & ISR_PRS)
			  dm9000_rx(dev);    //����dm9000_rx  �����������ݰ�
			
			 /* Trnasmit Interrupt check */
			 if (int_status & ISR_PTS)
			  dm9000_tx_done(dev, db);
			//DM9000E����·״̬�����ı�ʱ�������ж�
			 if (db->type != TYPE_DM9000E) {
			  if (int_status & ISR_LNKCHNG) {
			   /* fire a link-change request */
			   schedule_delayed_work(&db->phy_poll, 1);
			  }
			 }
			
			 /* Re-enable interrupt mask */
			 //����ʱʹ����Ӧ�ж�
			 iow(db, DM9000_IMR, db->imr_all);
			
			 /* Restore previous register address */
			 writeb(reg_save, db->io_addr);
			
			 spin_unlock_irqrestore(&db->lock, flags);
			
			 return IRQ_HANDLED;
			}

      //NAPI 	���и��ٴ������ݰ�ʱ�����α��жϺ���Ӧ������ѯ��ʽ�������ݰ�������Ƶ���жϸ�CPU��ɵĸ���  
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
		dm9000_open(struct net_device *dev)  //�����������ں�ע���жϣ���λ����ʼ��dm9000�����MII�ӿڣ�ʹ�ܴ����
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
		
			mii_check_media(&db->mii, netif_msg_link(db), 1); //MII���ý��״̬����Ҫ���Link״̬
			netif_start_queue(dev); //�����ϲ����������������ռ��л��������ã���ʼ�������ݰ���������
			
			dm9000_schedule_poll(db);//������ʱ���ȡ�������dm9000_poll_work���ڼ����������״̬
		
			return 0;
		}


		static const struct net_device_ops dm9000_netdev_ops = {//�����豸��������(����file_operations)
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
		dm9000_probe(struct platform_device *pdev)    //̽���豸��ò�������Դ��Ϣ��������Щ��Ϣ�����ڴ���жϣ�������register_netdevע����������豸
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
			ndev = alloc_etherdev(sizeof(struct board_info)); //���������豸 net_device
			if (!ndev) {
				dev_err(&pdev->dev, "could not allocate device.\n");
				return -ENOMEM;
			}
		
			SET_NETDEV_DEV(ndev, &pdev->dev);   //��platform_device��net_device��������
		
			dev_dbg(&pdev->dev, "dm9000_probe()\n");
		
			/* setup board info structure */
			db = netdev_priv(ndev);
		
			db->dev = &pdev->dev;
			db->ndev = ndev;
		
			spin_lock_init(&db->lock);
			mutex_init(&db->addr_lock);
		
			INIT_DELAYED_WORK(&db->phy_poll, dm9000_poll_work);
		
		  //��ȡƽ̨�豸��Դ������DM9000��ַ�Ĵ�����ַ��DM9000���ݼĴ�����ַ����DM900��ռ�õ��жϺ�
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
		
		 //�����ַ�Ĵ���IO�ڴ�����ӳ��  
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
		  //�������ݼĴ���IO�ڴ�����ӳ��
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
		
		  //��ȡ�豸ID���ж��Ƿ��������ܹ����������оƬ
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
			
		   //��������оƬ�Ľӿں���
			ndev->netdev_ops	= &dm9000_netdev_ops;
			ndev->watchdog_timeo	= msecs_to_jiffies(watchdog);
			ndev->ethtool_ops	= &dm9000_ethtool_ops;  //���ڲ�ѯ��������������
		
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
			ret = register_netdev(ndev);   //ע�������豸����
		
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
		
		
		static struct platform_driver dm9000_driver = { //��dm9000��װ�� ƽ̨�豸����
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
		
			return platform_driver_register(&dm9000_driver); //��dm9000��װ����ƽ̨�豸,  ��driver��device����һ��ʱ����ע��ɹ�������dm9000_probe    
			                                                 /*dm9000��ƽ̨�豸����� arch/arm/mach-s5pc100/mach-smdkc100.c ��   s5pc100_device_dm9000
									#if  defined(CONFIG_DM9000)
									static struct resource dm9000_resources[] = {  dm9000�ڲ���256���Ĵ�����ͨ������ӿں�����򽻵� ����ַ�Ĵ��������ݼĴ�����
									[0] = {  ��Ӧ��·ͼ��cs1 --> 0x88000000   addr2 --cmd=0    ��ʾָ����ǵ�ַ�Ĵ��� ��ָ����������256���Ĵ����е��Ǹ���
									.start        = 0x88000000,     ������flash��ֱ�ӹҽӵ�CPU ��BANK��. ������SMC   Bank 1
									.end         = 0x88000000 + 0x3,  ����64λ�������µ����߷���   
									.flags        = IORESOURCE_MEM,
									},
									[1] = {  ��Ӧ��·ͼ��cs1 --> 0x88000000   addr2 -- cmd=1 --->0x4  ָ�����ݼĴ���    
									.start        = 0x88000000 + 0x4,   
									.end        = 0x88000000 + 0x4 +0x3,   ����64λ�������µ����߷���  
									.flags        = IORESOURCE_MEM,
									},
									[2] = {
									    	.start = IRQ_EINT(10),
									    	.end   = IRQ_EINT(10),
									.flags        = IORESOURCE_IRQ | IRQ_TYPE_LEVEL_HIGH,
									},
									};
									 
									static struct dm9000_plat_data s5pc100_dm9000_platdata = {
									.flags        =   DM9000_PLATF_16BITONLY,   ָ�����߿�� (�ɵ�·ͼdata0~data15 ֪����16λ���ߣ�
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


{//�������� 

	   ------------------------           
     |����    192.168.9.5    |
     ------------------------
                |
     ------------------------
     |����    192.168.9.119  |    //����ת�ã��Ͱ��������IPҪ��ͬһ���Σ� �Ҳ��ܺ�����IPһ��     
     ------------------------
                |
	   ------------------------
     |�����  192.168.9.110   |
     ------------------------

  setenv serverip 192.168.9.110    //ע������������� ubuntu ��ipҪһ��
  setenv ipaddr 192.168.9.5
	setenv gatewayip  192.168.9.1	
	setenv bootcmd tftp 41000000 uImage\;tftp 42000000 exynos4412-fs4412.dtb\;bootm 41000000 - 42000000
	setenv bootargs root=/dev/nfs nfsroot=192.168.9.110:/nfs/rootfs rw console=ttySAC2,115200 init=/linuxrc ip=192.168.9.5		
	
}	


{//��ҵ
	
	1. ʵ�������Ч��
	
	2. ����Ӧ�ó���(ͬʱ����������, һ��ʵ����������˸5�룬��һ��ʵ�������Ч���� 
	
	3. һ��Ӧ�ó���ͬʱ�������������� һ���������Ƶƣ�һ�����Ʒ��������С�һ�����������ڽ����ַ�,�ַ�1 ���������ַ�p,�������У�
	
	4. ʵ�ְ���k2���£�����˸������k3���£���������
	
	
   ��Ϥƽ̨�豸��� ���豸��   

   ��taskletʵ���Ƴٴ���  //�ο�workqueue
   
   ʵ�ְ���key3���ж�     //�ο�key2�ж�
   
   ʵ�ְ������Ƶ���˸     //���豸������LED�Ĵ�����ַ�����Ƴٴ����� ��ʱ������   	
}	    