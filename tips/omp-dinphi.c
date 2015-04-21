#include <stdio.h>
#include <stdlib.h>

//#ifdef_OPENMP		
#  include <omp.h>
//#endif

#include <unistd.h>//参数的识别
#include <getopt.h> //长命令的解析

//哲学家的数目
int  Number;

//定义一个philosopher的三个状态
#define Thinking 1
#define Trying 2
#define Eating 3

int *state;//记录哲学家的状态




//得到参数
void GetArg(
	char* argv[] /*in*/,
	int* number /*out*/
	);

//开始吃饭
void EatMeal();
void EatMeal2();

//吃饭的资源 Fork
struct Fork{
	int i;//只有0和1两个值，0代表不能被拿起来，1代表可以被拿起来
};


//cLOCK Declare the forks
omp_lock_t *mylock;

struct Fork * myfork;//声明fork的指针，代表吃饭的时候所拿的叉子

void initFork(struct Fork* fork, int i);//初始化每个资源的情况

int take_Fork(int forkNo);//拿起叉子
int release_Fork(int forkNo);//释放叉子
/**哲学家开始吃饭的函数
 * */
void main(int argc, char* argv[])
{
	
	int methodID=0;// 如果为1的话是normal，如果为2的话是mutex，如果为3的话是sema
	
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
				methodID = 1;
				break;
			case 1 : printf("%s\n",long_option[option_index].name);
				methodID = 2;
			    break;
			case 2 : printf("%s\n",long_option[option_index].name);
				methodID = 3;
				break;
		}
		k++;
	}
	
	
	
	
	GetArg(argv, &Number);//得到参数,参数代表哲学家的个数
	
	
	myfork = malloc(Number*sizeof(struct Fork)); //根据输入的哲学家的数目来初始化叉子资源的数目
	state = malloc(Number*sizeof(int));	//为哲学家的状态记录
	
	mylock = malloc(Number*sizeof(omp_lock_t));//
	
	//	printf("%d\n",Number);	
	initFork(myfork, Number);
	
	if(methodID == 1)
	{
			printf("%d\n",methodID);
			#pragma omp parallel num_threads(Number) //开始根据Number的值产生线程
			EatMealNormal();
	
	}else if(methodID == 2 )
	{
		printf("%d\n",methodID);
	
			#pragma omp parallel num_threads(Number) //开始根据Number的值产生线程
			EatMeal();
	}
	else 
	{
		printf("%d\n",methodID);
			#pragma omp parallel num_threads(Number) //开始根据Number的值产生线程
			EatMeal2();
	
	}
	
}
void GetArg(
	char * argv[],
	int* number
	)
{
	*number = strtol(argv[2], NULL, 10);
}

//
void EatMealNormal()
{
	//记录当前的线程id号
	int phID = omp_get_thread_num();
	int thread_count = omp_get_num_threads();// 所有进程的数目
	
	state[phID] = Thinking; //线程初始化的时候为Thinking
	
	int leftFork = (phID + thread_count) % thread_count;
	int rightFork = (phID + thread_count +1) % thread_count;
	
	int mealTime = 5;
	int mymealTime = 0;
	while (mymealTime < mealTime) //每个philosopher必须吃得符合规定
	{
		if(state[phID] == Thinking)
		{
			printf("Philosopher %d is thinking\n", phID);
			sleep(1);
			state[phID] = Trying;
		}else if(state[phID] == Trying)
		{
			
			printf("Philosopher %d is Trying\n", phID);
			sleep(1);
			if(!take_Fork(leftFork)&&!take_Fork(rightFork)) //拿起左边的叉子
			{
				state[phID] = Eating;
				release_Fork(leftFork);
				release_Fork(rightFork);
			}
			
			
		}
		else 
		{
			printf("Philosopher %d is Eating\n", phID);
			sleep(1);
			mymealTime++;
			

		}
	}
	
}

/**
 * 开始吃饭，每个哲学家通过调用这个函数，来判断是否
 * 
 */
void EatMeal(){
		
	//#ifdef_OPENMP
		int phID = omp_get_thread_num(); //当前进程的ID
		int thread_count = omp_get_num_threads();// 所有进程的数目
	//#else
		
		
	//#endif
	
		state[phID] = Thinking;
		
		int leftFork = (phID + thread_count )%thread_count; //左边叉子的ID
		int rightFork = (phID + thread_count + 1)%thread_count;//右边叉子的ID
		
		int mealTime = 5;
		int mymealTime = 0;
		
		while(mymealTime < mealTime)
		{
			if(state[phID] == Thinking)
			{
				printf("Philosopher %d is thinking!\n", phID);
				sleep(1);// 处于Thinking一段时间
				state[phID] = Trying;
			}else if(state[phID] == Trying)
			{
				printf("Philosopher %d is trying! \n", phID);
				sleep(1);
				
				#pragma omp critical
				{
					if(take_Fork(leftFork))   //拿起左边的叉子
					{
						if(take_Fork(rightFork)) //准备拿起右边的产自
						{
							state[phID] = Eating;
							
						}else release_Fork(leftFork);//如果拿不起来右边的叉子，则放弃左边的叉子
					}
					else state[phID] = Trying;
				}
			}else
			{
				printf("Philosopher %d is eating!\n", phID);
				sleep(1);
				mymealTime++;
				
				#pragma omp critical
				{
					release_Fork(leftFork);
					release_Fork(rightFork);
				}
				state[phID] = Thinking;
			} 
		}
	
	
}


/**
 * 第二种方法来吃饭，EatMeal2
 * 
 * */
 
void EatMeal2()
{
		
		//#ifdef_OPENMP
		int phID = omp_get_thread_num(); //当前进程的ID
		int thread_count = omp_get_num_threads();// 所有进程的数目
	//#else
		
		
	//#endif
	
		state[phID] = Thinking;
		
		int leftFork = (phID + thread_count )%thread_count; //左边叉子的ID
		int rightFork = (phID + thread_count + 1)%thread_count;//右边叉子的ID
		
		int mealTime = 5;
		int mymealTime = 0;
		
		while(mymealTime < mealTime)
		{
			if(state[phID] == Thinking)
			{
				printf("Philosopher %d is thinking!\n", phID);
				sleep(1);// 处于Thinking一段时间
				state[phID] = Trying;
			}else if(state[phID] == Trying)
			{
				printf("Philosopher %d is trying! \n", phID);
				sleep(1);
			
				omp_set_lock(&mylock[leftFork]);	
				omp_set_lock(&mylock[rightFork]);
				state = Eating;
			

			}else
			{
				printf("Philosopher %d is eating!\n", phID);
				sleep(1);
				mymealTime++;
				
				omp_unset_lock(&mylock[leftFork]);
				omp_unset_lock(&mylock[rightFork]);
				state[phID] = Thinking;
			} 
		}
	
}




/**
 * 初始化叉子的状态，设置为0，没有被使用
 * **/
void initFork(struct Fork* fork,int i)
{
	while(i)
	{
		fork[i-1].i = 1;
		i--;
	}
}

/***
 * 拿起叉子
 * */
int take_Fork(int forkNo)
{
	if(myfork[forkNo].i == 1)
	{
		myfork[forkNo].i = 0;
		return 1;
	}
	else return 0;
}

/**
 * 放下叉子
 */
int	release_Fork(int forkNo)
{
	myfork[forkNo].i = 1;
	return 1;
} 
