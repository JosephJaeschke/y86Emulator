/* y86dis.c
* y86 disassembler
* joseph jaeschke
* December 9, 2016
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void getNxtCode(int ipointer,char *codeStr,char *code)
{
	int i;
	for(i=0;i<2;i++)
	{
		code[i]=codeStr[ipointer+i];
	}
}

void getNxt4Bytes(int pos,char *buffer,char *output)
{
	int i;
	int j=0;
	for(i=pos;i<pos+8;i++)
	{
		output[j]=buffer[i];
		j++;
	}
}

char *getRegister(char num)
{
	if(num=='0')
	{
		return "eax";
	}
	else if(num=='1')
	{
		return "ecx";
	}
	else if(num=='2')
	{
		return "edx";
	}
	else if(num=='3')
	{
		return "ebx";
	}
	else if(num=='4')
	{
		return "esp";
	}
	else if(num=='5')
	{
		return "ebp";
	}
	else if(num=='6')
	{
		return "esi";
	}
	else if(num=='7')
	{
		return "edi";
	}
	else
	{
		return "";
	}
}

void halt()
{
	printf("halt");
}

void nop()
{
	printf("nop");
}

void rrmovl(char r1,char r2)
{
	printf("rrmovl\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void cmovle(char r1,char r2)
{
	printf("cmovle\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void cmovl(char r1,char r2)
{
	printf("cmovl\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void cmove(char r1,char r2)
{
	printf("cmove\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void cmovne(char r1,char r2)
{
	printf("cmovne\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void cmovge(char r1,char r2)
{
	printf("cmovge\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void cmovg(char r1,char r2)
{
	printf("cmovg\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void irmovl(char r1,char *val)
{
	printf("irmovl\t$%sh ,%%%s",val,getRegister(r1));
}

void rmmovl(char r1,char r2,char *val)
{
	printf("rmmovl\t%%%s, %sh(%%%s)",getRegister(r1),val,getRegister(r2));
}

void mrmovl(char r1,char r2,char *val)
{
	printf("mrmovl\t%sh(%%%s), %%%s",val,getRegister(r2),getRegister(r1));
}

void addl(char r1,char r2)
{
	printf("addl\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void subl(char r1,char r2)
{
	printf("subl\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void andl(char r1,char r2)
{
	printf("andl\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void xorl(char r1,char r2)
{
	printf("xorl\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void mull(char r1,char r2)
{
	printf("mull\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void cmpl(char r1,char r2)
{
	printf("cmpl\t%%%s ,%%%s",getRegister(r1),getRegister(r2));
}

void jmp(char *dst)
{
	printf("jmp\t%sh",dst);
}

void jle(char *dst)
{
	printf("jle\t%sh",dst);
}

void jl(char *dst)
{
	printf("jl\t%sh",dst);
}

void je(char *dst)
{
	printf("je\t%sh",dst);
}

void jne(char *dst)
{
	printf("jne\t%sh",dst);
}

void jge(char *dst)
{
	printf("jge\t%sh",dst);
}

void jg(char *dst)
{
	printf("jg\t%sh",dst);
}

void call(char *dst)
{
	printf("call\t%sh",dst);
}

void ret()
{
	printf("ret");
}

void pushl(char r1)
{
	printf("pushl\t%%%s",getRegister(r1));
}

void popl(char r1)
{
	printf("popl\t%%%s",getRegister(r1));
}

void readb(char r1, char *val)
{
	printf("readb\t%sh(%%%s)",val,getRegister(r1));
}

void readl(char r1, char *val)
{
	printf("readl\t%sh(%%%s)",val,getRegister(r1));
}

void writeb(char r1, char *val)
{
	printf("writeb\t%sh(%%%s)",val,getRegister(r1));
}

void writel(char r1, char *val)
{
	printf("writel\t%sh(%%%s)",val,getRegister(r1));
}

void movsbl(char r1, char r2, char *val)
{
	printf("movsbl\t%sh(%%%s), %%%s",val,getRegister(r2),getRegister(r1));
}

int main(int argc, char **argv)
{
	if(argv[1][0]=='-'&&argv[1][1]=='h')
	{
		printf("Usage: ./y86dis <y86 input file>\n");
		return 0;
	}
	int endFile=1;
	FILE *fp=fopen(argv[1],"r");
	if(fp==NULL)
	{
		printf("ERROR: No Such File\n");
		return 0;
	}
	char buff[10000];
	while(fscanf(fp,"%s",buff)!=EOF)
	{
		if(strcmp(".text",buff)==0)
		{
			endFile=0;
			break;
		}
	}
	if(endFile)
	{
		printf("ERROR: No Instructions Found\n");
		fclose(fp);
		return 0;
	}
	fscanf(fp,"%s",buff);
	fscanf(fp,"%s",buff);
	int ip=0;
	char code[6];
	char *ptr;
	int opcode=0;
	int line=1;
	do
	{
		char bytes[4];
		getNxtCode(ip,buff,code);
		if(strcmp(code,"")==0)
		{
			break;
		}
		printf("%d:\t",line);
		opcode=(int)strtol(code,&ptr,16);
		switch(opcode)
		{
			case 0x00: nop();ip+=2;break;
			case 0x10: halt();ip+=2;break;
			case 0x20: rrmovl(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x21: cmovle(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x22: cmovl(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x23: cmove(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x24: cmovne(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x25: cmovge(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x26: cmovg(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x30: getNxt4Bytes(ip+4,buff,bytes);irmovl(buff[ip+3],bytes);ip+=12;break;
			case 0x40: getNxt4Bytes(ip+4,buff,bytes);rmmovl(buff[ip+2],buff[ip+3],bytes);ip+=12;break;
			case 0x50: getNxt4Bytes(ip+4,buff,bytes);mrmovl(buff[ip+2],buff[ip+3],bytes);ip+=12;break;
			case 0x60: addl(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x61: subl(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x62: andl(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x63: xorl(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x64: mull(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x65: cmpl(buff[ip+2],buff[ip+3]);ip+=4;break;
			case 0x70: getNxt4Bytes(ip+2,buff,bytes);jmp(bytes);ip+=10;break;
			case 0x71: getNxt4Bytes(ip+2,buff,bytes);jle(bytes);ip+=10;break;
			case 0x72: getNxt4Bytes(ip+2,buff,bytes);jl(bytes);ip+=10;break;
			case 0x73: getNxt4Bytes(ip+2,buff,bytes);je(bytes);ip+=10;break;
			case 0x74: getNxt4Bytes(ip+2,buff,bytes);jne(bytes);ip+=10;break;
			case 0x75: getNxt4Bytes(ip+2,buff,bytes);jge(bytes);ip+=10;break;
			case 0x76: getNxt4Bytes(ip+2,buff,bytes);jg(bytes);ip+=10;break;
			case 0x80: getNxt4Bytes(ip+2,buff,bytes);call(bytes);ip+=10;break;
			case 0x90: ret();ip+=2;break;
			case 0xa0: pushl(buff[ip+2]);ip+=4;break;
			case 0xb0: popl(buff[ip+2]);ip+=4;break;
			case 0xc0: getNxt4Bytes(ip+2,buff,bytes);readb(buff[ip+2],bytes);ip+=12;break;
			case 0xc1: getNxt4Bytes(ip+2,buff,bytes);readl(buff[ip+2],bytes);ip+=12;break;
			case 0xd0: getNxt4Bytes(ip+2,buff,bytes);writeb(buff[ip+2],bytes);ip+=12;break;
			case 0xd1: getNxt4Bytes(ip+2,buff,bytes);writel(buff[ip+2],bytes);ip+=12;break;
			case 0xe0: getNxt4Bytes(ip+2,buff,bytes);movsbl(buff[ip+2],buff[ip+3],bytes);ip+=12;break;
			default:printf("ERROR: Invalid Machine Instruction\n");fclose(fp);return 0;
		}
		printf("\n");
		line++;
	}while(1);
	fclose(fp);
	return 0;
}
