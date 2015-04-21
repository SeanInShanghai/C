/**
 *哲学家吃饭问题：几个哲学家吃饭，假定哲学家有五个人，这五个哲学家坐在一张圆桌上面，
 *每个哲学家的左手旁边都放有一个叉子(fork)，那么，这围城一圈的五个哲学家有五个叉子
 *。每个哲学家有三种状态，thinking(思考)，trying(尝试去拿叉子吃饭)，eating(已
 *经拿起叉子，正在吃饭)。每次吃饭需要两个叉子，也就是哲学家左右手边的叉子。
 * */
 
 /**
  * 本例使用pthread库实现哲学家进餐的问题。
  * */
  
 /**
  *在这个例子中使用互斥量解决死锁问题。 
  * */
 
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h> 
#include <sys/wait.h>//为了使用sleep()函数
 
 
#include <unistd.h>//参数的识别
#include <getopt.h> //长命令的解析

//哲学家的数目
int  Number;
 
//声明共享变量fork，其中fork的数目和哲学家数目是相同的
pthread_mutex_t *pfork;

//定义一个philosopher的三个状态
#define Thinking 1
#define Trying 2
#define Eating 3

int *state;

void *EatMeal();

//得到参数
void GetArg(
    char* argv[] /*in*/,
    int* number /*out*/
    );
void main(int argc,  char* argv[])
{
    
    
    
    int k =0;
    
    while(k<1)
    {
        int option_index = 0;
        int rvalue = 0;
        struct option long_option[] = {
            {"normal",0,0,0 },
            {"method1",0,0,0},
            {"method2",0,0,0}
        };
        
        rvalue = getopt_long_only(argc,argv, "a:bc::", long_option,&option_index);
        switch(option_index)
        {
            case 0 : printf("%s\n",long_option[option_index].name);
                break;
            case 1 : printf("%s\n",long_option[option_index].name);
                break;
            case 2 : printf("%s\n",long_option[option_index].name);
                break;
        }
        k++;
    }
    
    
    
    
    
    
    GetArg(argv, &Number);
    pfork = malloc(Number*sizeof(pthread_mutex_t));
    state = malloc(Number*sizeof(int));
    
    //声明进程数组，每一个进程代表一个哲学家
    pthread_t philosopher[Number];
    int i;
    //初始化每一个资源实例
    for( i = 0; i < Number; i++)
    {
        pthread_mutex_init(&pfork[i],NULL);
    }
    
    //创建和哲学家数量想对应的进程，并且每个进程开始进行吃饭的活动
    for( i = 0; i < Number; i++)
    {
        //记录当前进程的编号，并传递到Meal()函数中
        int j = i;
        pthread_create(&philosopher[i], NULL, EatMeal, &j);
        printf("I am philosopher %d\n", j);
    }
    
    //将所有的进程进行Join操作。
    for( i=0; i < Number; i++)
    {
        pthread_join(philosopher[i], NULL);
    }
    
    //退出程序
    pthread_exit(0);
    
    return ;
    
}

void *EatMeal(int *i)
{
    //记录当前的线程id号
    int id = *i;

    
    state[id] = Thinking; //线程初始化的时候为Thinking
    
    int leftFork = (id + Number -1) % Number;
    int rightFork = (id + Number +1) % Number;
    
    int mealTime = 5;
    int mymealTime = 0;
    while (mymealTime < mealTime) //每个philosopher必须吃得符合规定
    {
        if(state[id] == Thinking)
        {
            printf("Philosopher %d is thinking\n", id);
            sleep(1);
            state[id] = Trying;
        }else if(state[id] == Trying)
        {
            printf("Philosopher %d is Trying\n", id);
            sleep(1);
            pthread_mutex_lock(&pfork[leftFork]);
            pthread_mutex_lock(&pfork[rightFork]);
            state[id] = Eating;
        }
        else 
        {
            printf("Philosopher %d is Eating\n", id);
            sleep(1);
            mymealTime++;
            
            pthread_mutex_unlock(&pfork[leftFork]);
            pthread_mutex_unlock(&pfork[rightFork]);
        }
    }
    
}

void GetArg(
    char * argv[],
    int* number
    )
{
    *number = strtol(argv[1], NULL, 10);
}
