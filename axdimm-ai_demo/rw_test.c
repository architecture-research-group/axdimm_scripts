#include <linux/io.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/atomic.h>
//#include <sys/time.h>

/*memcpy_avx */
#define _MM_MALLOC_H_INCLUDED 1

#include <x86intrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <asm/fpu/api.h>

MODULE_LICENSE("GPL");

static ulong offset = 0x100000000;
module_param(offset, ulong, 0644);

static ulong size = 0x1000000000;
module_param(size, ulong, 0644);

static ulong loc =  0x0000000000;

static uint test = 0;
module_param(test, uint, 0644);

static uint data = 1;
module_param(data, uint,  0644);

char t_2;

volatile void *addr;
volatile void *addr0;
volatile void *addr0_psum;
volatile void *addr1;
volatile void *addr1_psum;
atomic_t t = ATOMIC_INIT(0);


volatile void *psum_addr;
static uint reg_data[16];

static uint NMPInst[1024][16];

void memcpy_avx (void *dest, void *src, size_t size)
{
        volatile __m256i a, b;
        uint64_t i;

        for (i=0; i < size; i+=64) {
                //a =  _mm256_load_ps((float const *)(tmp + i));
                //b =  _mm256_load_ps((float const *)(tmp + i + 32));

                //kernel_fpu_begin();
				_mm_mfence();
                a =  _mm256_stream_load_si256 ((__m256i const*)(src + i));
                b =  _mm256_stream_load_si256 ((__m256i const*)(src + i + 32));

                _mm_lfence();
                //_mm256_store_ps((float *)(db[1] + i), a);
                //_mm256_store_ps((float *)(db[1] + i + 32), b);
                _mm256_stream_si256 ((__m256i *)(dest + i), a);
                _mm256_stream_si256 ((__m256i *)(dest + i + 32), b);
                _mm_sfence();

                //kernel_fpu_end();

                // _mm_clflush ((void const*)(src + i));
                // _mm_clflush ((void const*)(src + i + 32));
                // _mm_clflush ((void const*)(dest + i));
                // _mm_clflush ((void const*)(dest + i + 32));
        }
}

void ax_single(void)
{
	int num_of_inst_per_rank;
	int breakcount;

	ulong i;


	mb();memcpy((void *)(addr + 256 + 0x2800000000), (void *)(&data),1);

	printk("BufferCopy Done=============================\n");

	//CONF_SYSTEM_EN SET
	reg_data[0] = 0xcafe;
	memcpy((void *)(addr + 0x3E1000000), (void *)reg_data, 64);
	memcpy((void *)(addr + 0x7E1000000), (void *)reg_data, 64);

	//WR_INST
	num_of_inst_per_rank = sizeof(NMPInst) / 8;

	//getnstimeofday(&start);
	if (num_of_inst_per_rank <= 8)
	{
		   memcpy((void *)(addr + 0x7C0000000), (void *)NMPInst[0], 64);
		   memcpy((void *)(addr + 0xFC0000000), (void *)NMPInst[0], 64);
	}
	else
	{
		   for (i = 0; i < (num_of_inst_per_rank / 16); i += 1)
		   {
				   memcpy((void *)(addr + i * 256 + 0x3E0000000), (void *)NMPInst[2 * i], 64);
				   memcpy((void *)(addr + i * 256 + 0x3E0000040), (void *)NMPInst[2 * i + 1], 64);
				   memcpy((void *)(addr + i * 256 + 0x7E0000000), (void *)NMPInst[2 * i], 64);
				   memcpy((void *)(addr + i * 256 + 0x7E0000040), (void *)NMPInst[2 * i + 1], 64);
		   }
	}


	//are these addresses of registers?
	printk("NMPInst copies Done=============================\n");
	memcpy((void *)(addr + 0x3E1000040), (void *)reg_data, 64);
	memcpy((void *)(addr + 0x7E1000040), (void *)reg_data, 64);
	
    udelay(1);
	breakcount = 0;


	//SLS_EXEC UNSET
	reg_data[0] = 0x0;
	memcpy((void *)(addr + 0x3E1000040), (void *)reg_data, 64);
	memcpy((void *)(addr + 0x7E1000040), (void *)reg_data, 64);
	
	//CONF_SYSTEM_EN_UNSET
	reg_data[0] = 0x0;
	memcpy((void *)(addr + 0x3E1000000), (void *)reg_data, 64);
	memcpy((void *)(addr + 0x7E1000000), (void *)reg_data, 64);

	printk("At copied loc: 0x%x\n", *(unsigned*)(addr + 256 + 0x280000000) );
}

void ax_read(ulong loc)
{
	printk("ax_read\n");
	printk("data at %ld: %lx\n", loc, *(unsigned long*)(addr + 256 + loc));
}

static int mem_init(void)
{
	printk("MEM INIT");
	switch (test)
	{
	case 0:
	case 1:
		//addr = memremap(0x100000000, 0x1000000000, MEMREMAP_WC);
		addr = memremap(0x100000000, 0x800000000, MEMREMAP_WC);
		break;
	default:
		addr = memremap(0x100000000, 0x1000000000, MEMREMAP_WC);
		break;
	}

	return 0;
}

static void mem_exit(void)
{
	t_2=(char)255;
	switch (test)
	{
	case 0:
		ax_single();
		break;
	case 1:
		ax_read(loc);
		break;
	default:
		printk("\n\n\nWrong Test Case\n\n\n");
		break;
	}
	memunmap((void *)addr);
	printk("Module exit\n");
}

module_init(mem_init);
module_exit(mem_exit);
