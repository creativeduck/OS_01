//// OS_Sim.cpp : �� ���Ͽ��� 'main' �Լ��� ���Ե˴ϴ�. �ű⼭ ���α׷� ������ ���۵ǰ� ����˴ϴ�.
////
//
//#include <iostream>
//#include <thread>
//#include <Windows.h>
//#include <mutex>              // std::mutex, std::unique_lock
//#include <condition_variable>
//
//#define TIME_QUANTUM    5
//
//#define PROCESS_READY   0
//#define PROCESS_RUN     1
//#define PROCESS_BLOCK   2
//
//#define SCHED_IO_REQ            0
//#define SCHED_QUANTUM_EXPIRED   1
//
//volatile int cur_proc;
//
//std::condition_variable cv;
//std::mutex cv_m;
//
//struct proc_tbl_t {
//    int id;
//    int priority;
//    int state;
//    int time_quantum;
//    std::thread th;
//    std::mutex mu_lock;
//    struct proc_tbl_t* prev;
//    struct proc_tbl_t* next;
//} proc_tbl[10];
//
//struct proc_tbl_t run_q;
//struct proc_tbl_t block_q;
//
//void Put_Tail_Q(proc_tbl_t*, proc_tbl_t*);
//proc_tbl_t* Get_Head_Q(proc_tbl_t* head);
//void Print_Q(proc_tbl_t*);
//void syscall_reqio();
//void sys_timer_int();
//void sys_scheduler(int why);
//
//void IO_Completion_Interrupt(int id);
//void proc_1(int id);
//void proc_2(int id);
//void proc_3(int id);
//void proc_4(int id);
//void proc_5(int id);
//
//
//
//void proc_1(int id)
//{
//    int r;
//    std::unique_lock<std::mutex> lk(cv_m);
//
//    for (;;) {
//        cv.wait(lk, [=] {return cur_proc == id; });
//        std::cout << id << ' ';
//        r = rand() % 100;
//        if (r < 10) syscall_reqio();  // call IO with 10% probability
//        else Sleep(10);
//    }
//}
//
//void proc_2(int id)  // CPU-bound process, Do computation only, No IO Request
//{
//    std::unique_lock<std::mutex> lk(cv_m);
//
//    for (;;) {
//        cv.wait(lk, [=] {return cur_proc == id; });
//        std::cout << id << ' ';
//        Sleep(10);
//    }
//}
//
//void proc_3(int id)
//{
//    int r;
//    std::unique_lock<std::mutex> lk(cv_m);
//
//    for (;;) {
//        cv.wait(lk, [=] {return cur_proc == id; });
//        std::cout << id << ' ';
//        r = rand() % 100;
//        if (r < 15) syscall_reqio();  // call IO with 15% probability
//        else Sleep(10);
//    }
//}
//
//void proc_4(int id)
//{
//    int r;
//    std::unique_lock<std::mutex> lk(cv_m);
//
//    for (;;) {
//        cv.wait(lk, [=] {return cur_proc == id; });
//        std::cout << id << ' ';
//        r = rand() % 100;
//        if (r < 20) syscall_reqio();  // call IO with 20% probability
//        else Sleep(10);
//    }
//}
//
//void proc_5(int id)
//{
//    int r;
//    std::unique_lock<std::mutex> lk(cv_m);
//
//    for (;;) {
//        cv.wait(lk, [=] {return cur_proc == id; });
//        std::cout << id << ' ';
//        r = rand() % 100;
//        if (r < 5) syscall_reqio();  // call IO with 5% probability
//        else Sleep(10);
//    }
//}
//
//int main()
//{
//    proc_tbl_t* p;
//
//    run_q.next = run_q.prev = &(run_q);
//    block_q.next = block_q.prev = &(block_q);
//    cur_proc = -1;
//
//    p = &(proc_tbl[0]);
//    p->id = 0;
//    p->priority = 0;
//    p->state = PROCESS_READY;
//    p->th = std::thread(IO_Completion_Interrupt, 0);
//
//    Put_Tail_Q(&run_q, p);
//    //    Print_Q(&run_q);
//
//    p = &(proc_tbl[1]);
//    p->id = 1;
//    p->priority = 0;
//    p->state = PROCESS_READY;
//    p->th = std::thread(proc_1, 1);
//
//    Put_Tail_Q(&run_q, p);
//    //    Print_Q(&run_q);
//
//    p = &(proc_tbl[2]);
//    p->id = 2;
//    p->priority = 0;
//    p->state = PROCESS_READY;
//    p->th = std::thread(proc_2, 2);
//
//    Put_Tail_Q(&run_q, p);
//    //    Print_Q(&run_q);
//
//    p = &(proc_tbl[3]);
//    p->id = 3;
//    p->priority = 0;
//    p->state = PROCESS_READY;
//    p->th = std::thread(proc_3, 3);
//
//    Put_Tail_Q(&run_q, p);
//    //    Print_Q(&run_q);
//
//    p = &(proc_tbl[4]);
//    p->id = 4;
//    p->priority = 0;
//    p->state = PROCESS_READY;
//    p->th = std::thread(proc_4, 4);
//
//    Put_Tail_Q(&run_q, p);
//
//    p = &(proc_tbl[5]);
//    p->id = 5;
//    p->priority = 0;
//    p->state = PROCESS_READY;
//    p->th = std::thread(proc_5, 5);
//
//    Put_Tail_Q(&run_q, p);
//    //    Print_Q(&run_q);
//        // The member function will be executed in a separate thread
//
//        // Wait for the thread to finish, this is a blocking operation
//
//        // Now Begin Schedule
//
//    sys_timer_int();
//
//    return 0;
//}
//
//void syscall_reqio()
//{
//    // Invoke IO Operation, 
//    // Then schedule to other process
//    sys_scheduler(SCHED_IO_REQ);
//}
//
//void sys_timer_int()
//{
//    proc_tbl_t* p;
//
//    for (;;) {
//        /* do Time Service */
//        if (cur_proc == -1) {
//            sys_scheduler(SCHED_QUANTUM_EXPIRED);
//        }
//        else {
//            p = &(proc_tbl[cur_proc]);
//            p->time_quantum--;
//            if (p->time_quantum <= 0)
//                sys_scheduler(SCHED_QUANTUM_EXPIRED);
//        }
//        Sleep(100);
//    }
//}
//
//void sys_scheduler(int why)
//{
//    proc_tbl_t* p;
//
//    /* �л����� �� �Լ������� �ڵ带 �ۼ��ؾ� �Ѵ� */
//
//    if (cur_proc != -1) {
//        p = &(proc_tbl[cur_proc]);
//        p->state = PROCESS_READY;
//        Put_Tail_Q(&run_q, p);
//    }
//    //    Print_Q(&run_q);
//    p = Get_Head_Q(&run_q);
//    if (p == NULL) {
//        cur_proc = -1;
//        return;
//    }
//
//    cur_proc = p->id;
//    p->state = PROCESS_RUN;
//    p->time_quantum = TIME_QUANTUM;
//    cv.notify_all();   // switch to process p->id and run */
//}
//
///* IO Interrupt Generator */
//void IO_Completion_Interrupt(int id)
//{
//
//    for (;;) {
//
//        /* �л����� �̰��� �ڵ带 �ۼ��ؾ� �Ѵ� */
//
//        Sleep(3000);   /* Wakeup in every 3 sec */
//    }
//}
//
//void Put_Tail_Q(proc_tbl_t* head, proc_tbl_t* item)
//{
//    (head->mu_lock).lock();
//    item->prev = head->prev;
//    head->prev->next = item;
//    item->next = head;
//    head->prev = item;
//    (head->mu_lock).unlock();
//}
//
//proc_tbl_t* Get_Head_Q(proc_tbl_t* head)
//{
//    proc_tbl_t* item;
//
//    (head->mu_lock).lock();
//    if (head->next == head) {
//        (head->mu_lock).unlock();
//        return NULL;
//    }
//    item = head->next;
//
//    item->next->prev = head;
//    head->next = item->next;
//
//    (head->mu_lock).unlock();
//    return item;
//}
//
//void Print_Q(proc_tbl_t* head)
//{
//    proc_tbl_t* item;
//
//    item = head->next;
//    while (item != head) {
//        std::cout << item->id << ' ';
//        item = item->next;
//    }
//    std::cout << '\n';
//}
//
//
//// ���α׷� ����: <Ctrl+F5> �Ǵ� [�����] > [��������� �ʰ� ����] �޴�
//// ���α׷� �����: <F5> Ű �Ǵ� [�����] > [����� ����] �޴�
//
//// ������ ���� ��: 
////   1. [�ַ�� Ž����] â�� ����Ͽ� ������ �߰�/�����մϴ�.
////   2. [�� Ž����] â�� ����Ͽ� �ҽ� ��� �����մϴ�.
////   3. [���] â�� ����Ͽ� ���� ��� �� ��Ÿ �޽����� Ȯ���մϴ�.
////   4. [���� ���] â�� ����Ͽ� ������ ���ϴ�.
////   5. [������Ʈ] > [�� �׸� �߰�]�� �̵��Ͽ� �� �ڵ� ������ ����ų�, [������Ʈ] > [���� �׸� �߰�]�� �̵��Ͽ� ���� �ڵ� ������ ������Ʈ�� �߰��մϴ�.
////   6. ���߿� �� ������Ʈ�� �ٽ� ������ [����] > [����] > [������Ʈ]�� �̵��ϰ� .sln ������ �����մϴ�.
