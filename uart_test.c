#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/shm.h>

#define TTY_DEV "/dev/ttyS0"

int open_tty(int baud,int databit,int parity,int stopbit) {
	int fd;
	
	fd = open(TTY_DEV, O_RDWR | O_NOCTTY | O_NDELAY);
	//fd = open(TTY_DEV, O_RDWR|O_NONBLOCK);
	if(fd < 0)
	{
		printf("open err\n");
		return -1;
	}
	
	printf("open pass\n");
	tty_config(fd,baud,databit,parity,stopbit);
	
	return fd;
}

int close_tty(int fd) {
	if(close(fd) < 0) 
		return -1;
	else
		return 0;
}

speed_t tty_getBaudrate(int baudrate) {
	switch(baudrate) {
		case 1:return B115200;
		case 2:return B57600;
		case 3:return B38400;
		case 4:return B19200;
		case 5:return B9600;
		case 6:return B4800;
		case 7:return B2400;
		default:return -1;
	}
}

int tty_config(int fd,int baud,int databit,int parity,int stopbit) {
	speed_t speed;  
	struct termios cfg;
	struct termios options;
	//printf("fd:%d baud:%d %d %d %d\n",fd,baud,databit,parity,stopbit);
	speed = tty_getBaudrate(baud);
	if(speed == -1)
		return -2;
	
	tcgetattr(fd, &options);
	
	//cfsetispeed(&cfg, speed);
    //cfsetospeed(&cfg, speed);
	
	//修改控制模式，保证程序不会占用串口
    options.c_cflag |= CLOCAL;
    //修改控制模式，使得能够从串口中读取输入数据
    options.c_cflag |= CREAD;
	//不使用流控制
	options.c_cflag &= ~CRTSCTS;
	//设置数据位
    options.c_cflag &= ~CSIZE; //屏蔽其他标志位
    switch (databit) {
		case 5    :
        options.c_cflag |= CS5;
        break;
		case 6    :
        options.c_cflag |= CS6;
        break;
		case 7    :
        options.c_cflag |= CS7;
        break;
		case 8:
        options.c_cflag |= CS8;
        break;
		default:
        return -4;
    }
	//设置校验位
    switch (parity) {
		case 1://无校验
		options.c_cflag &= ~PARENB;
        options.c_iflag &= ~INPCK;
		break;
		case 2://Even-偶数校验
		options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;
		break;
		case 3://Odd-奇数校验
		options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;
        break;
		default:break;
	}
	// 设置停止位
    switch (stopbit) {
		case 1://1个停止位
		options.c_cflag &= ~CSTOPB;
        break;
		case 2://2个停止位
		options.c_cflag |= CSTOPB;
        break;
	}
	//修改输出模式，原始数据输出
    //options.c_oflag &= ~OPOST;
    //设置等待时间和最小接收字符
    //options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */
    //options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */
	//如果发生数据溢出，接收数据，但是不再读取
	options.c_oflag = 0; //输出模式  
    options.c_lflag = 0; //不激活终端模式
	cfsetospeed(&options, speed);//设置波特率 
    tcflush(fd,TCIFLUSH);
	//激活配置 (将修改后的termios数据设置到串口中）
    if (tcsetattr(fd,TCSANOW,&options) != 0)
        return -5;
    return 0;
}

int tty_send(int fd, char *data, int datalen)  
{  
    int len = 0;  
    len = write(fd, data, datalen);//实际写入的长度  
    if(len == datalen) {  
        printf("send_len=%d\n",len);
		return len;  
    } else {  
        tcflush(fd, TCOFLUSH);//TCOFLUSH刷新写入的数据但不传送  
        return -1;  
    }       
    return 0;  
}  

int tty_recv(int fd, char *data, int datalen)  
{  
    int len=0;  
    printf("recv_len=%d\n",len);	
    len = read(fd, data, datalen);   
    return len;  
}  

int tty_test(int fd,char *test_data,int len) {
	char recv[512];
	memset(recv,0,sizeof(recv));
	tty_send(fd,test_data,len);
	usleep(200*1000);
	int ret = tty_recv(fd,recv,len);
	//printf("tty_test recv ret=%d,recv=%s\n",ret,recv);
	if(!strcmp(recv,test_data))
		return 0;
	else
	    return 1;
	return 0;
}

void main(int argc, char** argv) {
	char send[512];
	memset(send,0,sizeof(send));
	//argv[2]-=0x30;
	sprintf(send,argv[2]);
	//printf("%d\n",atoi(argv[1]));
	int fd = open_tty(atoi(argv[1]),8,1,1);
	if(fd < 0)
		return;
	int ret = tty_test(fd,send,strlen(send));
	if(ret == 0)
		printf("tty test pass\n");
	else
		printf("tty test err\n");
	close_tty(fd);
}
