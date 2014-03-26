#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "LRU.h"

/**这里是一个安全释放内存的宏。*/
#define safe_free(x); if(x){free(x);x = NULL;}

int InitPageTable(PageTable* pPT,size_t tableSize)
{
    int i = 0;
    if(! pPT) return 0;

    /**为页表开辟空间*/
    pPT->m_iTLen = tableSize;
    pPT->m_pTable = (PageTableItem*)malloc(sizeof(PageTableItem) * tableSize);
    if(! pPT->m_pTable) return 0;

    /**初始化每个页表项*/
    for(; i < tableSize; ++i)
    {
        pPT->m_pTable[i].m_iBlockID = -1;   ///初始值为-1，因为还没有分配到内存块
        pPT->m_pTable[i].m_bIsLoaded = 0;   ///0表示该页未被装入内存
        pPT->m_pTable[i].m_iPassTime = 0;   ///自上次访问到现在经过时间初始值为0
    }

    return 1;
}

void DestroyPageTable(PageTable* pPT)
{
    if(! pPT) return;

    safe_free(pPT->m_pTable);
    pPT->m_iTLen = 0;
}

int InitAddressMapping(AddressMapping* pAM,     ///指向地址变换机构
                       size_t maxBlockID,       ///最大内存块号
                       size_t pageSize,         ///页面大小
                       size_t pageTableSize,    ///页表大小
                       size_t allocBlockNum)    ///分配给作业的内存块数
{
    int i = 0;
    if(! pAM) return 0;

    pAM->m_maxBlockID = maxBlockID;

    /**初始化页表*/
    pAM->m_iPageSize = pageSize;
    if(! InitPageTable(&(pAM->m_PT),pageTableSize))
        return 0;

    /**为索引数组开辟空间。该索引数组用来保存已装入内存
    的页号，这样做的好处是：当要求页面置换时不用遍历整
    个页表，只需根据索引数组中的索引找到页表中已装入内
    存的页面来替换。*/
    pAM->m_LoLen = allocBlockNum;   ///该数组的长度等于分到的内存块数
    pAM->m_LoUsed = 0;              ///开始时还没有页面被装入，所以初值为0
    pAM->m_pLoaded = (int*)malloc(sizeof(int) * allocBlockNum);
    if(! pAM->m_pLoaded) return 0;

    for(; i < allocBlockNum; ++i)
    {
        pAM->m_pLoaded[i] = -1; ///初始状态没有页面装入，全部置-1
    }

    return 1;
}

void DestroyAddressMapping(AddressMapping* pAM)
{
    if(! pAM) return;

    pAM->m_maxBlockID = 0;
    pAM->m_iPageSize = 0;
    DestroyPageTable(&(pAM->m_PT));
    safe_free(pAM->m_pLoaded);
    pAM->m_LoLen = 0;
}

int InitJob(Job* pJob,size_t iCodeLen)
{
    int i = 0,j = 0;            ///遍历用的索引
    int codeDispersion = 0;     ///指令离散度
    int centerNum = 0;          ///离散中心
    if(! pJob) return 0;

    /**开辟存放作业指令的内存空间*/
    pJob->m_iLength = iCodeLen;
    pJob->m_pCode = (int*)malloc(sizeof(int) * iCodeLen);
    if(! pJob->m_pCode) return 0;

    /**指令数组其实就是一组随机数，范围为[0,320)*/
    for(i = 0; i < iCodeLen; )
    {
        codeDispersion = rand() % 7;
        centerNum = rand() % iCodeLen;
        for(j = i; j < i+codeDispersion && j < iCodeLen; ++j)
        {
            pJob->m_pCode[j] = (centerNum - codeDispersion + rand() % (codeDispersion<<1)) % iCodeLen;
        }
        i = j;
    }
    return 1;
}

void DestroyJob(Job* pJob)
{
    if(! pJob) return;

    safe_free(pJob->m_pCode);
    pJob->m_iLength = 0;
}

int Init(AddressMapping* pAM,       ///指向地址变换机构
          Job* pJob,                ///指向作业
          size_t memSize,           ///内存大小
          size_t pageSize,          ///页面大小
          size_t codeNum,           ///作业指令数
          size_t allocBlockNum)     ///分配给作业的内存块数
{
    if(! (pAM&&pJob)) return 0;
    srand(time(NULL));      ///初始化随机数种子

    /**初始化作业结构*/
    if(! InitJob(pJob,codeNum)) return 0;
    /**初始化地址变换机构*/
    if(! InitAddressMapping(pAM,
                            memSize / pageSize,
                            pageSize,
                            codeNum / pageSize + (codeNum % pageSize != 0),
                            allocBlockNum))
        return 0;

    return 1;
}

void Destroy(AddressMapping* pAM,Job* pJob)
{
    DestroyAddressMapping(pAM);
    DestroyJob(pJob);
}

int GetMemBlock(AddressMapping* pAM)
{
    int index = 0;      ///遍历用的索引
    int num = 0;        ///生成的随机数
    int flag = 1;       ///是否要继续生成的标志
    if(! pAM) return 0;

    while(flag)
    {
        flag =0;
        num = rand() % pAM->m_maxBlockID;
        /**若发现生成的随机数等于任一已装入内存页面的物理块号，
        则重新生成。*/
        for(index = 0; index < pAM->m_LoUsed; ++index)
        {
            if(num == pAM->m_PT.m_pTable[pAM->m_pLoaded[index]].m_iBlockID)
            {
                flag = 1;
                break;
            }
        }
    }
    return num;
}

void DemandPaging_LRU(AddressMapping* pAM,int pageID)
{
    int i = 0;
    if(! pAM) return;

    if(pAM->m_LoUsed < pAM->m_LoLen)    ///内存块未满
    {
        /**装入页面到内存*/
        for(; i < pAM->m_LoLen; ++i)
        {
            if(pAM->m_pLoaded[i] == -1)
            {
                /**模拟将页面调入内存，其实也就是给相应页表项
                的物理块号赋随机值，然后将相应页表项的状态位
                置为 1 。*/
                pAM->m_PT.m_pTable[pageID].m_iBlockID = GetMemBlock(pAM);
                pAM->m_PT.m_pTable[pageID].m_bIsLoaded = 1;
                /**除上述之外，还需要将调入的页号保存到索引数组中，
                方便页面置换使用。最后记得要递增索引数组的元素使用数*/
                pAM->m_pLoaded[i] = pageID;
                pAM->m_LoUsed++;

                break;
            }
        }
    }
    else    ///内存块已满，需要页面置换
    {
        ReplacePage_LRU(pAM,pageID);
    }
}

void ReplacePage_LRU(AddressMapping* pAM,int pageID)
{
    int index = 0;              ///遍历用的索引
    int passTime = 0;           ///比较用的页面未使用的经过时间
    int maxPass = 0;            ///最大的页面未使用的经过时间
    int maxIndex = 0;           ///具有最大未使用时间的页面号在索引数组中的索引
    int BlockID = 0;            ///具有最大未使用时间的页面物理块号
    if(! pAM) return;

    /**遍历索引数组，找出具有最大未
    访问时间的页面号在索引数组中的索引*/
    for(; index < pAM->m_LoLen; ++index)
    {
        passTime = pAM->m_PT.m_pTable[pAM->m_pLoaded[index]].m_iPassTime;
        if(passTime > maxPass)
        {
            maxPass = passTime;
            maxIndex = index;
        }
    }
    /**取出具有最小访问次数的页面物理块号，
    保存到换入页的物理块号中，并更改换入页
    的状态位为 1 ，表示已换入。*/
    BlockID = pAM->m_PT.m_pTable[pAM->m_pLoaded[maxIndex]].m_iBlockID;
    pAM->m_PT.m_pTable[pageID].m_iBlockID = BlockID;
    pAM->m_PT.m_pTable[pageID].m_bIsLoaded = 1;

    /**修改换出页的状态位为 0 ，表示已换出。
    另将其页面未使用时间置为 0 。*/
    pAM->m_PT.m_pTable[pAM->m_pLoaded[maxIndex]].m_bIsLoaded = 0;
    pAM->m_PT.m_pTable[pAM->m_pLoaded[maxIndex]].m_iPassTime = 0;

    /**修改索引数组中换出页的索引为换入页的索引*/
    pAM->m_pLoaded[maxIndex] = pageID;
}

void Run_LRU(AddressMapping* pAM,Job* pJob)
{
    int cID = 0;            ///指令序号
    int pageID = 0;         ///逻辑页号
    int offset = 0;         ///页内地址
    int phyAddr = 0;        ///物理地址
    int missingCount = 0;   ///缺页次数
    int index = 0;          ///遍历用的辅助索引
    FILE* pOutput = NULL;   ///指向输出信息的文件

    if(! (pAM&&pJob)) return;

    pOutput = fopen("Result_LRU.txt","w");
    if(! pOutput)
    {
        printf("打开输出文件失败！\n");
        return;
    }

    fprintf(pOutput,"最大物理内存块号：%d\n",pAM->m_maxBlockID);
    fprintf(pOutput,"页面大小：%d条指令\n",pAM->m_iPageSize);
    fprintf(pOutput,"作业大小：%d条指令\n",pJob->m_iLength);
    fprintf(pOutput,"允许分配的内存块数：%d\n\n",pAM->m_LoLen);
    fprintf(pOutput,"指令号 逻辑地址 逻辑页号 物理块号 物理地址 装入情况\n");

    while(cID < pJob->m_iLength)
    {
        pageID = pJob->m_pCode[cID] / pAM->m_iPageSize;
        offset = pJob->m_pCode[cID] % pAM->m_iPageSize;

        if(pAM->m_PT.m_pTable[pageID].m_bIsLoaded)  ///页已装入
        {
            /**让当前页未使用时间归零 。让其他页未使用时间加 1。*/
            pAM->m_PT.m_pTable[pageID].m_iPassTime = 0;
            for(index = 0; index < pAM->m_LoUsed; ++index)
            {
                if(pAM->m_pLoaded[index] != pageID)
                    pAM->m_PT.m_pTable[pAM->m_pLoaded[index]].m_iPassTime++;
            }
            /**通过地址变换机构计算逻辑地址对应的物理地址*/
            phyAddr = pAM->m_PT.m_pTable[pageID].m_iBlockID * pAM->m_iPageSize + offset;

            /**输出信息到文件*/
            fprintf(pOutput,"%d\t%d\t%d\t %d\t  %d\t   ",cID,
                                                pJob->m_pCode[cID],
                                                pageID,
                                                pAM->m_PT.m_pTable[pageID].m_iBlockID,
                                                phyAddr);

            /**输出页面装入情况到文件*/
            for(index = 0; index < pAM->m_LoUsed; ++index)
            {
                fprintf(pOutput,"%d\t",pAM->m_pLoaded[index]);
            }
            fprintf(pOutput,"\n");
            index = 0;

            ///待会还要输出装载情况
            ++cID;
        }
        else    ///页未装入，请求调页
        {
            DemandPaging_LRU(pAM,pageID);
            ++missingCount;
        }
    }
    /**输出缺页次数和缺页率到文件*/
    fprintf(pOutput,"缺页次数：%d\n缺页率：%g",missingCount,
                                                missingCount * 1.0f / pJob->m_iLength);

    fclose(pOutput);    ///记得关闭文件
}



