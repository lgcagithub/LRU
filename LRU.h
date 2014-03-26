#ifndef LRU_H_INCLUDED
#define LRU_H_INCLUDED

/**ҳ����ṹ*/
typedef struct PageTableItem
{
    int m_iBlockID;         ///��Ӧ��������
    int m_bIsLoaded;        ///�Ƿ���װ���־
    int m_iPassTime;        ///��ҳ���ϴη��ʵ����ھ�����ʱ��
} PageTableItem;

/**ҳ��ṹ*/
typedef struct PageTable
{
    PageTableItem* m_pTable;///ҳ��������
    int m_iTLen;            ///ҳ����
} PageTable;

/**��ַ�任����*/
typedef struct AddressMapping
{
    int m_maxBlockID;       ///����ڴ���
    int m_iPageSize;        ///ҳ���С
    PageTable m_PT;         ///ҳ��
    int* m_pLoaded;         ///������װ���ڴ��ҳ������������
    int m_LoLen;            ///��������ĳ���
    int m_LoUsed;           ///�Ѿ�װ���ҳ������
} AddressMapping;

/**��ҵ�ṹ*/
typedef struct Job
{
    int* m_pCode;   ///��ҵ��ָ������
    int m_iLength;  ///ָ�����鳤��
} Job;

///-//////////////////////////////////////////////////////////////////

int InitPageTable(PageTable* pPT,size_t tableSize); ///��ʼ��ҳ��
void DestroyPageTable(PageTable* pPT);              ///����ҳ��

/**��ʼ����ַ�任����*/
int InitAddressMapping(AddressMapping* pAM,     ///ָ���ַ�任����
                       size_t maxBlockID,       ///����ڴ���
                       size_t pageSize,         ///ҳ���С
                       size_t pageTableSize,    ///ҳ���С
                       size_t allocBlockNum);   ///�������ҵ���ڴ����

void DestroyAddressMapping(AddressMapping* pAM);///���ٵ�ַ�任����

int InitJob(Job* pJob,size_t iCodeLen);     ///��ʼ����ҵ
void DestroyJob(Job* pJob);                 ///������ҵ

/**��������ĳ�ʼ��*/
int Init(AddressMapping* pAM,       ///ָ���ַ�任����
          Job* pJob,                ///ָ����ҵ
          size_t memSize,           ///�ڴ��С
          size_t pageSize,          ///ҳ���С
          size_t codeNum,           ///��ҵָ����
          size_t allocBlockNum);    ///�������ҵ���ڴ����
void Destroy(AddressMapping* pAM,Job* pJob);///�������ٹ���

int GetMemBlock(AddressMapping* pAM);     ///����ڴ���

void DemandPaging_LRU(AddressMapping* pAM,int pageID);   ///�����ҳ
void ReplacePage_LRU(AddressMapping* pAM,int pageID);    ///ҳ���û�
void Run_LRU(AddressMapping* pAM,Job* pJob);                 ///����ģ��

#endif // LRU_H_INCLUDED

