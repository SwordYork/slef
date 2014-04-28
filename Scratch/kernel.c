void _io_hlt();
void _write_mem8(int addr, int data);

void main(){
	//char* video_memory = (char*) 0xa0000;
	//	*video_memory = 'X';
	int i;
	for (i = 0xa0000;i<0xaffff;i++){
		_write_mem8(i,i & 0x0f);
	}
	for(;;){
		_io_hlt();
	}
}	
