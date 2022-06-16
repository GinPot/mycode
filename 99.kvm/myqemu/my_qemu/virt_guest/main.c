
#define OUT_PORT		0x8000
#define IN_PORT			0x8004



#define read_sysreg(r) ({					\
	unsigned long __val;						\
	asm volatile("mrs %0, " #r : "=r" (__val));	\
	__val;							\
})

void print(const char *buff)
{
	while(buff && *buff)
		*(unsigned char *)OUT_PORT = *buff++;
}

char getchar(void)
{
	return *(char *)IN_PORT;
}

int main(void)
{
	char ch[2];
	char i = read_sysreg(CurrentEL);
	
	print("CurrentEL=");
	print(&i);

	
	print("\nHello world.\n");
	
	ch[0] = getchar();
	ch[1] = '\n';
	
	print("Get from host: ");
	print(ch);


	return 0;
}