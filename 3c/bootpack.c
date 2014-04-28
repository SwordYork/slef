void _io_hlt(void);

void HariMain(void)
{

fin:
	_io_hlt();
	goto fin;

}
