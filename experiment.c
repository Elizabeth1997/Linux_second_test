#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<unistd.h>
#include <math.h>
#include <netdb.h> 
#define PATH  "example.txt" 
    
    /*整个做实验的流程是：先上网查找实现相似操作的源代码，然后copy下来，大致看懂了之后在
    该源代码基础上进行修改，或是明白实现的思路，自己上网找相应的函数等。加上参考以及自己的思考，我实现
    该程序的思路是：读取cron文件中的内容到缓存——>扫描文件的每一行——>提取每一行的八字段——>判断四五字段是否需要改写
    ——>若需要改写则改写完成之后拼接字符串覆盖原有行记录——>不需要改写则不执行操作——>执行下一行重复步骤直到缓存内容全部
    读取完毕——>将cron文件内容清空——>将缓存内容加入cron文件中即可*/
	/*因为我电脑cron文件的内容都是规范的不需要修改，为了验证代码有效，我建立了一个文本文档example.txt,将
	读取的十几行cron记录复制过去，将几行的四五字段改为不规范*/

int main () 
{
  //获取ip地址
	/*这段核心代码是从网上copy下来的，几乎没有进行修改，之前用了一个很复杂的获取ip地址的办法
	后面老师点拨了一下，用hostname获取会更加简单*/
  struct hostent *he;  
  char hostname[40]={0} ;  
  char ipaddr[40]={0};  
  gethostname(hostname,sizeof(hostname));  
  he = gethostbyname(hostname);    
  char IP[128];  
  char **phe = NULL;  
  for( phe=he->h_addr_list ; NULL != *phe ; ++phe)  
  {  
      inet_ntop(he->h_addrtype,*phe,IP,sizeof(IP));  
  }


  int i;
  int j;
  char* token=")";
  char linebuffer[512] = {0}; //读取一行存入缓存区
  char month[512]={0};//每一行的第一个字段
  char day[10]={0};//每一行的第二个字段
  char time[20]={0};//每一行的第三个字段
  char host_name[20] = {0};  //一行中第4个字段的存入缓冲
  char system_name[20] = {0}; //一行中第5个字段的存入缓冲
  char six[50] = {0};
  char seven[50] = {0};
  char eight[50] = {0};//每一行的最后一个字段
  int k=0;
  int m=0;
  int flag;
  
  char save[100][512];  //修改之后的文件内容保存在该二维数组中
  

  FILE * pFile = fopen (PATH, "r+" );//创建一个可读写的example.txt 

  if (pFile == NULL)
    {
       	printf("open error");  
       	return -1;
    }
    //将文件内容保存到二维数组中,m的初始值为0
    while(fgets(save[m], 512, pFile))  
    {  
        m++;  
    }  
    //printf("%d\n",m);
    //现在m的值为文件的行数-1
    //文件指针指向文件的开头
  fseek(pFile,0,SEEK_SET);
    //fgets()函数用于将文件的所有行依次取出
  while(fgets(linebuffer, 512, pFile))  
    {  
        flag=0;//用于标记linebuffer是否可以复用
        sscanf(linebuffer, "%s %s %s %s %s %s %s %[^)]",month,day,time,host_name,system_name,six,seven,eight); 
        strcat(eight,token);//在最后一个字符串后面追加一个右括号）
        //printf(host_name);获取到的是software
        //printf(system_name);获取到的是CROND[6675]:
        //如果主机名为localhost,则用ip地址进行覆盖
        if(!strcmp("localhost",host_name))
        {
        	strcpy(host_name, IP);
          flag++;
        }  
        //比较第五个字段的前五个字符是不是小写的chron，如果是的话，将小写字母换成大写
        if(!strncmp("crond", system_name,5)) //比较前五个字符
        {  
          for(i=0;i<=4;i++)
          {
            system_name[i]=toupper(system_name[i]);//小写字母变为大写字母
          }
          flag++;              
        } 
        /*flag这个变量的作用是用来标记当前所读的一行记录是否要做修改和重新写入
          当flag为0时，说明行记录不需要被修改，所以不执行任何操作
          当flag不为0时，将字符串格式化拼接（中间带空格），然后覆盖改行日志记录内容
        */
        if(flag!=0)
        {
          //拼接字符串
          sprintf(save[k], "%s %s %s %s %s %s %s %s\n",month,day,time,host_name,system_name,six,seven,eight);
        }
        /*else
        {
           strncpy(save[k],linebuffer,512);  
        }*/
        //k值用来记录当前读取的行数
          k++;

    }
    //清空文件内容
    fseek(pFile,0,SEEK_SET);  
    truncate("example.txt",0);

    //将缓存内容写入文件
    fseek(pFile,0,SEEK_SET);  
    for(j=0;j<m;j++)  
     {  
        fwrite(save[j],strlen(save[j]),1,pFile);  
     }  
    fclose(pFile);  

    /*printf("%s\n", host_name);
    printf("%s\n", system_name);
    printf("%s\n", six);
    printf("%s\n", seven);
    printf("%s\n", eight);*/
    return 0;

}
       