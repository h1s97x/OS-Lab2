/*
* Filename : ppipe.c
* copyright : (C) 2006 by zhanghonglie
* Function : 利用管道实现在父子进程间传递整数
*/
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

#define M 10
int x[M],y[M];
int f1()
{
    int i;
    x[1] = 1;
    for(i = 2; i < M; i++)
    {
        x[i] = x[i-1]*i;
    }
}
int f2()
{
    int i;
    y[1] = 1;
    y[2] = 1;
    for(i = 3; i < M; i++)
    {
        y[i] = y[i-1] + y[i-2];
    }
}
int fx(int i)
{
    if(i >= 1 && i < M) return x[i];
    else printf("error,please input i(i>=1 && i<M)");
}
int fy(int i)
{
    if(i >= 1 && i < M) return y[i];
    else printf("error,please input i(i>=1 && i<M)");
}
int main(int argc, char *argv[])
{
    pid_t pid1,pid2;   // 进程号
    int pipe1[2];    // 存放第一个无名管道标号
    int pipe2[2];    // 存放第二个无名管道标号
    int m = 9,n = 9;         // 存放要传递的整数
    int i,j;
    f1();
    f2();
    // 使用 pipe()系统调用建立两个无名管道。建立不成功程序退出，执行终止
    if (pipe(pipe1) < 0)
    {
        perror("pipe not create");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipe2) < 0)
    {
        perror("pipe not create");
        exit(EXIT_FAILURE);
    }
    // 使用 fork()系统调用建立子进程,建立不成功程序退出，执行终止
    if ((pid1 = fork()) < 0)
    {
        perror("process 1 not create");
        exit(EXIT_FAILURE);
    }
	else if (pid1 > 0)
	{
		// 此时1号子进程已经创建成功，当前处在父进程控制下
		// 创造2号子进程
		// 在这里创建2号子进程的原因是防止在1号子进程中创建他的子进程
		if ((pid2 = fork()) < 0)
		{
			// 建立2号子进程失败
			printf("process 2 not create\n");
			exit(EXIT_FAILURE);
		}
		else if (pid2 > 0)
			printf("process create successfully\n"); 
	}

    // 子进程号等于 0 表示子进程在执行,
    if (pid1 == 0)
    {
        // 子进程负责从管道 1 的 0 端读,管道 2 的 1 端写，
        // 所以关掉管道 1 的 1 端和管道 2 的 0 端。
		close(pipe1[0]);
		close(pipe2[0]);
		close(pipe2[1]);
        // 每次循环从管道 1 的 0 端读一个整数放入变量 X 中,
        // 并对 X 加 1 后写入管道 2 的 1 端，直到 X 大于 10
        printf("child %d read: %d write: %d\n", getpid(), m, x[m]);            
        write(pipe1[1], &x[m], sizeof(int));
        // 读写完成后,关闭管道
        close(pipe1[1]);
        sleep(1);
        // 子进程执行结束
        exit(EXIT_SUCCESS);
    }

    if (pid2 == 0 && pid1 > 0)
    {
        // 子进程负责从管道 1 的 0 端读,管道 2 的 1 端写，
        // 所以关掉管道 1 的 1 端和管道 2 的 0 端。
		close(pipe2[0]);
		close(pipe1[0]);
		close(pipe1[1]);
        // 每次循环从管道 2 的 0 端读一个整数放入变量 X 中,
        // 并对 X 加 1 后写入管道 2 的 1 端，直到 X 大于 10
        printf("child %d read: %d write: %d\n", getpid(), n, y[n]);  
        int temp = y[n];          
        write(pipe2[1], &temp, sizeof(int));
        // 读写完成后,关闭管道
        close(pipe2[1]);
        sleep(1);
        // 子进程执行结束
        exit(EXIT_SUCCESS);
    }
    // 子进程号大于 0 表示父进程在执行,
    if (pid1 > 0 && pid2 > 0)
    {
        // 每次循环向管道 1 的 1 端写入变量 X 的值,并从
        // 管道 2 的 0 端读一整数写入 X 再对 X 加 1，直到 X 大于 10
    	waitpid(pid1, NULL, 0);
	    waitpid(pid2, NULL, 0);
        close(pipe1[1]);
		close(pipe2[1]);
		// 接收结果
        read(pipe1[0], &i, sizeof(int));
        read(pipe2[0], &j, sizeof(int));
        printf("parent %d read: %d %d \n", getpid(), i, j);
        printf("f(%d,%d) = %d \n",i , j, i+j);
        close(pipe1[0]);
		close(pipe2[0]);
    }
    // 父进程执行结束
    return EXIT_SUCCESS;
}