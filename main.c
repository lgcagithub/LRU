#include <stdio.h>
#include <stdlib.h>
#include "LRU.h"

int main()
{
    size_t memSize = 0;         ///内存大小，按指令条数算
    size_t pageSize = 0;        ///页面大小
    size_t codeNum = 0;         ///作业指令数量
    size_t allocBlockNum = 0;   ///分配给作业的内存块数
    AddressMapping AM;          ///地址变换机构
    Job job;                    ///作业

    printf("请输入内存大小：");
    scanf("%d",&memSize);
    printf("请输入页面大小：");
    scanf("%d",&pageSize);
    printf("请输入作业大小：");
    scanf("%d",&codeNum);
    printf("请输入分配给作业的内存块数：");
    scanf("%d",&allocBlockNum);

    if(memSize < codeNum)
    {
        printf("内存太小，作业无法运行\n");
        return 0;
    }

    if(Init(&AM,&job,memSize,pageSize,codeNum,allocBlockNum))
        Run_LRU(&AM,&job);

    Destroy(&AM,&job);

    printf("结果已输出到文件\"Result_LRU.txt\"中···\n");

    return 0;
}

