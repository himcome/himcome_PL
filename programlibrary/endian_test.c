#include <stdio.h>
 /**
**************************************************************************************************
* @Brief    Endian Test      
* @Param    
* @RetVal   
			@ 0 :Big Endian
			@ 1 :Little Endian 
			@-1 :Error
* @Note     
**************************************************************************************************
*/
unsigned int Endian_test( ) {
	 unsigned int endian_flag;
	 unsigned int endian_int=0x1234;
	 unsigned char *endian_out = (char*)&endian_int;	 
   	 if(*endian_out==0x12){
		 endian_flag = 0;}
	 else if (*endian_out==0x34){
		 endian_flag = 1;}
	 else {
		 endian_flag =-1;}
	 return endian_flag;
 }
 

