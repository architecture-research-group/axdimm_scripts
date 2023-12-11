#### Generate Sequence of RdCAS to Physical Addresses with a Predefined Stride
#### Then
#### Generate a Sequence of WrCAS to Physical Addresses with the Same Physical Addresses

```
make
./run.sh
```

Output:
```
n869p538@axdimm:issue_rdcas_wrcas$ ./run.sh 
```

/*
128 Src_buf <- 512 * 1024 RdCAS

128 Dst_buf <- 512 * 1024 WrCAS

128 CompCpy(Src_buf_i, Dst_buf_i, 32kB);

CompCpy(Src_buf_i, Src_buf_i, 32kB);

RdCAS: 0x3<A1><A2><U1><U2><U3><U4><U5><U6>
WrCAS: 0x3<A1><A2><U1><U2><U3><U4><U5><U6>

Base: 0x10000000

src_page_0: 0x300000000 <- 512 RdCAS 
dst_page_0: 0x380000000 <- 512 WrCAS 
...
src_page_127: 0x37F000000
dst_page_127: 0x3FF000000 <- WrCAS

0x300 <-RdCAS
0x37F <-RdCAS 

0x380 <-WrCAS
0x3FF <- WrCAS

&Src_buf_i = 0x3<A1><A2><U1><U2><U3><U4><U5><U6>
&Dst_buf_i = 0x3<A1><A2><U1><U2><U3><U4><U5><U6>

32KB Src_buf :  | b_0 | ... |b_32767| <- RdCAS
32KB Dst_buf :  | b_0 | ... |b_32767| <- WrCAS

4MB <- Src Address Space, Dst Address Space

8MB

*/


#### Allocation Issues:

(1)

#define DSTBUF_START 0x380000000
MMAPing 0x380000000
ret: -1
Error:: Invalid argument

----
#define BUFSIZE 32 * 1024 // 32KB Pages
#define RANGESIZE (1<<22)

#define SRCBUF_START 0x300000000
#define DSTBUF_START 0x3F0000000
----

(2)
#define BUFSIZE 32 * 1024 // 32KB Pages
#define RANGESIZE (1<<22)

#define SRCBUF_START 0x300000000
#define DSTBUF_START 0x300000000 + RANGESIZE
-----

(3)
#define BUFSIZE 32 * 1024 // 32KB Pages
#define RANGESIZE (1<<22)

#define SRCBUF_START 0x300000000
#define DSTBUF_START 0x340000000

