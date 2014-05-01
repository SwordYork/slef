void _io_hlt();
void _write_mem8(int addr, int data);

void main(){
	//char* video_memory = (char*) 0xa0000;
	//	*video_memory = 'X';
	int i;
	char *p;		// char* p,q <===> char *p,q
	p = (char *) 0xa0000;
//	for (i = 0xa0000;i<0xaffff;i++){
	for (i = 0; i <= 0xffff; i++){
//		p = (char *) i;			
//	    *(p + i) = i & 0x1f;
	    i[p] = i & 0x2f;		//2[p] all ok!!!!!!!!!!!!!!
//		p[i] = i & 0x2f;
		//_write_mem8(i,i & 0x0f);
	}
	for(;;){
		_io_hlt();
	}
}	
