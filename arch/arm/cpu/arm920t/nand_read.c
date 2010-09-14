/* 
 * vivi/s3c2410/nand_read.c: Simple NAND read functions for booting from NAND
 *
 * Copyright (C) 2002 MIZI Research, Inc.
 *
 * Author: Hwang, Chideok <hwang@mizi.com>
 * Date  : $Date: 2002/08/14 10:26:47 $
 *
 * $Revision: 1.6 $
 * $Id: param.c,v 1.9 2002/07/11 06:17:20 nandy Exp 
 *
 * History
 *
 * 2002-05-xx: Hwang, Chideok <hwang@mizi.com>
 *    - 될거라고 난디에게 줌.
 *
 * 2002-05-xx: Chan Gyun Jeong <cgjeong@mizi.com>
 *    - 난디의 부탁을 받고 제대로 동작하도록 수정.
 *
 * 2002-08-10: Yong-iL Joh <tolkien@mizi.com>
 *    - SECTOR_SIZE가 512인 놈은 다 읽도록 수정
 *
 */

//#include <config.h>

#define __REGb(x)	(*(volatile unsigned char *)(x))
#define __REGw(x) (*(volatile unsigned short *)(x)) 
#define __REGi(x)	(*(volatile unsigned int *)(x))
#define NF_BASE		0x4e000000
#define NFCONF		__REGi(NF_BASE + 0x0)
#define NFCMD		__REGb(NF_BASE + 0x4)
#define NFADDR		__REGb(NF_BASE + 0x8)
#define NFDATA		__REGb(NF_BASE + 0xc)
#define NFSTAT		__REGb(NF_BASE + 0x10)

#define NFDATA16 		__REGw(NF_BASE + 0x10) 

#define BUSY 1
inline void wait_idle(void) {
    int i;

    while(!(NFSTAT & BUSY))
      for(i=0; i<10; i++);
}

#define NAND_SECTOR_SIZE	512
#define NAND_BLOCK_MASK		(NAND_SECTOR_SIZE - 1)

/* low level nand read function */
int
nand_read_ll(unsigned char *buf, unsigned long start_addr, int size)
{
    int i, j;
	unsigned int page_num;
	unsigned short *ptr16 = (unsigned short *)buf; 

	
    if ((start_addr & NAND_BLOCK_MASK) || (size & NAND_BLOCK_MASK)) {
        return -1;	/* invalid alignment */
    }

    /* chip Enable */
    NFCONF &= ~0x800;
    for(i=0; i<10; i++);

    for(i=start_addr; i < (start_addr + size);) {

	page_num = i >> 11;
      /* READ0 */
      NFCMD = 0;

      /* Write Address */
      NFADDR = 0;
	NFADDR = 0;
	NFADDR = page_num & 0xff;
	NFADDR = (i >> 11) & 0xff;
	NFADDR = (i >> 19) & 0xff;

	NFCMD = 0x30;

      wait_idle();

      for (i = 0; i < (2048>>1); i++) 
	{ 
		*ptr16 = NFDATA16; 
		ptr16++; 
	}
    }

    /* chip Disable */
    NFCONF |= 0x800;	/* chip disable */

    return 0;
}

