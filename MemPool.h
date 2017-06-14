/**
 * MemPool.h
 *
 * Memory Pooling Class
 *
 * Copyright (c) 2004 by cdecl (byung-kyu kim)
 *
 */

#ifndef __MEM_POOL_H__BY__CDECL
#define __MEM_POOL_H__BY__CDECL


namespace GLASS {

// MemPool Class 
class MemPool
{
public:	
	typedef unsigned int size_type;
	enum { DEFAULT_CHUNK_SIZE = 32 };

	struct MemNode { MemNode *next_; };

public:
	MemPool(size_type type_size, size_type chunk_size = DEFAULT_CHUNK_SIZE) : 
		pHead_(0), pFree_(0), nUsed_(0), 
		TYPESIZE(type_size > sizeof(MemNode*) ? type_size : sizeof(MemNode*)),
		CHUNK_SIZE(chunk_size)
	{
		ExpanseChunk();
	};
	~MemPool() { Release(); }
	
public:
	void* malloc()
	{
		if (IsFull()) {
			if (pFree_) return (NodeNext(pFree_));

			ExpanseChunk();
		}

		int nOffSet = sizeof(MemNode*) + (TYPESIZE * nUsed_++);
		return (reinterpret_cast<char*>(pHead_) + nOffSet);
	}

	void free(void *p)
	{
		MemNode *pmn = reinterpret_cast<MemNode*>(p);
		pmn->next_ = pFree_;
		pFree_ = pmn;
	}

	void ExpanseChunk()
	{
		size_type nAllocSize = TYPESIZE * CHUNK_SIZE + sizeof(MemNode*);

		MemNode *pmn = 
			reinterpret_cast<MemNode *>(::operator new(nAllocSize));

		nUsed_ = 0;;

		pmn->next_ = pHead_;
		pHead_ = pmn;
	}

	bool IsFull() const 
	{
		return nUsed_ >= CHUNK_SIZE;
	}

	void Release()
	{
		while (pHead_ != 0) {
			::operator delete(NodeNext(pHead_));
		}
	}

private:
	inline MemNode* NodeNext(MemNode *&pmn)
	{
		MemNode *p = pmn;
		pmn = pmn->next_;
		return p;
	}

private:
	// No Copy
	MemPool(const MemPool&);
	MemPool& operator=(const MemPool&);

private:
	MemNode *pHead_;
	MemNode *pFree_;
	
	size_type nUsed_;

private:
	const size_type TYPESIZE;
	const size_type CHUNK_SIZE;

};




} // namespace GLASS

#endif 


