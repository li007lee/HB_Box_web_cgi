/*
 * main.c
 *
 *  Created on: 2015年12月2日
 *      Author: root
 */

#include "my_include.h"


typedef struct _tagCPU_OCCUPY        //定义一个cpu occupy的结构体
{
char name[20];      //定义一个char类型的数组名name有20个元素
unsigned int user; //定义一个无符号的int类型的user
unsigned int nice; //定义一个无符号的int类型的nice
unsigned int system;//定义一个无符号的int类型的system
unsigned int idle; //定义一个无符号的int类型的idle
}CPU_OCCUPY;


typedef struct _tagMEM_OCCUPY         //定义一个mem occupy的结构体
{
char name[20];      //定义一个char类型的数组名name有20个元素
unsigned long total;
char name2[20];
unsigned long free;
}MEM_OCCUPY;


static float  get_memoccupy (MEM_OCCUPY *mem) //对无类型get函数含有一个形参结构体类弄的指针O
{
    FILE *fd;
    //int n;
    char buff[256];
    MEM_OCCUPY *m;
    m=mem;

    fd = fopen ("/proc/meminfo", "r");

    fgets (buff, sizeof(buff), fd);
    //printf("###[%s]\n", buff);
    sscanf (buff, "%s %lu %s", m->name, &m->total, m->name2);
   // printf("##name=%s total=%u name2=%s", m->name, m->total, m->name2);
    int mem_total;
    mem_total = m->total;
    fgets (buff, sizeof(buff), fd);
    //printf("###[%s]\n", buff);
    memset(m, 0, sizeof(MEM_OCCUPY));
    sscanf (buff, "%s %lu %s", m->name, &m->total, m->name2);
    int mem_free;
    mem_free = m->total;
#if 0
    fgets (buff, sizeof(buff), fd);
    printf("###[%s]\n", buff);
    fgets (buff, sizeof(buff), fd);
    printf("###[%s]\n", buff);
    //sscanf (buff, "%s %u %s", m->name, &m->total, m->name2);

    fgets (buff, sizeof(buff), fd); //从fd文件中读取长度为buff的字符串再存到起始地址为buff这个空间里
    printf("###[%s]\n", buff);
    sscanf (buff, "%s %u %s", m->name2, &m->free, m->name2);
#endif
    fclose(fd);     //关闭文件fd
  //  printf("\nRRRRRR   used=%d total=%d\n",mem_total-mem_free, mem_total);
    return ((float)(mem_total-mem_free)/(float)mem_total)*100;
}

static int cal_cpuoccupy (CPU_OCCUPY *old, CPU_OCCUPY *n)
{
    unsigned long od, nd;
    unsigned long id, sd;
    int cpu_use = 0;

    od = (unsigned long)(old->user + old->nice + old->system +old->idle);//第一次(用户+优先级+系统+空闲)的时间再赋给od
    nd = (unsigned long)(n->user + n->nice + n->system +n->idle);//第二次(用户+优先级+系统+空闲)的时间再赋给od

    id = (unsigned long) (n->user - old->user);    //用户第一次和第二次的时间之差再赋给id
    sd = (unsigned long) (n->system - old->system);//系统第一次和第二次的时间之差再赋给sd
    if((nd-od) != 0)
    cpu_use = (int)((sd+id)*100)/(nd-od); //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used
    else cpu_use = 0;
    //printf("cpu: %u/n",cpu_use);
    return cpu_use;
}

static int get_cpuoccupy (CPU_OCCUPY *cpust) //对无类型get函数含有一个形参结构体类弄的指针O
{
    FILE *fd;
    //int n;
    char buff[256];
    CPU_OCCUPY *cpu_occupy;
    cpu_occupy=cpust;

    fd = fopen ("/proc/stat", "r");
    fgets (buff, sizeof(buff), fd);

    sscanf (buff, "%s %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle);

    fclose(fd);
    return 0;
}

int GetCPUMEM(int *cpu_c, float *mem_c)
{
    CPU_OCCUPY cpu_stat1;
    CPU_OCCUPY cpu_stat2;
    MEM_OCCUPY mem_stat;
    //int cpu_c;
    //float mem_c;
    memset(&cpu_stat1, 0, sizeof(CPU_OCCUPY));
    memset(&cpu_stat2, 0, sizeof(CPU_OCCUPY));
    memset(&mem_stat, 0, sizeof(MEM_OCCUPY));
    //获取内存
    *mem_c = get_memoccupy ((MEM_OCCUPY *)&mem_stat);

    //第一次获取cpu使用情况
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat1);
    sleep(2);
    //第二次获取cpu使用情况
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat2);

    //计算cpu使用率
    *cpu_c = cal_cpuoccupy ((CPU_OCCUPY *)&cpu_stat1, (CPU_OCCUPY *)&cpu_stat2);
    //printf("\n@@@@ cpu : %d%%   mem : %.0f%% \n", cpu, mem_c);

	//printf("Content type: text/html\n\n");
	//printf("%d%%    MEM : %.0f%%", *cpu_c, mem_c);

    return 0;
}
