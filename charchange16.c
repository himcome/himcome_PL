#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fcntl.h>

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
int main(int argc, const char** argv)
{
    int cnt;
    unsigned char out[33];
    int outlen = 0;
    StringToHex(argv[1], out, &outlen);
    out[outlen]=CRC16_MODBUS(out,outlen);
    out[outlen+1]=(CRC16_MODBUS(out,outlen)>>8); 
    strcat(out,"\n");
    for(cnt = 0; cnt < (outlen+3); cnt ++)
    {
       	printf("%02X ",out[cnt]);
    }
    putchar(10); 
    int fp = open("test.txt", O_RDWR|O_CREAT|O_TRUNC|O_NONBLOCK); 
    write(fp,out,strlen(out));
    close(fp);

    return 0;
}
