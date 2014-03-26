#include <stdio.h>
#include <stdlib.h>
#include "LRU.h"

int main()
{
    size_t memSize = 0;         ///�ڴ��С����ָ��������
    size_t pageSize = 0;        ///ҳ���С
    size_t codeNum = 0;         ///��ҵָ������
    size_t allocBlockNum = 0;   ///�������ҵ���ڴ����
    AddressMapping AM;          ///��ַ�任����
    Job job;                    ///��ҵ

    printf("�������ڴ��С��");
    scanf("%d",&memSize);
    printf("������ҳ���С��");
    scanf("%d",&pageSize);
    printf("��������ҵ��С��");
    scanf("%d",&codeNum);
    printf("������������ҵ���ڴ������");
    scanf("%d",&allocBlockNum);

    if(memSize < codeNum)
    {
        printf("�ڴ�̫С����ҵ�޷�����\n");
        return 0;
    }

    if(Init(&AM,&job,memSize,pageSize,codeNum,allocBlockNum))
        Run_LRU(&AM,&job);

    Destroy(&AM,&job);

    printf("�����������ļ�\"Result_LRU.txt\"�С�����\n");

    return 0;
}

