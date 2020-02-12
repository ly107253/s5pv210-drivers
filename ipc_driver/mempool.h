#ifndef _MEMPOOL_H
#define _MEMPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BYTE
#define BYTE	unsigned char
#endif
#ifndef WORD
#define WORD	unsigned short
#endif
#ifndef DWORD
#define DWORD	unsigned int
#endif
#ifndef BOOL
#define BOOL 	int
#endif
#ifndef FALSE
#define FALSE	0
#endif
#ifndef TRUE
#define TRUE	1
#endif

#ifdef __IS_KERNEL__
#define HEAP_SIZE 			PAGE_SIZE 	// 内存堆大小
#else
#define HEAP_SIZE 			4080 		// 内存堆大小
#endif

#define ALIGN_SIZE			(8)			// 内存长度对齐
#define MIN_ALLOC_SIZE		(16)		// 最小可分配内存

#define SMALL_SIZE			(256)		// 小粒度内存（16～256字节）
#define MEDIUM_SIZE			(1024)		// 中粒度内存（264～1024字节）
#define LARGE_SIZE			(4096)		// 大粒度内存（1032～4096字节）

#define SMALL_SPAN			(16)		// 小粒度内存间隔
#define MEDIUM_SPAN			(64)		// 中粒度内存间隔
#define LARGE_SPAN			(128)		// 大粒度内存间隔

#define	SMALL_PAGES			(SMALL_SIZE/SMALL_SPAN)						// 小粒度内存页个数
#define MEDIUM_PAGES		((MEDIUM_SIZE-SMALL_SIZE)/MEDIUM_SPAN)		// 中粒度内存页个数
#define LARGE_PAGES			((LARGE_SIZE-MEDIUM_SIZE)/LARGE_SPAN)		// 大力度内存页个数
#define MAX_PAGES			(SMALL_PAGES+MEDIUM_PAGES+LARGE_PAGES)		// 总内存页个数


// 内存块信息
typedef union tag_BkInfo
{
	unsigned short Value;
	struct
	{
		unsigned short	nUsed:1;		// 第0位表示是否使用（0:未使用，1:已使用）
		unsigned short	nSize:15;		// 前15位标记内存长度
	};
} BkInfo;

typedef struct tag_ChunkHead
{
	BkInfo				bkPrv;					// 前一个内存块的信息
	BkInfo				bkCur;					// 当前内存块的信息
} ChunkHead;

// 内存块
typedef struct tag_Chunk
{
	ChunkHead	head;
	union
	{
		struct
		{
			struct tag_Chunk**	ppHead;			// 链表头
			struct tag_Chunk*	lpPrv;			// 指向链表的前元素（空闲链表）
			struct tag_Chunk*	lpNxt;			// 指向链表的后元素
		};
        unsigned char	Data[1];				// 用户内存起始地址
	};
} Chunk;

// 内存页
typedef struct tag_Page
{
	Chunk*	pHead;								// 空闲链表头
} Page;

//内存堆
typedef struct tag_Heap
{
	struct tag_Heap**	ppHead;
	struct tag_Heap* 	pNext;
	struct tag_Heap*	pPrev;
} Heap;

#define MAX_FREE_HEAPNUM 	4  					// 保持不释放的最大空闲堆数
#define HEAP_TAGSIZE 		(sizeof(Heap)+sizeof(ChunkHead)) // 内存堆标记信息长度
#define HEAP_TAIL(p) 		((ChunkHead*)((BYTE *)(p)+HEAP_SIZE-sizeof(ChunkHead)))
#define FILL_INVALID_CHUNKHEAD(p)  do { \
	(p)->bkPrv.Value = 0xFFFF; \
	(p)->bkCur.Value = 0xFFFF; \
} while(0)

typedef struct tag_MemoryPool
{
	struct list_head listLarge; 		// 已分配大内存链表
	Heap* 			pHeapFree; 			// 空闲堆链表指针
	Heap*			pHeapUsed;			// 使用堆链表指针
	WORD			nHeapNum;			// 总共分配的堆数目
	WORD 			nFreeHeapNum; 		// 空闲堆数目
	DWORD			nMaxBufSize;		// 缓冲区长度
	DWORD			nUsedSize;			// 缓冲区已分配空间
	DWORD			nLargeUsedSize;		// 大内存已分配空间
	Page			pages[MAX_PAGES+1];	// 内存页
} MemoryPool;

BOOL MemPool_Init(MemoryPool *pool, DWORD nBufSize);
void MemPool_Destroy(MemoryPool *pool);
void* MemPool_Alloc(MemoryPool *pool, DWORD objSize);
void MemPool_Free(MemoryPool *pool, void *pObj);
DWORD MemPool_GetSize(MemoryPool *pool);
DWORD MemPool_GetSpace(MemoryPool *pool);
DWORD MemPool_GetUnAllocSize(MemoryPool *pool);

#ifdef __cplusplus
}
#endif

#endif /*_MEMPOOL_H*/

