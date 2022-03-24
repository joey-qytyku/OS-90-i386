void kernel_main(void)
{
	char *msg = "Hello, VGA world";
	for (int i=0;msg[i]!=0;*(char*)(0xB8000+i)=msg[i],i+=2);
}
