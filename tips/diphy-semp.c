/**
 *哲学家吃饭问题：几个哲学家吃饭，假定哲学家有五个人，这五个哲学家坐在一张圆桌上面，
 *每个哲学家的左手旁边都放有一个叉子(fork)，那么，这围城一圈的五个哲学家有五个叉子
 *。每个哲学家有三种状态，thinking(思考)，trying(尝试去拿叉子吃饭)，eating(已
 *经拿起叉子，正在吃饭)。每次吃饭需要两个叉子，也就是哲学家左右手边的叉子。
 * */


/**
 * 本例使用信号量解决哲学家进餐的问题。
 * 哲学家的数目和叉子的数目相等。
 * 叉子是几个哲学家共享的资源，将这些资源声明为信号量，其中每一个资源为一个信号量，
 * 本例中的信号量为0和1两个值，当信号量为0的时候，是不能够被另外一个进程使用，当
 * 信号两为1的时候，可以被使用。
 * */
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#include <semaphore.h>
#include <stdlib.h>
//设定哲学家的数目
int Number;

//定义哲学家的三种状态
#define Thinking 1 
#define Trying 2
#define Eating 3

pthread_mutex_t mutex; //声明一个互斥量，用来在使用资源的时候锁住

sem_t *semph; //为每一个叉子定义信号量

int *state;    //记录每一个哲学家的状态

//得到参数
void GetArg(
    char* argv[];
    int* number;
);

void main(int argc, char* argv[])
{
    GetArg(argv, &Number);
    semph = malloc(Number* sizeof(sem_t));
    state = malloc(Number* sizeof(int));
    
    pthread_t philosopher[Number];     //为每一个哲学家声明一个进程
    
    pthread_mutex_init(&mutex, NULL); //初始化互斥量
    
    void * EatMeal(); //哲学家操作的主要函数
    
    int i =0;
    for(i = 0; i < Number; i++)
    {
        int j = i;
        sem_init(&semph[i], 0, 1);// 初始化信号量，信号量的值为0或者1
        
        //创建进程，每个进程执行函数EatMeal()中的操作
        int res = pthread_create(&philosopher[i], NULL, EatMeal, &j); 
        printf("This is thread %d \n",i);
    
        /*
        //检查是否创建进程成功
        if(res !=0)
        {
            printf("%d sucess!\n", i);
        }*/
    }
    
    for(i = 0; i < Number; i++)
    {
        pthread_join(philosopher[i], NULL);
    }
    
    pthread_exit(0);
    //sleep(100);//主要进程执行等待的任务（应该修改，直到每一个线程都被完成主进程才结束）
}

//定义哲学家吃饭的这个问题
void *EatMeal(int *j)
{
    int phiD = *j; //记录哲学家的id号
    int leftPhi; //记录哲学家左边的叉子的id号
    int rightPhi; //记录哲学家右边的叉子的id号
    
    leftPhi = (phiD + Number - 1) % Number;
    rightPhi = (phiD + Number + 1) % Number;
    
    state[phiD] = Thinking; //哲学家的初始状态设为 Thinking
    
    int mymealCount = 0;
    
    int mealCount = 1;
    
    double time;
    
    while(mymealCount < mealCount) //规定每个人吃饭不能超过最大吃饭次数                                                                           
    {
        
        time = (rand()%9 + 1)/100.0; //得到的时间在0.01 - 0.1之间
        
         //如果当前的状态是Thinking，那么则转化为Trying的状态
        int i = state[phiD];//记录当前的状态
        if(state[phiD] == Thinking)
        {
            printf("%d philosopher is thinking!\n", phiD);
            sleep(1);//在这个状态停留一段时间
            state[phiD] = Trying;
                        
        }
        //如果当前的状态是Trying，则检查是否能够达到Eating的状态
        else if(state[phiD] == Trying)
        {
            printf("%d philosopher is trying!\n",phiD);
            sleep(1);//在这个状态停留一段时间
            if(!sem_wait(&semph[leftPhi]))//如果能够拿起哲学家左边的叉子
            {
                if(!sem_wait(&semph[rightPhi]))//也能够拿起右边的叉子
                {
                    state[phiD] = Eating;
                }else//不能拿起右边的叉子
                {
                    sem_post(&semph[leftPhi]);//放下左边的叉子
                }
            }
        }
        else //状态为 Eating
        {
            printf("%d philosopher is eating!\n",phiD);
            sleep(1);//先吃一段时间
            sem_post(&semph[leftPhi]);//放下左边的叉子
            sem_post(&semph[rightPhi]);//放下右边的叉子
            state[phiD] = Thinking;
            mymealCount++;
        }
        
        if(i != state[phiD])
        {
            pthread_mutex_lock(&mutex);
            printf("%d state tranformed from %d to %d\n", phiD, i, state[phiD]);
            pthread_mutex_unlock(&mutex);
        }
        
    }
}

void GetArg(
    char* argv[],
    int* number
    )
{
    *number = strtol(argv[1], NULL, 10);
}
