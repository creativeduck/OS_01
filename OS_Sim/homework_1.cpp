// OS_Sim.cpp : �� ���Ͽ��� 'main' �Լ��� ���Ե˴ϴ�. �ű⼭ ���α׷� ������ ���۵ǰ� ����˴ϴ�.
//

#include <iostream>
#include <thread>
#include <Windows.h>
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable>

#define TIME_QUANTUM    5

#define PROCESS_READY   0
#define PROCESS_RUN     1
#define PROCESS_BLOCK   2

#define SCHED_IO_REQ            0
#define SCHED_QUANTUM_EXPIRED   1

#define IO_PERIOD 2000 // IO_Completion_Interrupt �� sleep �ϴ� �ֱ�, ms ����

#define TYPE_READY 0   // ���� ť�� �ִ� ���μ����� ����ϴ� ���̶�� �˸�
#define TYPE_BLOCK 1   // ��� ť�� �ִ� ���μ����� ����ϴ� ���̶�� �˸�

#define TIMER_TURN 0   // timer �� ����
#define IO_TURN 1      // IO�� ����

volatile int cur_proc; // ���� ����ǰ� �ִ� ���μ����� id ��

std::condition_variable cv; // ���� ����
std::mutex cv_m;            // ���ؽ�

struct proc_tbl_t { // ���� ���μ���
    int id;                  // id
    int priority;            // �켱����
    int state;               // ����
    int time_quantum;        // �ð� �Ҵ緮
    std::thread th;          // ������
    std::mutex mu_lock;      // ���ؽ�
    struct proc_tbl_t* prev; // ���� ���μ���
    struct proc_tbl_t* next; // ���� ���μ���
} proc_tbl[10]; // ���μ��� 10�� ���� �迭

struct proc_tbl_t run_q;    // �غ� ť
struct proc_tbl_t block_q;  // ��� ť

void Put_Tail_Q(proc_tbl_t*, proc_tbl_t*);
proc_tbl_t* Get_Head_Q(proc_tbl_t* head);
void Print_Q(proc_tbl_t*, int type);
void syscall_reqio();
void sys_timer_int();
void sys_scheduler(int why);

void IO_Completion_Interrupt(int id);
void proc_1(int id);
void proc_2(int id);
void proc_3(int id);
void proc_4(int id);
void proc_5(int id);
void proc_6(int id);
void proc_7(int id);
void proc_8(int id);
void proc_9(int id);

bool interested[2];
volatile int turn;

/*
�� ���μ������� ������ ���鼭 �۵��Ѵ�.
cv.wait() �Լ��� cur_proc �� �ڽ��� ��ȣ�� �� ������ ��ٸ���,
�ڽ��� ���ʰ� �Ǹ� �ý��� ȣ���� �Ѵ�.

CPU-�ٿ�� ���μ����� ��� id�� ����ϰ� �ٽ� ����,
IO-�ٿ���� ��� syscal_reqio() ȣ���� ���� ������� �䱸�Ѵ�.

�̶� IO/CPU ��������� rand �� ������ �����Ѵ�.
���� ���, r < 40 ���� �������ָ� 40% Ȯ���� IO �ý��� ȣ�� �ϵ���
*/
void proc_1(int id) // ���޹��� id ������ ���μ��� ����
{
    int r;
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        // cur_proc �� ���� id ���� ��ġ�ϸ�, wait���� ��� ���� ����
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        r = rand() % 100;
        if (r < 10) syscall_reqio();  // call IO with 10% probability
        else Sleep(10);
    }
}
void proc_2(int id)  // CPU-bound process, Do computation only, No IO Request
{
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        Sleep(10);
    }
}
void proc_3(int id)
{
    int r;
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        r = rand() % 100;
        if (r < 30) syscall_reqio();  // call IO with 30% probability
        else Sleep(10);
    }
}
void proc_4(int id)
{
    int r;
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        r = rand() % 100;
        if (r < 50) syscall_reqio();  // call IO with 50% probability
        else Sleep(10);
    }
}
void proc_5(int id)
{
    int r;
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        r = rand() % 100;
        if (r < 5) syscall_reqio();  // call IO with 5% probability
        else Sleep(10);
    }
}
void proc_6(int id)  // CPU-bound process, Do computation only, No IO Request
{
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        Sleep(10);
    }
}
void proc_7(int id)  // CPU-bound process, Do computation only, No IO Request
{
    int r;
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        r = rand() % 100;
        if (r < 90) syscall_reqio();  // call IO with 90% probability
        else Sleep(10);
    }
}
void proc_8(int id)  // CPU-bound process, Do computation only, No IO Request
{
    int r;
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        r = rand() % 100;
        if (r < 70) syscall_reqio();  // call IO with 70% probability
        else Sleep(10);
    }
}
void proc_9(int id)  // CPU-bound process, Do computation only, No IO Request
{
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        Sleep(10);
    }
}

int main()
{
    std::cout << "�ü�� ���� 1: �����ٸ� ���ΰ��� 1" << '\n' << "�̸�: �ȱ���" << '\n' << "�й�: 2017280049" << '\n' << "������: 2022.04.06." << '\n';
    proc_tbl_t* p;

    run_q.next = run_q.prev = &(run_q);
    block_q.next = block_q.prev = &(block_q);
    cur_proc = -1;

    // �̰� IO_Completion_Interrup() �Լ��� �����ϴ� ������
    p = &(proc_tbl[0]);
    p->id = 0;
    p->priority = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(IO_Completion_Interrupt, 0);
    //    Print_Q(&run_q);

    // ������� ���� ���μ��� ����
    p = &(proc_tbl[1]);
    p->id = 1;
    p->priority = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_1, 1);

    Put_Tail_Q(&run_q, p);
    //    Print_Q(&run_q);

    p = &(proc_tbl[2]);
    p->id = 2;
    p->priority = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_2, 2);

    Put_Tail_Q(&run_q, p);
    //    Print_Q(&run_q);

    p = &(proc_tbl[3]);
    p->id = 3;
    p->priority = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_3, 3);

    Put_Tail_Q(&run_q, p);
    //    Print_Q(&run_q);

    p = &(proc_tbl[4]);
    p->id = 4;
    p->priority = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_4, 4);

    Put_Tail_Q(&run_q, p);

    p = &(proc_tbl[5]);
    p->id = 5;
    p->priority = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_5, 5);

    Put_Tail_Q(&run_q, p);
    //    Print_Q(&run_q);

    p = &(proc_tbl[6]);
    p->id = 6;
    p->priority = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_6, 6);
    Put_Tail_Q(&run_q, p);
    //    Print_Q(&run_q);

    p = &(proc_tbl[7]);
    p->id = 7;
    p->priority = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_7, 7);
    Put_Tail_Q(&run_q, p);
    //    Print_Q(&run_q);

    p = &(proc_tbl[8]);
    p->id = 8;
    p->priority = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_8, 8);
    Put_Tail_Q(&run_q, p);
    //    Print_Q(&run_q);

    p = &(proc_tbl[9]);
    p->id = 9;
    p->priority = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_9, 9);
    Put_Tail_Q(&run_q, p);
    //    Print_Q(&run_q);

    // The member function will be executed in a separate thread

    // Wait for the thread to finish, this is a blocking operation

    // Now Begin Schedule

    sys_timer_int();

    return 0;
}

/*
�� �Լ��� ���� ���μ����� ����� ��ġ�� �����ߴٰ� �����ϰ� �����ϰ�,
sys_scheduler() �Լ��� SCHED_IO_REQ �� �����ؼ� ȣ����
*/
void syscall_reqio()
{
    // Invoke IO Operation, 
    // Then schedule to other process
    std::cout << "\ncurrent process requests I/O";
    sys_scheduler(SCHED_IO_REQ);
}

/*
0.1�� �������� sleep �ߴٰ� Ÿ�̸� ���ͷ�Ʈ�� �߻���Ű��,
���� ���� ���� ���μ����� time_quantum �� ���ҽ�Ų��.
�̸� ���� ���� �ü���� 0.1�ʸ��� Ÿ�̸� ���ͷ�Ʈ�� �߻���Ű�� ��ó�� �Ѵ�.
*/
void sys_timer_int()
{
    proc_tbl_t* p;

    for (;;) {
        std::cout << "\ntimer interrupt\n";
        /* do Time Service */
        if (cur_proc == -1) {                       // ���� ���� ���� ���μ����� ���ٸ�,
            sys_scheduler(SCHED_QUANTUM_EXPIRED);   // �̰� ȣ���ؼ� �����층�� �����Ѵ�.
        }
        else {
            p = &(proc_tbl[cur_proc]); // ���� ���� ���� ���μ����� ������
            p->time_quantum--;         // time_quantum �� 1 ���ҽ�Ų��.
            if (p->time_quantum <= 0)  // ���� �� ���� 0�� �ȴٸ�,
            {
                std::cout << "\ntimes up";
                sys_scheduler(SCHED_QUANTUM_EXPIRED); // �̰� ȣ���ؼ� �����층�� �����Ѵ�.
            }
        }
        Sleep(100);
    }
}

// �����ٸ�
void sys_scheduler(int why)
{
    proc_tbl_t* p; // ���μ��� ���� ����

    /* �л����� �� �Լ������� �ڵ带 �ۼ��ؾ� �Ѵ� */    
    // �̰� ��ȣ������ ���� Peterson �� �ع��̴�.
    interested[TIMER_TURN] = true;
    turn = TIMER_TURN;
    while (interested[IO_TURN] && turn == TIMER_TURN);
    // why �� SCHED_IO_REQ ���, IO ��û�� ���̹Ƿ�, ���� ������ ���μ��� ��Ͻ�Ű��
    if (why == SCHED_IO_REQ)
    {
        if (cur_proc != -1)            // ���� �������� ���μ����� �ִٸ�
        {
            p = &(proc_tbl[cur_proc]); // �ش� ���μ�����
            p->state = PROCESS_BLOCK;  // ��� ���·� �����
            Put_Tail_Q(&block_q, p);   // block_q �� �ڿ� ������
            std::cout << '\n' << p->id << " process' state changes to block state\n";
        }
    }
    // why �� SCHED_QUANTUM_EXPIRED ���, �ð� �Ҵ��� �ٵ� ���̹Ƿ�, ���� ������ ���μ��� �غ���·� �����
    else
    {
        if (cur_proc != -1) {          // ���� �������� ���μ����� �ִٸ�
            p = &(proc_tbl[cur_proc]); // �ش� ���μ�����
            p->state = PROCESS_READY;  // READY ���·� �����
            Put_Tail_Q(&run_q, p);     // ready_q �� �ڿ� ������
            std::cout << '\n' << p->id << " process' state changes to ready state\n";
        }
    }
    // ���ο� ���μ��� ������ �� ������
    Print_Q(&run_q, TYPE_READY);   // ���� ready_q �� �ִ� ���μ����� ���
    p = Get_Head_Q(&run_q);        // ready_q ���� �� �� ���μ��� ������        
    if (p == NULL) {               // ���μ����� �������� ������
        cur_proc = -1;             // �� ���� -1�� �ٲٰ� ����
        return;
    }
    cur_proc = p->id;              // ���μ����� �ִٸ�, �� ���μ��� id ������ cur_proc ��������
    p->state = PROCESS_RUN;        // ���μ��� ���¸� RUN ���� �ٲ�
    p->time_quantum = TIME_QUANTUM;// �ð� �Ҵ緮 �ֱ�
    std::cout << p->id << " process' state changes to run state\n";
    // �̰� ȣ���ϸ� ���� ������ �����ϴ� ���� ���μ����� ������ ���߰�,
    // ���ο� ���� ���μ����� PROCESS_RUN ���°� �Ǿ� ������ �����Ѵ�.
    cv.notify_all();   // switch to process p->id and run */
    // ��ȣ������ ���� ��
    interested[TIMER_TURN] = false;
}
/* IO Interrupt Generator */
void IO_Completion_Interrupt(int id)
{
    proc_tbl_t* p;

    for (;;) { // ���ѹݺ��ϸ� �ֱ������� ���ͷ�Ʈ �߻���Ŵ
        /* �л����� �̰��� �ڵ带 �ۼ��ؾ� �Ѵ� */
        // ��ȣ������ ���� ��
        interested[IO_TURN] = true;
        turn = IO_TURN;
        while (interested[TIMER_TURN] && turn == IO_TURN);
        std::cout << "\nI/O interrupt";
        Print_Q(&block_q, TYPE_BLOCK);// ���� block_q �� �ִ� ���μ����� ���
        p = Get_Head_Q(&block_q);     // block_q ���� I/O ��û�� �Ϸ�Ǳ⸦ ��ٸ���, �� ���� ���� ���μ��� �ϳ��� ������.
        if (p != NULL)                // ���� ����⸦ ��ٸ��� ��ϵ� ���μ����� �ִٸ�
        {
            p->state = PROCESS_READY; // ���μ����� ���¸� PROCESS_READY �� �����
            Put_Tail_Q(&run_q, p);    // ready_q �� ���� �߰���
            std::cout << p->id << " process wakes up and its state changes to ready state\n";
        }
        // ��ȣ������ ���� ��
        interested[IO_TURN] = false;
        Sleep(IO_PERIOD);             // ���� �ֱ�(IO_PERID ��) ��ŭ sleep �Ѵ�.
    }
}

void Put_Tail_Q(proc_tbl_t* head, proc_tbl_t* item)
{
    (head->mu_lock).lock();
    item->prev = head->prev;
    head->prev->next = item;
    item->next = head;
    head->prev = item;
    (head->mu_lock).unlock();
}

proc_tbl_t* Get_Head_Q(proc_tbl_t* head)
{
    proc_tbl_t* item;

    (head->mu_lock).lock();
    if (head->next == head) {
        (head->mu_lock).unlock();
        return NULL;
    }
    item = head->next;

    item->next->prev = head;
    head->next = item->next;

    (head->mu_lock).unlock();
    return item;
}

void Print_Q(proc_tbl_t* head, int type)
{
    if (type == TYPE_READY)
        std::cout <<"\ncurrent processes in ready_q : [ ";
    else
        std::cout << "\ncurrent processes in block_q : [ ";
    proc_tbl_t* item;
    item = head->next;
    while (item != head) {
        std::cout << item->id << ' ';
        item = item->next;
    }
    std::cout << "]\n";
}

// ���α׷� ����: <Ctrl+F5> �Ǵ� [�����] > [��������� �ʰ� ����] �޴�
// ���α׷� �����: <F5> Ű �Ǵ� [�����] > [����� ����] �޴�

// ������ ���� ��: 
//   1. [�ַ�� Ž����] â�� ����Ͽ� ������ �߰�/�����մϴ�.
//   2. [�� Ž����] â�� ����Ͽ� �ҽ� ��� �����մϴ�.
//   3. [���] â�� ����Ͽ� ���� ��� �� ��Ÿ �޽����� Ȯ���մϴ�.
//   4. [���� ���] â�� ����Ͽ� ������ ���ϴ�.
//   5. [������Ʈ] > [�� �׸� �߰�]�� �̵��Ͽ� �� �ڵ� ������ ����ų�, [������Ʈ] > [���� �׸� �߰�]�� �̵��Ͽ� ���� �ڵ� ������ ������Ʈ�� �߰��մϴ�.
//   6. ���߿� �� ������Ʈ�� �ٽ� ������ [����] > [����] > [������Ʈ]�� �̵��ϰ� .sln ������ �����մϴ�.
