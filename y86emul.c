/* y86emul.c
* y86 emulator
* joseph jaeschke
* December 9, 2016
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

int size=0;
unsigned int of=0;
unsigned int zf=0;
unsigned int sf=0;
unsigned int ip=0;
unsigned int halted=0;
unsigned char *mem;
static volatile int AOK=1;

union byterep
{
	struct fields
	{
		unsigned int lhalf:4;
		unsigned int hhalf:4;
	}halfs;
	unsigned char whole;
}byte;

unsigned int registers[8];

void intHandler(int i)
{
	AOK=0;
	printf("\nAOK\n");
	free(mem);
	exit(1);
}

int validAdr(int adr)
{
	if(adr>=size||adr<0)
	{
		printf("ADR\n");
		free(mem);
		exit(1);
		return 0;
	}
	return adr;
}

/////////////////////////////////////////////////
//////////START OF MACHINE INSTRUCTIONS//////////
/////////////////////////////////////////////////

void halt()
{
	printf("HLT\n");
	halted=1;
	ip++;
	return;
}

void nop()
{
	//printf("nop\n");
	ip++;
	return;
}

void rrmovl()
{
	//printf("rrmovl\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r1=(int)byte.halfs.hhalf;
	int r2=(int)byte.halfs.lhalf;
	registers[r2]=registers[r1];
	ip++;
	return;
}

void irmovl()
{
	//printf("irmovl ");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r=(int)byte.halfs.lhalf;
	ip++;
	int val=((int)*((unsigned int*)&mem[validAdr(ip)]));
	registers[r]=val;
	ip+=4;
	return;
}

void rmmovl()
{
	//printf("rmmovl ");
	ip++;
	int i=0;
	byte.whole=mem[validAdr(ip)];
	ip++;
	int r1=(int)byte.halfs.hhalf;
	int r2=(int)byte.halfs.lhalf;
	int loc=registers[r2]+(int)*((unsigned int*)&mem[validAdr(ip)]);
	int val=registers[r1];
	for(i=3;i>-1;i--)
	{
		char b=val>>24;
		mem[validAdr(loc+i)]=b;
		val=val<<8;
	}
	ip+=4;
	return;
}

void mrmovl()
{
	//printf("mrmovl ");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r1=(int)byte.halfs.hhalf;
	int r2=(int)byte.halfs.lhalf;
	ip++;
	int loc=registers[r2]+(int)*((unsigned int*)&mem[validAdr(ip)]);
	int toreg=(int)*((unsigned int*)&mem[validAdr(loc)]);
	registers[r1]=toreg;
	ip+=4;
	return;
}

void addl()
{
	//printf("addl\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r1=(int)byte.halfs.hhalf;
	int r2=(int)byte.halfs.lhalf;
	int val1=(signed)registers[r1];
	int val2=(signed)registers[r2];
	int sum=val1+val2;
	registers[r2]=(unsigned)sum;
	if((val1>0&&val2>0&&sum<0)||(val1<0&&val2<0&&sum>0))
	{
		of=1;
	}
	else
	{
		of=0;
	}
	if(sum<0)
	{
		sf=1;
	}
	else
	{
		sf=0;
	}
	if(sum==0)
	{
		zf=1;
	}
	else
	{
		zf=0;
	}
	ip++;
	return;
}

void subl()
{
	//printf("subl\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r1=(int)byte.halfs.hhalf;
	int r2=(int)byte.halfs.lhalf;
	int val1=(signed)registers[r1];
	int val2=(signed)registers[r2];
	int diff=val2-val1;
	registers[r2]=(unsigned)diff;
	if((val1<0&&val2>0&&diff<0)||(val1>0&&val2<0&&diff>0))
	{
		of=1;
	}
	else
	{
		of=0;
	}
	if(diff<0)
	{
		sf=1;
	}
	else
	{
		sf=0;
	}
	if(diff==0)
	{
		zf=1;
	}
	else
	{
		zf=0;
	}
	ip++;
	return;
}

void andl()
{
	//printf("andl\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r1=(int)byte.halfs.hhalf;
	int r2=(int)byte.halfs.lhalf;
	int val1=(signed)registers[r1];
	int val2=(signed)registers[r2];
	int ans=val2&val1;
	registers[r2]=(unsigned)ans;
	if(ans==0)
	{
		zf=1;
	}
	else
	{
		zf=0;
	}
	if(ans<0)
	{
		sf=1;
	}
	else
	{
		sf=0;
	}
	of=0;
	ip++;
	return;
}

void xorl()
{
	//printf("xorl\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r1=(int)byte.halfs.hhalf;
	int r2=(int)byte.halfs.lhalf;
	int val1=(signed)registers[r1];
	int val2=(signed)registers[r2];
	int ans=val2^val1;
	registers[r2]=(unsigned)ans;
	if(ans==0)
	{
		zf=1;
	}
	else
	{
		zf=0;
	}
	if(ans<0)
	{
		sf=1;
	}
	else
	{
		sf=0;
	}
	of=0;
	ip++;
	return;
}

void mull()
{
	//printf("mull\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r1=(int)byte.halfs.hhalf;
	int r2=(int)byte.halfs.lhalf;
	int val1=(signed)registers[r1];
	int val2=(signed)registers[r2];
	int ans=val2*val1;
	registers[r2]=(unsigned)ans;
	if((val1>0&&val2>0&&ans<0)||(val1<0&&val2<0&&ans>0))
	{
		of=1;
	}
	else
	{
		of=0;
	}
	if(ans==0)
	{
		zf=1;
	}
	else
	{
		zf=0;
	}
	if(ans<0)
	{
		sf=1;
	}
	else
	{
		sf=0;
	}
	ip++;
	return;
}

void cmpl()
{
	//printf("cmpl\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r1=(int)byte.halfs.hhalf;
	int r2=(int)byte.halfs.lhalf;
	int val1=(signed)registers[r1];
	int val2=(signed)registers[r2];
	int diff=val2-val1;
	if(diff==0)
	{
		zf=1;
	}
	else
	{
		zf=0;
	}
	if(diff<0)
	{
		sf=1;
	}
	else
	{
		sf=0;
	}
	of=0;
	ip++;
	return;
}

void jmp()
{
	//printf("jmp\n");
	ip++;
	ip=(int)*(unsigned int*)&mem[validAdr(ip)];
	return;
}


void jle()
{
	//printf("jle\n");
	if((sf^of)|zf)
	{
		jmp();
	}
	else
	{
		ip+=5;
	}
	return;
}

void jl()
{
	//printf("jl\n");
	if(sf^of)
	{
		jmp();
	}
	else
	{
		ip+=5;
	}
	return;
}

void je()
{
	//printf("je\n");
	if(zf)
	{
		jmp();
	}
	else
	{
		ip+=5;
	}
	return;
}

void jne()
{
	//printf("jne\n");
	if(!zf)
	{
		jmp();
	}
	else
	{
		ip+=5;
	}
	return;
}

void jge()
{
	//printf("jge\n");
	if(!(sf^of))
	{
		jmp();
	}
	else
	{
		ip+=5;
	}
	return;
}

void jg()
{
	//printf("jg\n");
	if(!(sf^of)&!zf)
	{
		jmp();
	}
	else
	{
		ip+=5;
	}
	return;
}

void call()
{
	//printf("call\n");
	ip++;
	int i=0;
	int dest=(int)*(unsigned int*)&mem[validAdr(ip)];
	ip+=4;
	int val=ip;
	for(i=0;i<4;i++)
	{
		registers[4]--;
		char b=val>>24;
		mem[validAdr(registers[4])]=b;
		val=val<<8;
	}
	ip=dest;
	return;
}

void ret()
{
	//printf("ret\n");
	ip=(int)*((unsigned int*)&(mem[validAdr(registers[4])]));
	registers[4]+=4;
	return;
}

void pushl()
{
	//printf("pushl\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r1=(int)byte.halfs.hhalf;
	int val=(signed)registers[r1];
	int i=0;
	for(i=0;i<4;i++)
	{
		registers[4]--;
		char b=val>>24;
		mem[validAdr(registers[4])]=b;
		val=val<<8;
	}
	ip++;
	return;
}

void popl()
{
	//printf("popl\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r1=(int)byte.halfs.hhalf;
	int val=(int)*(unsigned int*)&(mem[validAdr(registers[4])]);
	registers[4]+=4;
	registers[r1]=(unsigned)val;
	ip++;
	return;
}

void readb()
{
	//printf("readb\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r1=(int)byte.halfs.hhalf;
	unsigned char c;
	scanf("%c",&c);
	ip++;
	int memloc=((int)*(unsigned int*)&mem[validAdr(ip)])+registers[r1];
	mem[validAdr(memloc)]=(unsigned)c;
	ip+=4;
	if(c!=EOF)
	{
		zf=0;
	}
	else
	{
		zf=1;
	}
	return;
}

void readl()
{
	//printf("readl\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int r1=(int)byte.halfs.hhalf;
	int input;
	scanf("%d",&input);
	ip++;
	int i=0;
	int disp=(int)*(unsigned int*)&(mem[validAdr(ip)]);
	int memloc=disp+registers[r1];
	for(i=3;i>-1;i--)
	{
		//printf("bit shift loop ");
		char b=input>>24;
		mem[validAdr(memloc+i)]=(unsigned)b;
		input=input<<8;
	}
	ip+=4;
	if(input!=EOF)
	{
		zf=0;
	}
	else
	{
		zf=1;
	}
	return;
}

void writeb()
{
	//printf("writeb\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int loc=(int)registers[byte.halfs.hhalf];
	ip++;
	int memloc=loc+(int)*(unsigned int*)&mem[validAdr(ip)];
	printf("%c",(signed char)mem[validAdr(memloc)]);
	ip+=4;
	return;
}

void writel()
{
	//printf("writel\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int loc=(int)registers[byte.halfs.hhalf];
	ip++;
	int memloc=loc+(int)*(unsigned int*)&mem[validAdr(ip)];
	int toprint=(int)(*((unsigned int*)&(mem[validAdr(memloc)])));
	printf("%d",toprint);
	ip+=4;
	return;
}

void movsbl()
{
	//printf("movsbl\n");
	ip++;
	byte.whole=mem[validAdr(ip)];
	int to=(int)registers[byte.halfs.hhalf];
	int loc=(int)registers[byte.halfs.lhalf];
	ip++;
	int memloc=loc+(int)*(unsigned int*)&mem[validAdr(ip)];
	unsigned int sb=mem[validAdr(memloc)];
	byte.whole=sb;
	if(sb>=0x80)
	{
		registers[to]=(286331136+sb);
	}
	else
	{
		registers[to]=sb;
	}
	ip+=4;
	return;
}

///////////////////////////////////////////////
//////////END OF MACHINE INSTRUCTIONS//////////
///////////////////////////////////////////////

int main(int argc, char **argv)
{
	if(argv[1][0]=='-'&&argv[1][1]=='h')
	{
		printf("Usage: ./y86emul <.y86 program>\n");
		return 0;
	}
	//Loading file into memory
	FILE *fp=fopen(argv[1],"r");
	char buff[10000];
	if(fp==NULL)
	{
		printf("ERROR: No Such File\n");
		return 0;
	}
	fscanf(fp,"%s",buff);
	if(strcmp(".size",buff)==0)
	{
		fscanf(fp,"%s",buff);
		size = (int)(strtol(buff,NULL,16));
		mem = malloc(size);
		int i;
		for(i=0;i<size;i++)
		{
			mem[i]=0;
		}
	}
	else
	{
		printf("ERROR: Size Must be Specified First\n");
		fclose(fp);
		free(mem);
		return 0;
	}
	while(fscanf(fp,"%s",buff)!=EOF)
	{
		if(strcmp(".text",buff)==0)
		{
			fscanf(fp,"%s",buff);
			ip=(int)strtol(buff,NULL,16);
			fscanf(fp,"%s",buff);
			int i;
			int j=0;
			for(i=0;i<strlen(buff);i=i+2)
			{
				char tmp[2];
				tmp[0]=buff[i];
				tmp[1]=buff[i+1];
				mem[ip+j]=strtol(tmp,NULL,16);
				j++;
			}
		}
		else if(strcmp(".byte",buff)==0)
		{
			fscanf(fp,"%s",buff);
			int location=strtol(buff,NULL,16);
			fscanf(fp,"%s",buff);
			char tmp[2];
			tmp[0]=buff[0];
			tmp[1]=buff[1];
			mem[location]=strtol(tmp,NULL,16);
		}
		else if(strcmp(".long",buff)==0)
		{
			fscanf(fp,"%s",buff);
			int location=strtol(buff,NULL,16);
			fscanf(fp,"%s",buff);
			int i;
			for(i=0;i<strlen(buff);i=i+2)
			{
				char tmp[2];
				tmp[0]=buff[i];
				tmp[1]=buff[i+1];
				mem[location]=strtol(tmp,NULL,16);
				location++;
			}
		}
		else if(strcmp(".string",buff)==0)
		{
			fscanf(fp,"%s",buff);
			int location=strtol(buff,NULL,16);
			char c=fgetc(fp);
			c=fgetc(fp);
			c=fgetc(fp);
			int i=0;
			while(c!='\"')
			{
				buff[i]=c;
				c=fgetc(fp);
				i++;
			}
			buff[i+1]='\0';
			for(i=0;i<strlen(buff);i++)
			{
				mem[location]=buff[i];
				location++;
			}
		}
		else
		{
			printf("ERROR: Bad directive\n");
			fclose(fp);
			free(mem);
			return 0;
		}
	}
	int i=0;
	//Running machine instructions
	for(i=0;i<8;i++)
	{
		registers[i]=0;
	}
	unsigned char opcode;
	do
	{
		signal(SIGINT,intHandler);
		validAdr(ip);
		opcode=mem[ip];
		switch(opcode)
		{
			case 0x00: nop();break;
			case 0x10: halt();break;
			case 0x20: rrmovl();break;
			case 0x30: irmovl();break;
			case 0x40: rmmovl();break;
			case 0x50: mrmovl();break;
			case 0x60: addl();break;
			case 0x61: subl();break;
			case 0x62: andl();break;
			case 0x63: xorl();break;
			case 0x64: mull();break;
			case 0x65: cmpl();break;
			case 0x70: jmp();break;
			case 0x71: jle();break;
			case 0x72: jl();break;
			case 0x73: je();break;
			case 0x74: jne();break;
			case 0x75: jge();break;
			case 0x76: jg();break;
			case 0x80: call();break;
			case 0x90: ret();break;
			case 0xa0: pushl();break;
			case 0xb0: popl();break;
			case 0xc0: readb();break;
			case 0xc1: readl();break;
			case 0xd0: writeb();break;
			case 0xd1: writel();break;
			case 0xe0: movsbl();break;
			default:printf("INS\n");free(mem);fclose(fp);return 0;
		}
		validAdr(ip);
	}while(halted!=1&&AOK);
	free(mem);
	fclose(fp);
	return 0;
}
