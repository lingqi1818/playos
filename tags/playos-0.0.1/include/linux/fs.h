/*
*文件系统头文件
*
*mail:lingqi1818@gmail.com
*/

//设备号=主设备号*256+次设备号
//主设备号：1-内存，2-磁盘，3-硬盘，4-ttyx,5-tty,6-并行口，7-非命名管道
#define MAJOR(a) (((unsigned)(a))>>8)
#define MINOR(a) ((a)&0xff) 
extern int ROOT_DEV;
