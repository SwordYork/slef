#include "kernel.h"

void memman_init(struct MEMMAN *man)
{
	man->frees = 0;		//  total info	
	man->maxfrees = 0;	//  free maxfrees
	man->lostsize = 0;	//	failure size
	man->losts = 0;		//	release failures
	return;
}

unsigned int memman_total(struct MEMMAN *man)
{
	// total free size
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
	// allocation
	unsigned int i, a;
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) { // big enough
			a = man->free[i].addr;
			man->free[i].addr += size;	// update
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				man->frees--;				// delete block
				for (; i < man->frees; i++) { // next
					man->free[i] = man->free[i + 1]; 
				}
			}
			return a;
		}
	}
	return 0; 
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i, j;
	// find addr 
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			man->free[i - 1].size += size;
			if (i < man->frees) {
				if (addr + size == man->free[i].addr) {
					man->free[i - 1].size += man->free[i].size;
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1];
					}
				}
			}
			return 0;
		}
	}
	if (i < man->frees) {
		if (addr + size == man->free[i].addr) {
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0;
		}
	}
	if (man->frees < MEMMAN_FREES) {
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; 
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0;
	}
	man->losts++;
	man->lostsize += size;
	return -1;
}

unsigned int memtest(unsigned int start, unsigned int end){
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* detect 486 */
	eflg = _io_load_eflags();
	eflg |= EFLAGS_AC_BIT;
	_io_store_eflags(eflg);
	eflg = _io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0){
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; // AC-bit = 0
	_io_store_eflags(eflg);

	if (flg486 != 0){
		cr0 = _load_cr0();
		cr0 |= CR0_CACHE_DISABLE; // disable cache
		_store_cr0(cr0);
	}

	i = _memtest_sub(start,end);

	if(flg486 != 0){
		cr0 = _load_cr0();
		cr0 &= ~ CR0_CACHE_DISABLE;  // enable cache
		_store_cr0(cr0);
	}

	return i;

}

unsigned int memtest_sub(unsigned int start, unsigned int end){
	unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;

	for(i = start;i<=end;i+= 0x1000){
		p = (unsigned int *) (i + 0xffc);  // test last 4 bytes
		old = *p;
		*p = pat0;
		*p ^= 0xffffffff;
		if( *p != pat1 ){
not_memory:
			*p = old;
			break;
		}
		*p ^= 0xffffffff;

		if (*p != pat0){
			goto not_memory;
		}
		*p = old;
	}
	return i;
}

unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size){
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000; // round up to 4k
	a = memman_alloc(man, size);
	return a;
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size){
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memman_free(man, addr, size);
	return i;
}
