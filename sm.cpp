#include "sm.h"
#include<map>


/*******************************************************************************************/
/* Variables */
/*******************************************************************************************/
unsigned int m_initialPoolSize;              // Initial pool size to be created.
//vector<int> m_PoolSizes;                    // Vector of all the pool sizes created so far
map<unsigned int, PoolData_t*> m_PoolMap;    // Mapping of size and pool

/*******************************************************************************************/
/* Functions */
/*******************************************************************************************/



//----------------------------------------------------------------------------------------------
// @name                    : initStorageManager
//
// @description             : Creates pools on the basis of initial pool sizes specified.
//
// @param initialPoolSize   : This is the size of each pool (in bytes) that will be allocated
//                            at startup.
// @param numPools          : Number of pools specified at startup
// @param pools             : Array containing size of different pools that need to be created
//                            at startup
//
// @returns                 : Nothing
//----------------------------------------------------------------------------------------------
void initStorageManager(const unsigned initialPoolSize, int numPools, const unsigned int *pools)
{
    m_initialPoolSize = initialPoolSize;
    unsigned int totalClaimedMemory = 0;

    printf("StorageManager:: Initial Pools- ");
    for (int i = 0; i < numPools; i++)
    {
        PoolData_t *poolData = (PoolData_t *)malloc(sizeof(PoolData_t));
        if (poolData == nullptr)
        {
            printf("\n\n**MEMORY ERROR: initStorageManager: Failed to create pool %u!!\n\n", pools[i]);
            abort();
        }

        memset(poolData, 0, sizeof(poolData));

        size_t sizeOfPoolInBytes = pools[i] * sizeof(char) * initialPoolSize;
        totalClaimedMemory += sizeOfPoolInBytes;
        char *ptr = (char *)malloc(sizeOfPoolInBytes);

        initializePoolData(sizeOfPoolInBytes, ptr, pools[i], poolData);

        printf("%d ", pools[i]);
    }

    
    printf("\nStorageManager:: Pool init complete\n");
    printf("Total claimed memory: %u MB\n\n", totalClaimedMemory/1000/1000);

}

//----------------------------------------------------------------------------------------------
// @name                    : createNewPool
//
// @description             : Creates a new memory pool. This is called if an allocation 
//                            is requested for a size for which there exists no pools. This
//                            allocates memory to the pool and initializes its data. The size
//                            of this pool will depend on the m_initialPoolSize which was 
//                            initialized at the time of initStorageManager().
//
// @param sizeId            : The size of allocation (in bytes) which was requested. This 
//                            size is not the same as the pool size. 

//
// @returns                 : Nothing
//----------------------------------------------------------------------------------------------
void createNewPool(unsigned sizeId)
{
    PoolData_t *poolData = (PoolData_t *)malloc(sizeof(PoolData_t));
    if (poolData == nullptr)
    {
        printf("\n\n**MEMORY ERROR: createNewPool: Failed to create pool %u!!\n\n", sizeId);
        abort();
    }

    memset(poolData, 0, sizeof(poolData));

    size_t sizeOfPoolInBytes = sizeId * sizeof(char) * m_initialPoolSize;
    char *ptr = (char *)malloc(sizeOfPoolInBytes);

    initializePoolData(sizeOfPoolInBytes, ptr, sizeId, poolData);
    printf("Created new pool : %u\n", sizeId);
}

//----------------------------------------------------------------------------------------------
// @name                    : initializePoolData
//
// @description             : Initialize the Pool specific data.
//
// @param sizeOfPoolInBytes : Total size of the pool (bytes) that needs to be initialized.
// @param ptr               : Pointer to start of this memory pool
// @param sizeId            : Size ID of this pool. (e.g- For a request of 8 bytes, a pool of
//                            sizeId=8 will be created)
// @param poolData          : Pool specific data that has to be initialized
//
// @returns                 : Nothing
//----------------------------------------------------------------------------------------------
void initializePoolData(size_t sizeOfPoolInBytes, char *ptr, unsigned int sizeId, PoolData_t *poolData)
{
    poolData->poolSize = sizeId;
    poolData->startAddress = ptr;
    poolData->endAddress = ptr + sizeOfPoolInBytes;
    poolData->totalSize = sizeOfPoolInBytes;
    poolData->remainingSpace = sizeOfPoolInBytes;
    poolData->totalBlocks = (poolData->endAddress - poolData->startAddress) / poolData->poolSize;
    poolData->freeBlocks = poolData->totalBlocks;
    poolData->usedBlocks = 0;
    poolData->nextFreeBlock = -1;
    poolData->nextFreeBlockInSequence = 0;
    poolData->isFreedBlockBlockAvailable = false;
    poolData->totalAllocationsFromThisPool = 0;

    m_PoolMap[sizeId] = poolData;
}

/*******************************************************************************************/
void expandPool(unsigned size)
{
    //TODO
}

//----------------------------------------------------------------------------------------------
// @name                    : displayPoolInfo
//
// @description             : Displays detailed information of each pool present in the system.
//
// @returns                 : Nothing
//----------------------------------------------------------------------------------------------
void displayPoolInfo()
{
    map<unsigned int, PoolData_t*>::iterator it = m_PoolMap.begin();
    
    printf("\n\n");
    while (it != m_PoolMap.end())
    {
        PoolData_t *poolData = it->second;
        unsigned int poolSize = it->first;

        
        printf("Pool %u\n", poolSize);
        
        printf("  totalAllocationsFromThisPool       : %u\n", poolData->totalAllocationsFromThisPool);
        printf("  startAddress                       : 0x%x\n", poolData->startAddress);
        printf("  endAddress                         : 0x%x\n", poolData->endAddress);
        printf("  totalSize                          : %u bytes\n", poolData->totalSize);
        printf("  remainingSpace                     : %u bytes\n", poolData->remainingSpace);
        printf("  totalBlocks                        : %u\n", poolData->totalBlocks);
        printf("  freeBlocks                         : %u\n", poolData->freeBlocks);
        printf("  usedBlocks                         : %u\n", poolData->usedBlocks);
        printf("\n");

        it++;
    }
    printf("\n** Total Pools: %d **\n", m_PoolMap.size());
}

/*******************************************************************************************/
void destroyStorageManager()
{
    //TODO
}

//----------------------------------------------------------------------------------------------
// @name                    : SM_alloc
//
// @description             : This function is called from SM_ALLOC_ARRAY macro. It checks if
//                            a pool of sizeID=size is present. If yes, then memory is allocated
//                            from that pool. If not, then a new pool is created with sizeID=size
//                            and memory is allocated from it.
//
// @param size              : Size of memory requested for heap allocation.
//
// @returns                 : Pointer to start of the allocated memory (from Pool)
//----------------------------------------------------------------------------------------------
void * SM_alloc(size_t size)
{
    //printf("SM_alloc called for %d bytes\n", size);

    /* Find which pool to use. If pool of required size not present, create a pool */
    PoolData_t *poolData = m_PoolMap[size];
    if (poolData != nullptr)
    {
        /* Check if this pool has enough free blocks */
        if (poolData->freeBlocks == 0)
        {
            printf("ERROR: Pool %u exhausted!\n", size);
            abort();
            //expandPool(size);
        }
    }
    else
    {
        createNewPool(size);
        poolData = m_PoolMap[size];
    }

    char *ptr = nullptr;

    if (poolData->isFreedBlockBlockAvailable && poolData->nextFreeBlock >= 0)
    {
        /* Allocating a block which was freed earlier. */
        ptr = findAddressFromBlock(poolData->nextFreeBlock, poolData);
        poolData->isFreedBlockBlockAvailable = false;
        poolData->nextFreeBlock = -1;
        //printf(">> From freed block, poolData->nextFreeBlock = %u\n", poolData->nextFreeBlock);
    }
    else
    {
        /* Allocating from free blocks in sequnce */
        ptr = findAddressFromBlock(poolData->nextFreeBlockInSequence, poolData);
        //printf(">> From sequence\n");
        poolData->nextFreeBlockInSequence++;
    }

    //printf("Allocated 0x%x (block %u) in pool %u\n", ptr, poolData->usedBlocks, poolData->poolSize);

    /* Handle statistics */
    poolData->freeBlocks--;
    poolData->usedBlocks++;
    poolData->remainingSpace -= size;
    poolData->totalAllocationsFromThisPool++;

    return ptr;
}

//----------------------------------------------------------------------------------------------
// @name                    : SM_dealloc
//
// @description             : This function is called from SM_DEALLOC macro. It marks the 
//                            memory pointed to by ptr as free. Actual de-allocation DOES NOT
//                            takes place. This memory block is then re-claimed for future
//                            allocations from this pool.
//
// @param ptr               : Pointer to memory that needs to be freed.
//
// @returns                 : Nothing
//----------------------------------------------------------------------------------------------
void SM_dealloc(void *ptr)
{
    if (ptr == nullptr)
    {
        return;
    }

    /* Find the pool in which this address lies. */
    unsigned int poolSize = findPoolFromAddress(ptr);
    //printf("Deallocating 0x%x from pool %u\n", ptr, poolSize);

    /* Mark this address as free */
    PoolData_t *poolData = m_PoolMap[poolSize];
    poolData->freeBlocks++;
    poolData->usedBlocks--;
    poolData->nextFreeBlock = findBlockFromAddress((char *)ptr, poolData);
    //printf("Setting nextFreeBlock=%u in pool %u\n", poolData->nextFreeBlock, poolData->poolSize);
    poolData->isFreedBlockBlockAvailable = true;
    poolData->remainingSpace += poolSize;

    //printf("Deallocated 0x%x block (%u) from pool %u\n", ptr, poolData->nextFreeBlock, poolSize);
}

//----------------------------------------------------------------------------------------------
// @name                    : findPoolFromAddress
//
// @description             : This function is used to determine the Pool (sizeId) in which
//                            this memory block (address pointed by ptr) lies. this is used
//                            at the time of deallocation.
//                            TODO: Find some efficient way to determine in 
//                                  which pool does this ptr lie
//
// @param ptr               : Pointer to memory whose pool needs to be determined.
//
// @returns                 : sizeId of the pool in which this memory resides.
//----------------------------------------------------------------------------------------------
unsigned int findPoolFromAddress(void *ptr)
{
    map<unsigned int, PoolData_t*>::iterator it = m_PoolMap.begin();
    while (it != m_PoolMap.end())
    {
        PoolData_t *poolData = it->second;
        //printf(" Checking 0x%x in pool %u [0x%x, 0x%x] \n", ptr, poolData->poolSize, poolData->startAddress, poolData->endAddress);
        if (ptr >= poolData->startAddress && ptr <= poolData->endAddress)
        {
            return poolData->poolSize;
        }
        it++;
    }

    /* If we reached here, it means something is wrong! */
    printf("\n\n**ERROR: 0x%x not present in any of the memory pools!!\n\n", ptr);
    abort();
}

//----------------------------------------------------------------------------------------------
// @name                    : findAddressFromBlock
//
// @description             : Given the index (param block) and the pool, determine 
//                            the address of memory. This implements an efficient way to 
//                            determine memory address using simple arithmetic operation.
//                            This becomes possible because the pools are of fixed size.
//                            Each block (allocated memory) in a pool is at a fixed distance
//                            from each other. This helps in a fast memory allocation.
//
// @param block             : Index of the block in this pool
// @param poolData          : Pool specific data
//
// @returns                 : Pointer to memory corresponding to the given block.
//----------------------------------------------------------------------------------------------
char *findAddressFromBlock(unsigned int block, PoolData_t *poolData)
{
    return poolData->startAddress + (block * poolData->poolSize);
}

//----------------------------------------------------------------------------------------------
// @name                    : findBlockFromAddress
//
// @description             : Given the memory address and the Pool specific data, it determines
//                            the block i.e, index of memory chunk in the given Pool. This is
//                            used at the time of de-allocation to quickly mark the next free
//                            memory block in the given Pool.
//
// @param addr              : Memory address
// @param poolData          : Pool specific data
//
// @returns                 : Block corresponding to the given address (addr).
//----------------------------------------------------------------------------------------------
unsigned int findBlockFromAddress(char *addr, PoolData_t *poolData)
{
    return (addr - poolData->startAddress) / poolData->poolSize;    
}