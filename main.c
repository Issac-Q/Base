#include <stdio.h>
#include <pthread.h>
#include <unistd.h>



//字符串和字符串数组的存储区域测试
char *s3()
{
	static char r[]="Hello world!";
	printf("in s3 r=%p\n", r);
	printf("in s3: string's address: %p\n", &("Hello world!"));
	return r;
}

char *s2()
{
	char *q="Hello world!";
	printf("in s2 q=%p\n", q);
	printf("in s2: string's address: %p\n", &("Hello world!"));
	return q;
}

char *s1()
{
	char* p1 = "qqq";//为了测试‘char p[]="Hello world!"’中的字符串在静态存储区是否也有一份拷贝
	char p[]="Hello world!";
	char* p2 = "w";//为了测试‘char p[]="Hello world!"’中的字符串在静态存储区是否也有一份拷贝
	printf("in s1 p=%p\n", p);
	printf("in s1 p1=%p\n", p1);
	printf("in s1: string's address: %p\n", &("Hello world!"));
	printf("in s1 p2=%p\n", p2);
	return p;
}

static int a;
static int aa = 1;
static const int b;
static const int bb = 1;


//static 一定存储在data段中
//函数体外限定作用域为本文件
//函数体内就是限定存储在data段中


static void cleanup(void* arg)
{
	printf("cleanup\n");
}

void* thread_start(void* arg)
{
	pthread_cleanup_push(cleanup, NULL);
	printf("thread_start\n");
	//pthread_exit(0);
	//return 0;
	//sleep(10);
	while(1) {
		pthread_testcancel();
	}
	pthread_cleanup_pop(0);
}

int main(void)
{
    static const int x;
    static const int xx = 1;

    printf("a: %p\n", &a);
    printf("aa:%p\n", &aa);
    printf("b: %p\n", &b);
    printf("ab:%p\n", &bb);
    printf("x: %p\n", &x);
    printf("xx:%p\n", &xx);

    s1();

    s2();

    s3();

	pthread_t tid;
	if (pthread_create(&tid, NULL, thread_start, NULL) != 0) {
		printf("pthread_create failed\n");
		return 0;
	}
	sleep(2);
	pthread_cancel(tid);
	pthread_join(tid, NULL);
    return 0;
}
