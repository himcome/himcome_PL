#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/shm.h>
#include <string.h>

#define TTY_DEV "/dev/ttyS0"   //test tty file path

/**
**************************************************************************************************
* @Brief    StringToHex to Hex       
* @Param    
            @str: (to change hex)string 
            @out: a array to put Hex
            @outlen:the lengh of Hex
* @RetVal   the lengh of Hex 
* @Note     How to use:
						int main(int argc,char** argv){
							int cnt;
							unsigned char out[33];
							int outlen = 0;
							StringToHex(argv[1], out, &outlen);
							for(cnt = 0; cnt < outlen; cnt ++){
								printf("%02X ", out[cnt]);
							} 
							putchar(10); 
							return 0;
						}
**************************************************************************************************
*/
int StringToHex(char *str, unsigned char *out, unsigned int *outlen)
{
    char *p = str;
    char high = 0, low = 0;
    int tmplen = strlen(p), cnt = 0;
    tmplen = strlen(p);
    while(cnt < (tmplen / 2))
    {
        high = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
        low = (*(++ p) > '9' && ((*p <= 'F') || (*p <= 'f'))) ? *(p) - 48 - 7 : *(p) - 48;
        out[cnt] = ((high & 0x0f) << 4 | (low & 0x0f));
        p ++;
        cnt ++;
    }
    if(tmplen % 2 != 0) out[cnt] = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
    
    if(outlen != NULL) *outlen = tmplen / 2 + tmplen % 2;
    return tmplen / 2 + tmplen % 2;
}

/**
************************************************************************************************
* @Brief    Single byte data inversion        
* @Param    
*            @DesBuf: destination buffer
*            @SrcBuf: source buffer
* @RetVal    None
* @Note      (MSB)0101_0101 ---> 1010_1010(LSB)
**************************************************************************************************
*/
void InvertUint8(unsigned char *DesBuf, unsigned char *SrcBuf)
{
    int i;
    unsigned char temp = 0;

    for(i = 0; i < 8; i++)
    {
        if(SrcBuf[0] & (1 << i))
        {
            temp |= 1<<(7-i);
        }
    }
    DesBuf[0] = temp;
}

/**
**************************************************************************************************
* @Brief    double byte data inversion        
* @Param    
*            @DesBuf: destination buffer
*            @SrcBuf: source buffer
* @RetVal    None
* @Note      (MSB)0101_0101_1010_1010 ---> 0101_0101_1010_1010(LSB)
**************************************************************************************************
*/
void InvertUint16(unsigned short *DesBuf, unsigned short *SrcBuf)
{
    int i;
    unsigned short temp = 0;

    for(i = 0; i < 16; i++)
    {
        if(SrcBuf[0] & (1 << i))
        {
            temp |= 1<<(15 - i);
        }
    }
    DesBuf[0] = temp;
}

/**
**************************************************************************************************
* @Brief   CRC16_MODBUS        
* @Param    
*            @puchMsg: data
*            @usDataLen: lengh of data
* @RetVal    (unsigned short)  val of crc16
* @Note      
**************************************************************************************************
*/
unsigned short CRC16_MODBUS(unsigned char *puchMsg, unsigned int usDataLen)  
{  
    unsigned short wCRCin = 0xFFFF;  
    unsigned short wCPoly = 0x8005;  
    unsigned char wChar = 0;  
    
    while (usDataLen--)     
    {  
        wChar = *(puchMsg++);  
        InvertUint8(&wChar, &wChar);  
        wCRCin ^= (wChar << 8); 
        
        for(int i = 0; i < 8; i++)  
        {  
            if(wCRCin & 0x8000) 
            {
                wCRCin = (wCRCin << 1) ^ wCPoly;  
            }
            else  
            {
                wCRCin = wCRCin << 1; 
            }            
        }  
    }  
    InvertUint16(&wCRCin, &wCRCin);  
    return (wCRCin) ;  
} 

/**
**************************************************************************************************
* @Brief     open file  and open set config     
* @Param    
            @baud: to choose band
            @databit: 数据位
			@parity : 校验位
			@stopbit: 停止位
* @RetVal    0   sucess 
            -1   fail
* @Note      
**************************************************************************************************
*/
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

/**
**************************************************************************************************
* @Brief    tty send and recv        
* @Param    
* @RetVal    
* @Note      
**************************************************************************************************
*/
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
    //printf("recv_len=%d\n",len);	
    len = read(fd, data, datalen);   
    return len;  
}  
int tty_test(int fd,char *test_data,int len) {
	char recv[512];
	memset(recv,0,sizeof(recv));
	tty_send(fd,test_data,len);
	usleep(200*1000);
	int ret = tty_recv(fd,recv,len);
	printf("tty_test recv ret=%d,recv=%s\n",ret,recv);
	if(!strcmp(recv,test_data))
		return 0;
	else
	    return 1;
	return 0;
}

/**
**************************************************************************************************
* @Brief    main      
* @Param    
*            @argv[1] :  band choose 9600  5
*            @argv[2] :  date
*            @argv[3] :
*            @argv[4] :
* @RetVal    None
* @Note      
**************************************************************************************************
*/
void main(int argc, char** argv) {
	char send[512];
	int cnt;
	int outlen = 0;
	memset(send,0,sizeof(send));  
	//sprintf(send,argv[2]);
	StringToHex(argv[2], send, &outlen);
	send[outlen]=CRC16_MODBUS(out,outlen);
    send[outlen+1]=(CRC16_MODBUS(out,outlen)>>8);
	for(cnt = 0; cnt < (outlen+2); cnt ++){
       	printf("%X ",out[cnt]);
    }
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