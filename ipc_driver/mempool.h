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
#define HEAP_SIZE 			PAGE_SIZE 	// �ڴ�Ѵ�С
#else
#define HEAP_SIZE 			4080 		// �ڴ�Ѵ�С
#endif

#define ALIGN_SIZE			(8)			// �ڴ泤�ȶ���
#define MIN_ALLOC_SIZE		(16)		// ��С�ɷ����ڴ�

#define SMALL_SIZE			(256)		// С�����ڴ棨16��256�ֽڣ�
#define MEDIUM_SIZE			(1024)		// �������ڴ棨264��1024�ֽڣ�
#define LARGE_SIZE			(4096)		// �������ڴ棨1032��4096�ֽڣ�

#define SMALL_SPAN			(16)		// С�����ڴ���
#define MEDIUM_SPAN			(64)		// �������ڴ���
#define LARGE_SPAN			(128)		// �������ڴ���

#define	SMALL_PAGES			(SMALL_SIZE/SMALL_SPAN)						// С�����ڴ�ҳ����
#define MEDIUM_PAGES		((MEDIUM_SIZE-SMALL_SIZE)/MEDIUM_SPAN)		// �������ڴ�ҳ����
#define LARGE_PAGES			((LARGE_SIZE-MEDIUM_SIZE)/LARGE_SPAN)		// �������ڴ�ҳ����
#define MAX_PAGES			(SMALL_PAGES+MEDIUM_PAGES+LARGE_PAGES)		// ���ڴ�ҳ����


// �ڴ����Ϣ
typedef union tag_BkInfo
{
	unsigned short Value;
	struct
	{
		unsigned short	nUsed:1;		// ��0λ��ʾ�Ƿ�ʹ�ã�0:δʹ�ã�1:��ʹ�ã�
		unsigned short	nSize:15;		// ǰ15λ����ڴ泤��
	};
} BkInfo;

typedef struct tag_ChunkHead
{
	BkInfo				bkPrv;					// ǰһ���ڴ�����Ϣ
	BkInfo				bkCur;					// ��ǰ�ڴ�����Ϣ
} ChunkHead;

// �ڴ��
typedef struct tag_Chunk
{
	ChunkHead	head;
	union
	{
		struct
		{
			struct tag_Chunk**	ppHead;			// ����ͷ
			struct tag_Chunk*	lpPrv;			// ָ�������ǰԪ�أ���������
			struct tag_Chunk*	lpNxt;			// ָ������ĺ�Ԫ��
		};
        unsigned char	Data[1];				// �û��ڴ���ʼ��ַ
	};
} Chunk;

// �ڴ�ҳ
typedef struct tag_Page
{
	Chunk*	pHead;								// ��������ͷ
} Page;

//�ڴ��
typedef struct tag_Heap
{
	struct tag_Heap**	ppHead;
	struct tag_Heap* 	pNext;
	struct tag_Heap*	pPrev;
} Heap;

#define MAX_FREE_HEAPNUM 	4  					// ���ֲ��ͷŵ������ж���
#define HEAP_TAGSIZE 		(sizeof(Heap)+sizeof(ChunkHead)) // �ڴ�ѱ����Ϣ����
#define HEAP_TAIL(p) 		((ChunkHead*)((BYTE *)(p)+HEAP_SIZE-sizeof(ChunkHead)))
#define FILL_INVALID_CHUNKHEAD(p)  do { \
	(p)->bkPrv.Value = 0xFFFF; \
	(p)->bkCur.Value = 0xFFFF; \
} while(0)

typedef struct tag_MemoryPool
{
	struct list_head listLarge; 		// �ѷ�����ڴ�����
	Heap* 			pHeapFree; 			// ���ж�����ָ��
	Heap*			pHeapUsed;			// ʹ�ö�����ָ��
	WORD			nHeapNum;			// �ܹ�����Ķ���Ŀ
	WORD 			nFreeHeapNum; 		// ���ж���Ŀ
	DWORD			nMaxBufSize;		// ����������
	DWORD			nUsedSize;			// �������ѷ���ռ�
	DWORD			nLargeUsedSize;		// ���ڴ��ѷ���ռ�
	Page			pages[MAX_PAGES+1];	// �ڴ�ҳ
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

