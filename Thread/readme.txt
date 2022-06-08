1. pthread_exit跟响应cancel请求一样会先调用clean_up handler然后调用线程变量的析构函数
2. 直接return官方说是会隐式的调用pthread_exit,但是又说不会调用clean_up handler...
3. pthread_testcancel就是放一个取消点
4. 取消的时候在取点直接返回,后面的代码不会执行到
5. 线程退出的时候会析构线程变量


线程退出有三种方式
1. 线程执行函数return,不会调用clean up handler
2. 线程自己调用thread_exit(),任意位置退出线程,退出是调用clean up handler
3. 在取消点响应cancel退出线程,可以认为唤醒并调用thread_exit()
4. exit会导致进程退出, 主线程执行return也是一样的效果