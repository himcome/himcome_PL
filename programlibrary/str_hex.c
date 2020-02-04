/**************************************************************************************************
* @File str_hex.c
* @2020-2-4    
***************************************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "himcome.h"

/**
**************************************************************************************************
* @Brief    StringToHex to Hex       
* @Param    
            @str: (to change hex)string 
            @out: a array to put Hex
            @outlen:the lengh of Hex
* @RetVal   
* @Note      
			How to use:
						int main(int argc,char** argv)
						{
							int cnt;
							unsigned char out[33];
							int outlen = 0;
							StringToHex(argv[1], out, &outlen);
							for(cnt = 0; cnt < outlen; cnt ++)
							{
								printf("%02X ", out[cnt]);
							} 
							putchar(10); 
							return 0;
						}
**************************************************************************************************
*/
unsigned int StringToHex(char *str, unsigned char *out, unsigned int *outlen)//str to hex
{
    unsigned char *p = str;
    unsigned char high = 0, low = 0;
    unsigned int tmplen = strlen(p), cnt = 0;
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
**************************************************************************************************
* @Brief    Hex to StringToHex       
* @Param    
            @pHex：address of Hex first num 
            @pAscii: ASCII first address
            @nlen:the lengh of Hex
* @RetVal   
* @Note      
			How to use:
						int main(int argc,char** argv)
						{
							unsigned char unicode_number[22]={0x00,0x31,0x00,0x37,0x00,0x38,0x00,0x31,0x00,0x31,
                                0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x39,0x00,0x32,0x00,0x37};
							unsigned char consumer_number[100]={0};
							int outlen = 0;
							HexToAscii(unicode_number, consumer_number, 22);							 
							return 0;
						}
**************************************************************************************************
*/
void HexToAscii(unsigned char *pHex, unsigned char *pAscii, int nLen)
{
    unsigned char Nibble[2];
    unsigned int i,j;
    for (i = 0; i < nLen; i++){
        Nibble[0] = (pHex[i] & 0xF0) >> 4;
        Nibble[1] = pHex[i] & 0x0F;
        for (j = 0; j < 2; j++){
            if (Nibble[j] < 10){            
                Nibble[j] += 0x30;
            }
            else{
                if (Nibble[j] < 16)
                    Nibble[j] = Nibble[j] - 10 + 'A';
            }
            *pAscii++ = Nibble[j];
        }               // for (int j = ...)
    }           // for (int i = ...)
}
