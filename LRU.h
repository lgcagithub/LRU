#ifndef LRU_H_INCLUDED
#define LRU_H_INCLUDED

/**页表项结构*/
typedef struct PageTableItem
{
    int m_iBlockID;         ///对应的物理块号
    int m_bIsLoaded;        ///是否已装入标志
    int m_iPassTime;        ///该页自上次访问到现在经过的时间
} PageTableItem;

/**页表结构*/
typedef struct PageTable
{
    PageTableItem* m_pTable;///页表项数组
    int m_iTLen;            ///页表长度
} PageTable;

/**地址变换机构*/
typedef struct AddressMapping
{
    int m_maxBlockID;       ///最大内存块号
    int m_iPageSize;        ///页面大小
    PageTable m_PT;         ///页表
    int* m_pLoaded;         ///保存已装入内存的页表索引的数组
    int m_LoLen;            ///索引数组的长度
    int m_LoUsed;           ///已经装入的页表项数
} AddressMapping;

/**作业结构*/
typedef struct Job
{
    int* m_pCode;   ///作业的指令数组
    int m_iLength;  ///指令数组长度
} Job;

///-//////////////////////////////////////////////////////////////////

int InitPageTable(PageTable* pPT,size_t tableSize); ///初始化页表
void DestroyPageTable(PageTable* pPT);              ///销毁页表

/**初始化地址变换机构*/
int InitAddressMapping(AddressMapping* pAM,     ///指向地址变换机构
                       size_t maxBlockID,       ///最大内存块号
                       size_t pageSize,         ///页面大小
                       size_t pageTableSize,    ///页表大小
                       size_t allocBlockNum);   ///分配给作业的内存块数

void DestroyAddressMapping(AddressMapping* pAM);///销毁地址变换机构

int InitJob(Job* pJob,size_t iCodeLen);     ///初始化作业
void DestroyJob(Job* pJob);                 ///销毁作业

/**整个程序的初始化*/
int Init(AddressMapping* pAM,       ///指向地址变换机构
          Job* pJob,                ///指向作业
          size_t memSize,           ///内存大小
          size_t pageSize,          ///页面大小
          size_t codeNum,           ///作业指令数
          size_t allocBlockNum);    ///分配给作业的内存块数
void Destroy(AddressMapping* pAM,Job* pJob);///程序销毁过程

int GetMemBlock(AddressMapping* pAM);     ///获得内存块号

void DemandPaging_LRU(AddressMapping* pAM,int pageID);   ///请求调页
void ReplacePage_LRU(AddressMapping* pAM,int pageID);    ///页面置换
void Run_LRU(AddressMapping* pAM,Job* pJob);                 ///运行模拟

#endif // LRU_H_INCLUDED

