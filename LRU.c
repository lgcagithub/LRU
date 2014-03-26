#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "LRU.h"

/**������һ����ȫ�ͷ��ڴ�ĺꡣ*/
#define safe_free(x); if(x){free(x);x = NULL;}

int InitPageTable(PageTable* pPT,size_t tableSize)
{
    int i = 0;
    if(! pPT) return 0;

    /**Ϊҳ���ٿռ�*/
    pPT->m_iTLen = tableSize;
    pPT->m_pTable = (PageTableItem*)malloc(sizeof(PageTableItem) * tableSize);
    if(! pPT->m_pTable) return 0;

    /**��ʼ��ÿ��ҳ����*/
    for(; i < tableSize; ++i)
    {
        pPT->m_pTable[i].m_iBlockID = -1;   ///��ʼֵΪ-1����Ϊ��û�з��䵽�ڴ��
        pPT->m_pTable[i].m_bIsLoaded = 0;   ///0��ʾ��ҳδ��װ���ڴ�
        pPT->m_pTable[i].m_iPassTime = 0;   ///���ϴη��ʵ����ھ���ʱ���ʼֵΪ0
    }

    return 1;
}

void DestroyPageTable(PageTable* pPT)
{
    if(! pPT) return;

    safe_free(pPT->m_pTable);
    pPT->m_iTLen = 0;
}

int InitAddressMapping(AddressMapping* pAM,     ///ָ���ַ�任����
                       size_t maxBlockID,       ///����ڴ���
                       size_t pageSize,         ///ҳ���С
                       size_t pageTableSize,    ///ҳ���С
                       size_t allocBlockNum)    ///�������ҵ���ڴ����
{
    int i = 0;
    if(! pAM) return 0;

    pAM->m_maxBlockID = maxBlockID;

    /**��ʼ��ҳ��*/
    pAM->m_iPageSize = pageSize;
    if(! InitPageTable(&(pAM->m_PT),pageTableSize))
        return 0;

    /**Ϊ�������鿪�ٿռ䡣��������������������װ���ڴ�
    ��ҳ�ţ��������ĺô��ǣ���Ҫ��ҳ���û�ʱ���ñ�����
    ��ҳ��ֻ��������������е������ҵ�ҳ������װ����
    ���ҳ�����滻��*/
    pAM->m_LoLen = allocBlockNum;   ///������ĳ��ȵ��ڷֵ����ڴ����
    pAM->m_LoUsed = 0;              ///��ʼʱ��û��ҳ�汻װ�룬���Գ�ֵΪ0
    pAM->m_pLoaded = (int*)malloc(sizeof(int) * allocBlockNum);
    if(! pAM->m_pLoaded) return 0;

    for(; i < allocBlockNum; ++i)
    {
        pAM->m_pLoaded[i] = -1; ///��ʼ״̬û��ҳ��װ�룬ȫ����-1
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
    int i = 0,j = 0;            ///�����õ�����
    int codeDispersion = 0;     ///ָ����ɢ��
    int centerNum = 0;          ///��ɢ����
    if(! pJob) return 0;

    /**���ٴ����ҵָ����ڴ�ռ�*/
    pJob->m_iLength = iCodeLen;
    pJob->m_pCode = (int*)malloc(sizeof(int) * iCodeLen);
    if(! pJob->m_pCode) return 0;

    /**ָ��������ʵ����һ�����������ΧΪ[0,320)*/
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

int Init(AddressMapping* pAM,       ///ָ���ַ�任����
          Job* pJob,                ///ָ����ҵ
          size_t memSize,           ///�ڴ��С
          size_t pageSize,          ///ҳ���С
          size_t codeNum,           ///��ҵָ����
          size_t allocBlockNum)     ///�������ҵ���ڴ����
{
    if(! (pAM&&pJob)) return 0;
    srand(time(NULL));      ///��ʼ�����������

    /**��ʼ����ҵ�ṹ*/
    if(! InitJob(pJob,codeNum)) return 0;
    /**��ʼ����ַ�任����*/
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
    int index = 0;      ///�����õ�����
    int num = 0;        ///���ɵ������
    int flag = 1;       ///�Ƿ�Ҫ�������ɵı�־
    if(! pAM) return 0;

    while(flag)
    {
        flag =0;
        num = rand() % pAM->m_maxBlockID;
        /**���������ɵ������������һ��װ���ڴ�ҳ��������ţ�
        ���������ɡ�*/
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

    if(pAM->m_LoUsed < pAM->m_LoLen)    ///�ڴ��δ��
    {
        /**װ��ҳ�浽�ڴ�*/
        for(; i < pAM->m_LoLen; ++i)
        {
            if(pAM->m_pLoaded[i] == -1)
            {
                /**ģ�⽫ҳ������ڴ棬��ʵҲ���Ǹ���Ӧҳ����
                �������Ÿ����ֵ��Ȼ����Ӧҳ�����״̬λ
                ��Ϊ 1 ��*/
                pAM->m_PT.m_pTable[pageID].m_iBlockID = GetMemBlock(pAM);
                pAM->m_PT.m_pTable[pageID].m_bIsLoaded = 1;
                /**������֮�⣬����Ҫ�������ҳ�ű��浽���������У�
                ����ҳ���û�ʹ�á����ǵ�Ҫ�������������Ԫ��ʹ����*/
                pAM->m_pLoaded[i] = pageID;
                pAM->m_LoUsed++;

                break;
            }
        }
    }
    else    ///�ڴ����������Ҫҳ���û�
    {
        ReplacePage_LRU(pAM,pageID);
    }
}

void ReplacePage_LRU(AddressMapping* pAM,int pageID)
{
    int index = 0;              ///�����õ�����
    int passTime = 0;           ///�Ƚ��õ�ҳ��δʹ�õľ���ʱ��
    int maxPass = 0;            ///����ҳ��δʹ�õľ���ʱ��
    int maxIndex = 0;           ///�������δʹ��ʱ���ҳ��������������е�����
    int BlockID = 0;            ///�������δʹ��ʱ���ҳ��������
    if(! pAM) return;

    /**�����������飬�ҳ��������δ
    ����ʱ���ҳ��������������е�����*/
    for(; index < pAM->m_LoLen; ++index)
    {
        passTime = pAM->m_PT.m_pTable[pAM->m_pLoaded[index]].m_iPassTime;
        if(passTime > maxPass)
        {
            maxPass = passTime;
            maxIndex = index;
        }
    }
    /**ȡ��������С���ʴ�����ҳ�������ţ�
    ���浽����ҳ���������У������Ļ���ҳ
    ��״̬λΪ 1 ����ʾ�ѻ��롣*/
    BlockID = pAM->m_PT.m_pTable[pAM->m_pLoaded[maxIndex]].m_iBlockID;
    pAM->m_PT.m_pTable[pageID].m_iBlockID = BlockID;
    pAM->m_PT.m_pTable[pageID].m_bIsLoaded = 1;

    /**�޸Ļ���ҳ��״̬λΪ 0 ����ʾ�ѻ�����
    ����ҳ��δʹ��ʱ����Ϊ 0 ��*/
    pAM->m_PT.m_pTable[pAM->m_pLoaded[maxIndex]].m_bIsLoaded = 0;
    pAM->m_PT.m_pTable[pAM->m_pLoaded[maxIndex]].m_iPassTime = 0;

    /**�޸����������л���ҳ������Ϊ����ҳ������*/
    pAM->m_pLoaded[maxIndex] = pageID;
}

void Run_LRU(AddressMapping* pAM,Job* pJob)
{
    int cID = 0;            ///ָ�����
    int pageID = 0;         ///�߼�ҳ��
    int offset = 0;         ///ҳ�ڵ�ַ
    int phyAddr = 0;        ///�����ַ
    int missingCount = 0;   ///ȱҳ����
    int index = 0;          ///�����õĸ�������
    FILE* pOutput = NULL;   ///ָ�������Ϣ���ļ�

    if(! (pAM&&pJob)) return;

    pOutput = fopen("Result_LRU.txt","w");
    if(! pOutput)
    {
        printf("������ļ�ʧ�ܣ�\n");
        return;
    }

    fprintf(pOutput,"��������ڴ��ţ�%d\n",pAM->m_maxBlockID);
    fprintf(pOutput,"ҳ���С��%d��ָ��\n",pAM->m_iPageSize);
    fprintf(pOutput,"��ҵ��С��%d��ָ��\n",pJob->m_iLength);
    fprintf(pOutput,"���������ڴ������%d\n\n",pAM->m_LoLen);
    fprintf(pOutput,"ָ��� �߼���ַ �߼�ҳ�� ������ �����ַ װ�����\n");

    while(cID < pJob->m_iLength)
    {
        pageID = pJob->m_pCode[cID] / pAM->m_iPageSize;
        offset = pJob->m_pCode[cID] % pAM->m_iPageSize;

        if(pAM->m_PT.m_pTable[pageID].m_bIsLoaded)  ///ҳ��װ��
        {
            /**�õ�ǰҳδʹ��ʱ����� ��������ҳδʹ��ʱ��� 1��*/
            pAM->m_PT.m_pTable[pageID].m_iPassTime = 0;
            for(index = 0; index < pAM->m_LoUsed; ++index)
            {
                if(pAM->m_pLoaded[index] != pageID)
                    pAM->m_PT.m_pTable[pAM->m_pLoaded[index]].m_iPassTime++;
            }
            /**ͨ����ַ�任���������߼���ַ��Ӧ�������ַ*/
            phyAddr = pAM->m_PT.m_pTable[pageID].m_iBlockID * pAM->m_iPageSize + offset;

            /**�����Ϣ���ļ�*/
            fprintf(pOutput,"%d\t%d\t%d\t %d\t  %d\t   ",cID,
                                                pJob->m_pCode[cID],
                                                pageID,
                                                pAM->m_PT.m_pTable[pageID].m_iBlockID,
                                                phyAddr);

            /**���ҳ��װ��������ļ�*/
            for(index = 0; index < pAM->m_LoUsed; ++index)
            {
                fprintf(pOutput,"%d\t",pAM->m_pLoaded[index]);
            }
            fprintf(pOutput,"\n");
            index = 0;

            ///���ỹҪ���װ�����
            ++cID;
        }
        else    ///ҳδװ�룬�����ҳ
        {
            DemandPaging_LRU(pAM,pageID);
            ++missingCount;
        }
    }
    /**���ȱҳ������ȱҳ�ʵ��ļ�*/
    fprintf(pOutput,"ȱҳ������%d\nȱҳ�ʣ�%g",missingCount,
                                                missingCount * 1.0f / pJob->m_iLength);

    fclose(pOutput);    ///�ǵùر��ļ�
}



