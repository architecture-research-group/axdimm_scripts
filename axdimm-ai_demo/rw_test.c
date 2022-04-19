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

static uint test = 0;
module_param(test, uint, 0644);

static uint nproc = 4;
module_param(nproc, uint, 0644);

static uint data[16];
module_param_array(data, uint, NULL, 0644);

volatile void *addr;
volatile void *addr0;
volatile void *addr0_psum;
volatile void *addr1;
volatile void *addr1_psum;
atomic_t t = ATOMIC_INIT(0);

static uint polldata0[16];
static uint polldata1[16];

static uint readdata0[16];
static uint readdata1[16];
static uint readdata2[16];
static uint readdata3[16];
static uint readdata4[16];
static uint readdata5[16];
static uint readdata6[16];
static uint readdata7[16];

volatile void *psum_addr;
static uint reg_data[16];

static uint Buffer[1024][16];
static uint EmbTable[1024][16];
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

	/*
	for (i = 0; i < 0x200; i += 1)
	{
		mb();memcpy((void *)(addr + i * 256 + 0x000000000), (void *)EmbTable[2 * i%0x200], 64);
		mb();memcpy((void *)(addr + i * 256 + 0x400000000), (void *)EmbTable[2 * i%0x200], 64);
		mb();memcpy((void *)(addr + i * 256 + 0x000000040), (void *)EmbTable[2 * i%0x200 + 1], 64);
		mb();memcpy((void *)(addr + i * 256 + 0x400000040), (void *)EmbTable[2 * i%0x200 + 1], 64);
	}
	*/
	//mb();memcpy((void *)(addr + 256 + 0x000000000), (void *)Buffer,64);
	mb();memcpy((void *)(addr + 256 + 0x000000000), (void *)data,16);
	mb();memcpy((void *)(addr + 256 + 0x000000000), (void *)data,16);

	printk("BufferCopy Done=============================\n");

	//CONF_SYSTEM_EN SET
	reg_data[0] = 0xcafe;
	memcpy((void *)(addr + 0x3E1000000), (void *)reg_data, 64);
	memcpy((void *)(addr + 0x7E1000000), (void *)reg_data, 64);

	//WR_INST
	num_of_inst_per_rank = sizeof(NMPInst) / 8;

	//getnstimeofday(&start);

	//what is this?
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

	//getnstimeofday(&endinst);

	//are these addresses of registers?
	printk("NMPInst copies Done=============================\n");
	memcpy((void *)(addr + 0x3E1000040), (void *)reg_data, 64);
	memcpy((void *)(addr + 0x7E1000040), (void *)reg_data, 64);
	
    udelay(1);
	breakcount = 0;
	/*
	do
	{
		breakcount++;
		mb();memcpy((void *)polldata0, (void *)(addr + 0x3E1000440), sizeof(polldata0));
		printk("Rank0.PC             (0x%08x)\n", polldata0[0]);
		printk("Rank0.PSUM_POOL_DONE (0x%08x)\n", polldata0[1]);
		if (breakcount > 100)
		{
			printk("Rank0.Polling Failed");
			break;
		}
	} while (polldata0[1] != 0xffffffff);
	*/

	/*
	breakcount = 0;
	do
	{
		breakcount++;
		mb();memcpy((void *)polldata1, (void *)(addr + 0x7E1000440), sizeof(polldata1));
		printk("Rank1.PC             (0x%08x)\n", polldata1[0]);
		printk("Rank1.PSUM_POOL_DONE (0x%08x)\n", polldata1[1]);
		if (breakcount > 100)
		{
			printk("Rank1.Polling Failed");
			break;
		}
	} while (polldata1[1] != 0xffffffff);
	*/

	//getnstimeofday(&endsls);

	/*
	if (breakcount <= 100)
	{
		memcpy((void *)readdata0, (void *)(addr + 0x3E4020000), sizeof(readdata0));
		memcpy((void *)readdata1, (void *)(addr + 0x3E4020040), sizeof(readdata1));
		memcpy((void *)readdata2, (void *)(addr + 0x7E4020000), sizeof(readdata2));
		memcpy((void *)readdata3, (void *)(addr + 0x7E4020040), sizeof(readdata3));

		//getnstimeofday(&end);
		
		printk("Rank0.PSUM= 0x%08x\n", readdata0[0]);
		printk("Rank0.PSUM= 0x%08x\n", readdata1[0]);

		printk("Rank1.PSUM= 0x%08x\n", readdata2[0]);
		printk("Rank1.PSUM= 0x%08x\n", readdata3[0]);

	}
	*/

	//SLS_EXEC UNSET
	reg_data[0] = 0x0;
	memcpy((void *)(addr + 0x3E1000040), (void *)reg_data, 64);
	memcpy((void *)(addr + 0x7E1000040), (void *)reg_data, 64);
	
	//CONF_SYSTEM_EN_UNSET
	reg_data[0] = 0x0;
	memcpy((void *)(addr + 0x3E1000000), (void *)reg_data, 64);
	memcpy((void *)(addr + 0x7E1000000), (void *)reg_data, 64);
}

void ax_perf_test(void)
{
	int num_of_inst_per_rank;
	int breakcount;
	ulong i;

	for (i = 0; i < 0x200; i += 1)
	{
		mb();memcpy((void *)(addr + i * 256 + 0x000000000), (void *)EmbTable[2 * i%0x200], 64);
		mb();memcpy((void *)(addr + i * 256 + 0x800000000), (void *)EmbTable[2 * i%0x200], 64);
		mb();memcpy((void *)(addr + i * 256 + 0x000000040), (void *)EmbTable[2 * i%0x200 + 1], 64);
		mb();memcpy((void *)(addr + i * 256 + 0x800000040), (void *)EmbTable[2 * i%0x200 + 1], 64);
		mb();memcpy((void *)(addr + i * 256 + 0x000000080), (void *)EmbTable[2 * i%0x200], 64);
		mb();memcpy((void *)(addr + i * 256 + 0x800000080), (void *)EmbTable[2 * i%0x200], 64);
		mb();memcpy((void *)(addr + i * 256 + 0x0000000c0), (void *)EmbTable[2 * i%0x200 + 1], 64);
		mb();memcpy((void *)(addr + i * 256 + 0x8000000c0), (void *)EmbTable[2 * i%0x200 + 1], 64);
	}

	printk("wr_EmbTable Done=============================\n");

	//CONF_SYSTEM_EN SET
	reg_data[0] = 0xcafe;
	memcpy((void *)(addr + 0x7C2000000), (void *)reg_data, 64);
	memcpy((void *)(addr + 0xFC2000000), (void *)reg_data, 64);
	memcpy((void *)(addr + 0x7C2000080), (void *)reg_data, 64);
	memcpy((void *)(addr + 0xFC2000080), (void *)reg_data, 64);

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
			memcpy((void *)(addr + i * 256 + 0x7C0000000), (void *)NMPInst[2 * i], 64);
			memcpy((void *)(addr + i * 256 + 0x7C0000040), (void *)NMPInst[2 * i + 1], 64);
			memcpy((void *)(addr + i * 256 + 0xFC0000000), (void *)NMPInst[2 * i], 64);
			memcpy((void *)(addr + i * 256 + 0xFC0000040), (void *)NMPInst[2 * i + 1], 64);
			memcpy((void *)(addr + i * 256 + 0x7C0000080), (void *)NMPInst[2 * i], 64);
			memcpy((void *)(addr + i * 256 + 0x7C00000c0), (void *)NMPInst[2 * i + 1], 64);
			memcpy((void *)(addr + i * 256 + 0xFC0000080), (void *)NMPInst[2 * i], 64);
			memcpy((void *)(addr + i * 256 + 0xFC00000c0), (void *)NMPInst[2 * i + 1], 64);
		}
	}

	//getnstimeofday(&endinst);

	printk("wr_InstBuf Done=============================\n");
	memcpy((void *)(addr + 0x7C2000040), (void *)reg_data, 64);
	memcpy((void *)(addr + 0xFC2000040), (void *)reg_data, 64);
	memcpy((void *)(addr + 0x7C20000c0), (void *)reg_data, 64);
	memcpy((void *)(addr + 0xFC20000c0), (void *)reg_data, 64);

    udelay(1);
	breakcount = 0;
	do
	{
		breakcount++;
		memcpy((void *)polldata0, (void *)(addr + 0x7C2000840), sizeof(polldata0));
		printk("Rank0.PC             (0x%08x)\n", polldata0[0]);
		printk("Rank0.PSUM_POOL_DONE (0x%08x)\n", polldata0[1]);
		if (breakcount > 100)
		{
			printk("Rank0.Polling Failed");
			break;
		}
	} while (polldata0[1] != 0xffffffff);

	breakcount = 0;
	do
	{
		breakcount++;
		memcpy((void *)polldata1, (void *)(addr + 0xFC2000840), sizeof(polldata1));
		printk("Rank1.PC             (0x%08x)\n", polldata1[0]);
		printk("Rank1.PSUM_POOL_DONE (0x%08x)\n", polldata1[1]);
		if (breakcount > 100)
		{
			printk("Rank1.Polling Failed");
			break;
		}
	} while (polldata1[1] != 0xffffffff);

	breakcount = 0;
	do
	{
		breakcount++;
		memcpy((void *)polldata0, (void *)(addr + 0x7C20008c0), sizeof(polldata0));
		printk("Rank2.PC             (0x%08x)\n", polldata0[0]);
		printk("Rank2.PSUM_POOL_DONE (0x%08x)\n", polldata0[1]);
		if (breakcount > 100)
		{
			printk("Rank0.Polling Failed");
			break;
		}
	} while (polldata0[1] != 0xffffffff);

	breakcount = 0;
	do
	{
		breakcount++;
		memcpy((void *)polldata1, (void *)(addr + 0xFC20008c0), sizeof(polldata1));
		printk("Rank3.PC             (0x%08x)\n", polldata1[0]);
		printk("Rank3.PSUM_POOL_DONE (0x%08x)\n", polldata1[1]);
		if (breakcount > 100)
		{
			printk("Rank1.Polling Failed");
			break;
		}
	} while (polldata1[1] != 0xffffffff);

	//getnstimeofday(&endsls);

	if (breakcount <= 100)
	{
		

		memcpy((void *)readdata0, (void *)(addr + 0x7C8040000), sizeof(readdata0));
		memcpy((void *)readdata1, (void *)(addr + 0x7C8040040), sizeof(readdata1));
		memcpy((void *)readdata2, (void *)(addr + 0xFC8040000), sizeof(readdata2));
		memcpy((void *)readdata3, (void *)(addr + 0xFC8040040), sizeof(readdata3));

		memcpy((void *)readdata4, (void *)(addr + 0x7C8040080), sizeof(readdata4));
		memcpy((void *)readdata5, (void *)(addr + 0x7C80400c0), sizeof(readdata5));
		memcpy((void *)readdata6, (void *)(addr + 0xFC8040080), sizeof(readdata6));
		memcpy((void *)readdata7, (void *)(addr + 0xFC80400c0), sizeof(readdata7));

		//getnstimeofday(&end);
		
		printk("Rank0.PSUM= 0x%08x\n", readdata0[0]);
		printk("Rank0.PSUM= 0x%08x\n", readdata1[0]);

		printk("Rank1.PSUM= 0x%08x\n", readdata2[0]);
		printk("Rank1.PSUM= 0x%08x\n", readdata3[0]);

		printk("Rank2.PSUM= 0x%08x\n", readdata4[0]);
		printk("Rank2.PSUM= 0x%08x\n", readdata5[0]);

		printk("Rank3.PSUM= 0x%08x\n", readdata6[0]);
		printk("Rank3.PSUM= 0x%08x\n", readdata7[0]);
	}

	//SLS_EXEC UNSET
	reg_data[0] = 0x0;
	memcpy((void *)(addr + 0x7C2000040), (void *)reg_data, 64);
	memcpy((void *)(addr + 0xFC2000040), (void *)reg_data, 64);
	memcpy((void *)(addr + 0x7C20000c0), (void *)reg_data, 64);
	memcpy((void *)(addr + 0xFC20000c0), (void *)reg_data, 64);

	//CONF_SYSTEM_EN_UNSET
	reg_data[0] = 0x0;
	memcpy((void *)(addr + 0x7C2000000), (void *)reg_data, 64);
	memcpy((void *)(addr + 0xFC2000000), (void *)reg_data, 64);
	memcpy((void *)(addr + 0x7C2000080), (void *)reg_data, 64);
	memcpy((void *)(addr + 0xFC2000080), (void *)reg_data, 64);
}

static int mem_init(void)
{
	uint i=0;
	uint j=0;
	printk("MEM INIT");

	for (i=0; i<1024; i++)
		for (j=0; j<16; j++)
			Buffer[i][j] = 0xcccccccc;
	/*

	for (i=0; i<128; i++)
	{
		for (j=0; j<4; j++)
		{
			NMPInst[i][4*j] = 0x0;
			NMPInst[i][4*j+1] = 0x0;
			NMPInst[i][4*j+2] = 0x0;
			NMPInst[i][4*j+3] = 0x20000;
		}
	}
	NMPInst[127][3]=0x30000;
	*/

	switch (test)
	{
	case 0:
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
	switch (test)
	{
	case 0:
		ax_single();
		break;
	case 1:
		ax_perf_test();
		break;
	default:
		printk("\n\n\nWrong Test Case\n\n\n");
		break;
	}
	printk("At copied loc: 0x%.8x\n", *(uint*)(addr + 256 + 0x000000000) );
	memunmap((void *)addr);
	printk("Module exit\n");
}

module_init(mem_init);
module_exit(mem_exit);
