/*
************************************************************************************************************
*                                             嵌入式终端软件平台
*
* This software is the project which include the source and binary code are private and protected by law.
* if you get they from anywhere you should destroy it, please don't distribute it! you can't use this 
* software for any purpose. if you see this page in any other place please let'us to know. if you want to 
* get this software and its documentation agreement you can contact our company. However, this notice must 
* not be changed or removed and no warranty is either expressed or implied by its publication or distribution.
*
*		                   Copyright(C) 2013 LINYANG Electronics. All Rights Reserved.
*											jcxian 07/29/2013
*                                           zzq 08/12/2014 change to version of using heap
************************************************************************************************************
*/

#define __IS_KERNEL__

#ifdef __IS_KERNEL__
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif
#include "mempool.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __IS_KERNEL__
#define __MALLOC(size) 		kmalloc(size, GFP_KERNEL)
#define __FREE(p) 			kfree(p)
#define __MALlOC_HEAP 		__get_free_page(GFP_KERNEL)
#define __FREE_HEAP(p) 		free_page((DWORD)p)

#if 0
#define LOG_ASSERT(flag) do { \
	if(!(flag)) { \
		printk("mempool ASSERT failure: %s %d\n", __FILE__, __LINE__); \
	}} while(0) 
#else
#define LOG_ASSERT(flag) 
#endif

#else
#define __MALLOC(size) 		malloc(size)
#define __FREE(p) 			free(p)
#define __MALlOC_HEAP 		malloc(HEAP_SIZE)
#define __FREE_HEAP(p) 		free(p)

#define LOG_ASSERT(flag) do { \
	if(!(flag)) { \
		printf("ASSERT failure: %s %d\n", __FILE__, __LINE__); \
		exit(-1); \
	}} while(0) 
#endif

//系统内存头
typedef struct {
	struct list_head 	list;
	DWORD 				nSize;
	ChunkHead 			head;
} SysMemHead;

#define MAX_BKSIZE		(0x7FFF)				// 内存块最大尺寸
#define MAX_HDSIZE		(sizeof(ChunkHead))		// 内存块头尺寸


////////////////////////////////////////////////////////////////////////////////////////////
// Local inside function
//
static DWORD __MemAlign(DWORD nSize)
{
	DWORD nSpan = 0;
	
	if( nSize < SMALL_SIZE )
	{
		nSpan = SMALL_SPAN;
	}
	else if( nSize < MEDIUM_SIZE )
	{
		nSpan = MEDIUM_SPAN;
	}
	else if( nSize < LARGE_SIZE )
	{
		nSpan = LARGE_SPAN;
	}
	else
	{
		nSpan = ALIGN_SIZE;
	}

	return ((nSize + nSpan - 1) & (~(nSpan - 1)));
}

/*
 *	根据请求长度查找对应内存页序号
 *	bAlloc
 *		TRUE : 分配空间，如果不是完全匹配当前页的内存长度，取后一个内存页
 *		FALSE: 释放空间，取当前内存页，不考虑超出部分
 */
static int __GetPageIndex(DWORD nSize)
{
	// 小粒度内存
	if( nSize <= SMALL_SIZE )
	{
		int nIndx = nSize/SMALL_SPAN-1;
		return nIndx;
	}

	// 中粒度内存
	if( nSize <= MEDIUM_SIZE )
	{
		int nIndx = SMALL_PAGES + (nSize-SMALL_SIZE)/MEDIUM_SPAN - 1;
		return nIndx;
	}

	// 大粒度内存
	if( nSize <= LARGE_SIZE )
	{
		int nIndx = SMALL_PAGES + MEDIUM_PAGES + (nSize-MEDIUM_SIZE)/LARGE_SPAN - 1;
		return nIndx;
	}

	return MAX_PAGES;
}

// 获取内存页来分配内存
static Page* __GetPage(MemoryPool *pool, DWORD nSize)
{
	Page *page = NULL;
	
	int nIndx = __GetPageIndex(nSize);

	LOG_ASSERT( pool != NULL);

	if( nIndx < 0 )
	{
		return NULL;
	}
	
	// 查找可用空闲内存页
	// 唯一非O(1)
	while( nIndx <= MAX_PAGES )
	{
		page = &pool->pages[nIndx];
		if( page->pHead != NULL )
		{
			return page;
		}
		nIndx++;
	}

	return NULL;
}

// 判断内存是有有效
static BOOL __IsValid(MemoryPool *pool, Chunk *p)
{

	LOG_ASSERT( p != NULL);

	return(p->head.bkCur.nSize <= LARGE_SIZE);
}

// 获取前一个连续内存块
static Chunk* __GetPrevChunk(Chunk *p)
{
	unsigned char* pCache = NULL;
	LOG_ASSERT( p != NULL );

	if(p->head.bkPrv.nSize > LARGE_SIZE)
	{
		return NULL;
	}
	pCache = ((unsigned char*)p) - MAX_HDSIZE - p->head.bkPrv.nSize;
	return (Chunk*)pCache;
}

// 获取后一个连续内存块
static Chunk* __GetNextChunk(Chunk *p)
{
	unsigned char *pCache = NULL;
	LOG_ASSERT( p != NULL );
	pCache = ((unsigned char*)p) + MAX_HDSIZE + p->head.bkCur.nSize;
	return (Chunk*)pCache;
}

// 重置下个内存块信息
static void __ResetNextChunkInfo(MemoryPool *pool, Chunk *pHead)
{
	Chunk *pNext = NULL;
	LOG_ASSERT( pool != NULL );
	LOG_ASSERT( pHead != NULL );

	pNext = __GetNextChunk(pHead);
	if( __IsValid(pool, pNext) )
	{
		pNext->head.bkPrv = pHead->head.bkCur;
	}
}

// 添加到空闲列表
static void __PushByIndex(MemoryPool *pool, int nIndx, Chunk *p)
{
	Page *page = NULL;

	LOG_ASSERT( pool != NULL );
	LOG_ASSERT( p != NULL );
	LOG_ASSERT( nIndx >= 0 && nIndx <= MAX_PAGES );

	page = &pool->pages[nIndx];
	
	p->lpNxt = page->pHead;
	p->lpPrv = NULL;
	p->ppHead = &page->pHead;
	
	if( page->pHead != NULL )
	{
		page->pHead->lpPrv = p;
	}
	
	page->pHead = p;
}

static void __Push(MemoryPool *pool, Chunk *p)
{
	int nIndx = -1;
	LOG_ASSERT( pool != NULL );
	LOG_ASSERT( p != NULL );

	if(p->head.bkCur.nSize >= (HEAP_SIZE-HEAP_TAGSIZE-MAX_HDSIZE))  //空闲堆
	{
		Heap *pHeap = (Heap *)((BYTE *)p - sizeof(Heap));

		LOG_ASSERT(p->head.bkPrv.Value == 0xFFFF);

		if(NULL != pHeap->pNext)
		{
			pHeap->pNext->pPrev = pHeap->pPrev;
		}

		if(NULL != pHeap->pPrev)
		{
			pHeap->pPrev->pNext = pHeap->pNext;
		}
		else
		{
			*(pHeap->ppHead) = pHeap->pNext;
		}

		if(pool->nFreeHeapNum >= MAX_FREE_HEAPNUM)
		{
			__FREE_HEAP(pHeap);
			LOG_ASSERT(pool->nHeapNum > 0);
			pool->nHeapNum--;
		}
		else
		{
			pHeap->pNext = pool->pHeapFree;
			pHeap->pPrev = NULL;
			pHeap->ppHead = &pool->pHeapFree;
			if(pool->pHeapFree)
			{
				pool->pHeapFree->pPrev = pHeap;
			}
			pool->pHeapFree = pHeap;
			pool->nFreeHeapNum++;
		}

		return;
	}

	nIndx = __GetPageIndex(p->head.bkCur.nSize);
	__PushByIndex(pool, nIndx, p);
}

// 从空闲列表弹出
static Chunk* __Pop(Page *page)
{
	Chunk *p = NULL;

	LOG_ASSERT( page != NULL );

	p = page->pHead;
	page->pHead = p->lpNxt;
	if( page->pHead )
	{
		page->pHead->lpPrv = NULL;
	}
	p->lpNxt = NULL;
	p->lpPrv = NULL;
	return p;
}

static void __Pop_Chunk(Chunk *p)
{
	Chunk *pPrev = NULL;
	Chunk *pNext = NULL;

	LOG_ASSERT( p != NULL );
	
	pPrev = p->lpPrv;
	pNext = p->lpNxt;

	if( pNext != NULL )
	{
		pNext->lpPrv = pPrev;
	}

	if( pPrev != NULL )
	{
		pPrev->lpNxt = pNext;
	}
	else
	{
		*(p->ppHead) = pNext;
	}
}

// 拆分Chunk
static void __SplitChunk(MemoryPool *pool, Chunk *p, WORD nSize)
{
	int nSpace = 0;
	LOG_ASSERT( pool != NULL );
	LOG_ASSERT( p != NULL );

	nSpace = p->head.bkCur.nSize - nSize - MAX_HDSIZE;
	if( nSpace >= MIN_ALLOC_SIZE )
	{
		Chunk *pNew = NULL;

		// 调整待分配空间
		p->head.bkCur.nSize = nSize;
		
		// 拆分出一块新的内存
		pNew = (Chunk*)&p->Data[nSize];
		pNew->head.bkCur.nSize = nSpace;
		pNew->head.bkCur.nUsed = 0;
		pNew->head.bkPrv = p->head.bkCur;
		pNew->lpNxt = 0;
		pNew->lpPrv = 0;
		
		// 重置后一块内存
		__ResetNextChunkInfo(pool, pNew);
		
		// 添加到空闲列表（需要重新查找页）
		__Push(pool, pNew);
	}
}

// 合并后空闲Chunk
static BOOL __MergeNextChunk(MemoryPool *pool, Chunk **ppHead)
{
	Chunk *pGT = NULL;

	LOG_ASSERT( pool != NULL );
	LOG_ASSERT( ppHead != NULL );
	LOG_ASSERT( *ppHead != NULL );

	pGT = __GetNextChunk(*ppHead);
	if( __IsValid(pool, pGT) )
	{
		if( (pGT->head.bkCur.nUsed == 0) &&
			(pGT->head.bkCur.nSize > 0) )
		{
			WORD nNewSize = (*ppHead)->head.bkCur.nSize + MAX_HDSIZE + pGT->head.bkCur.nSize;
			if( nNewSize <= MAX_BKSIZE )
			{
				__Pop_Chunk(pGT);
				(*ppHead)->head.bkCur.nSize = nNewSize;
				return TRUE;
			}
		}
	}
	return FALSE;
}

// 合并前空闲Chunk
static BOOL __MergePrevChunk(MemoryPool *pool, Chunk **ppHead)
{
	Chunk *pGT = NULL;

	LOG_ASSERT( pool != NULL );
	LOG_ASSERT( ppHead != NULL );
	LOG_ASSERT( *ppHead != NULL );

	pGT = __GetPrevChunk(*ppHead);
	if(NULL != pGT && __IsValid(pool, pGT) )
	{
		if( (pGT->head.bkCur.nUsed == 0) &&
			(pGT->head.bkCur.nSize > 0) )
		{
			WORD nNewSize = pGT->head.bkCur.nSize + MAX_HDSIZE + (*ppHead)->head.bkCur.nSize;
			if( nNewSize <= MAX_BKSIZE )
			{
				__Pop_Chunk(pGT);
				pGT->head.bkCur.nSize = nNewSize;
				*ppHead = pGT;
				return TRUE;
			}
		}
	}
	return FALSE;
}

// 从内存页分配内存
static void* __Malloc_From_Page(MemoryPool *pool, Page *page, WORD nSize)
{
	Chunk *p = NULL;

	LOG_ASSERT( pool != NULL );
	LOG_ASSERT( page != NULL );

	// 从空闲列表取出一个空闲内存块
	p = __Pop(page);
	LOG_ASSERT( p != NULL );
	p->head.bkCur.nUsed = 1;
	
	// 拆分内存块（如果需要）
	__SplitChunk(pool, p, nSize);

	pool->nUsedSize += p->head.bkCur.nSize + MAX_HDSIZE;

	return (void*)p->Data;
}

// 从堆内存区分配内存
static void* __Malloc_From_Heap(MemoryPool *pool, WORD nSize)
{
	Chunk *pNew = NULL;
	Heap *pHeap = NULL;
	ChunkHead *pHeapTail = NULL;

	LOG_ASSERT( pool != NULL );

	if(pool->pHeapFree != NULL)
	{
		LOG_ASSERT(pool->nFreeHeapNum > 0);
		pool->nFreeHeapNum--;

		pHeap = pool->pHeapFree;
		pool->pHeapFree = pHeap->pNext;
		if(pool->pHeapFree)
		{
			pool->pHeapFree->pPrev = NULL;
		}
	}
	else
	{
		pool->nFreeHeapNum = 0;
		pHeap = (Heap *)__MALlOC_HEAP;
		//printf("heap addr=%08X\n", (DWORD)pHeap);
		if(NULL == pHeap)
		{
			return NULL;
		}
		pool->nHeapNum++;
	}

	pHeap->pNext = pool->pHeapUsed;
	pHeap->pPrev = NULL;
	pHeap->ppHead = &pool->pHeapUsed;
	if(pool->pHeapUsed)
	{
		pool->pHeapUsed->pPrev = pHeap;
	}
	pool->pHeapUsed = pHeap;

	pHeapTail = HEAP_TAIL(pHeap);
	FILL_INVALID_CHUNKHEAD(pHeapTail);

	pNew = (Chunk *)(pHeap+1);
	pNew->head.bkCur.nSize = HEAP_SIZE - HEAP_TAGSIZE - MAX_HDSIZE;
	pNew->head.bkCur.nUsed = 1;
	pNew->head.bkPrv.Value = 0xFFFF;

	// 拆分内存块（如果需要）
	__SplitChunk(pool, pNew, nSize);

	pool->nUsedSize += pNew->head.bkCur.nSize + MAX_HDSIZE;

	return (void*)pNew->Data;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Interface function
//
BOOL MemPool_Init(MemoryPool *pool, DWORD nBufSize)
{
	memset(pool, 0, sizeof(MemoryPool));

	pool->pHeapFree = (Heap *)__MALlOC_HEAP;
	//printf("heap addr=%08X\n", (DWORD)pool->pHeapFree);
	if(NULL == pool->pHeapFree)
	{
		return FALSE;
	}
	pool->pHeapFree->pNext = NULL;
	pool->pHeapFree->pPrev = NULL;
	pool->pHeapFree->ppHead = &pool->pHeapFree;

	pool->pHeapUsed = NULL;

	pool->nFreeHeapNum = 1;
	pool->nHeapNum = 1;
	pool->nUsedSize = 0;
	pool->nLargeUsedSize = 0;
	pool->nMaxBufSize = nBufSize;
	INIT_LIST_HEAD(&pool->listLarge);

	return TRUE;
}

/**
* @brief 释放消息内存
*/
void MemPool_Destroy(MemoryPool *pool)
{
	Heap *pTemp = NULL, *pNext = NULL;
	struct list_head *pList = NULL, *pListTmp = NULL;

	LOG_ASSERT( pool != NULL );

	///< 释放空闲
	pTemp = pool->pHeapFree;
	while(NULL != pTemp)
	{
		pNext = pTemp->pNext;
		__FREE_HEAP(pTemp);
		pTemp = pNext;
	}

	///< 释放使用
	pTemp = pool->pHeapUsed;
	while(NULL != pTemp)
	{
		pNext = pTemp->pNext;
		__FREE_HEAP(pTemp);
		pTemp = pNext;
	}

	for(pList = pool->listLarge.next; pList != &pool->listLarge; pList = pListTmp)
	{
		pListTmp = pList->next;

		__FREE(pList);
	}
}

// 分配内存
void* MemPool_Alloc(MemoryPool *pool, DWORD objSize)
{
	DWORD nNewSize = 0;
	void *pMem = NULL;
	Page *page = NULL;

	LOG_ASSERT( pool != NULL );
	nNewSize = __MemAlign(objSize);
	//空闲空间不够了就不再分配内存
	if( (nNewSize+MAX_HDSIZE) > (pool->nMaxBufSize - pool->nUsedSize) )
	{
		return NULL;
	}

	// 超过LARGE_SIZE的内存申请，直接向系统请求
	if( nNewSize > LARGE_SIZE || nNewSize > (HEAP_SIZE-MAX_HDSIZE-HEAP_TAGSIZE) )
	{
		SysMemHead *pLargeMem = NULL;
		if((nNewSize+ sizeof(SysMemHead)) > (pool->nMaxBufSize - pool->nUsedSize))
		{
			return NULL;
		}
		pLargeMem = (SysMemHead *)__MALLOC(nNewSize+sizeof(SysMemHead));
        if(NULL != pLargeMem)
		{
			FILL_INVALID_CHUNKHEAD(&pLargeMem->head);
			pLargeMem->nSize = nNewSize;
            list_add(&pLargeMem->list, &pool->listLarge);
			pLargeMem += 1;
			pool->nUsedSize += nNewSize + sizeof(SysMemHead);
			pool->nLargeUsedSize += nNewSize + sizeof(SysMemHead);
		}
		return (void *)pLargeMem;
	}

	if( nNewSize < MIN_ALLOC_SIZE )
	{
		nNewSize = MIN_ALLOC_SIZE;
	}

	// 查找合适的内存页
	page = __GetPage(pool, (WORD)nNewSize);
	if( page == NULL )
	{
		// 没有合适的内存页，则从公共内存区分配
		pMem = __Malloc_From_Heap(pool, (WORD)nNewSize);
	}
	else
	{
		// 从对应内存页上分配
		pMem = __Malloc_From_Page(pool, page, (WORD)nNewSize);
	}

	return pMem;
}

// 释放内存
void MemPool_Free(MemoryPool *pool, void *pObj)
{
	Chunk *p = NULL;
	WORD nSize = 0;

	LOG_ASSERT( pool != NULL );

	p = (Chunk*)(((unsigned char*)pObj) - MAX_HDSIZE);
	
        
	// 非本内存池的内存块，由系统释放
	if( !__IsValid(pool, p) )
	{
		SysMemHead *pLargeMem = (SysMemHead *)(((unsigned char *)pObj) - sizeof(SysMemHead)); 

		nSize = pLargeMem->nSize + sizeof(SysMemHead);
		LOG_ASSERT(nSize <= pool->nUsedSize && nSize <= pool->nLargeUsedSize);
		pool->nUsedSize -= nSize;
		pool->nLargeUsedSize -= nSize;
		list_del(&pLargeMem->list);
		__FREE(pLargeMem);
		return;
	}
	
	// 当前内存块是空闲内存块
	if( p->head.bkCur.nUsed == 0 )
	{
		return;
	}

	nSize = p->head.bkCur.nSize + MAX_HDSIZE;
	LOG_ASSERT(nSize <= pool->nUsedSize);
	pool->nUsedSize -= nSize;

	p->head.bkCur.nUsed = 0;
	
	
	// 判断前/后（连续）内存块是否空闲
	// 如空闲，则合并内存块
	__MergeNextChunk(pool, &p);
	__MergePrevChunk(pool, &p);

	// 重置后续块信息
	__ResetNextChunkInfo(pool, p);
	
	// 加入空闲链表
	__Push(pool, p);
}

DWORD MemPool_GetSize(MemoryPool *pool)
{
	LOG_ASSERT( pool != NULL );
	return pool->nMaxBufSize;
}

DWORD MemPool_GetSpace(MemoryPool *pool)
{
	LOG_ASSERT( pool != NULL );
	return (pool->nMaxBufSize - pool->nUsedSize);
}

DWORD MemPool_GetUnAllocSize(MemoryPool *pool)
{
	return MemPool_GetSpace(pool);
}

#ifdef __cplusplus
}
#endif

