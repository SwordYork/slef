#include "kernel.h" 

/*
 *	n >= 0
 */
void itoa(char *s,int n, int max_length){
	if (n == 0){
		s[0] = '0';
		s[1] = '\0';
	}

	int pos = 0;
	int m = n;
	while(m != 0){
		++ pos;
		m /= 10;
	}

	if(pos >= max_length){
		s[0] = '\0';
		return;
	}

	s[pos] = '\0';
	--pos;
	while( n != 0){
		s[pos--] = '0' + ( n % 10);
		n /= 10;
	}

	return;
}



void char2hex(char *s,char c, int max_length){
	s[0] = '0' + (int) ((c & 0xf0) >> 4 );
	s[1] = '0' + (int) (c & 0x0f);

	if(s[0] > 57)
		s[0] += 7;
	if(s[1] > 57)
		s[1] += 7;

	s[2] = '\0';

	return;
}
 
