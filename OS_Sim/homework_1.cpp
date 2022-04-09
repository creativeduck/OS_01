// OS_Sim.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
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

#define IO_PERIOD 2000 // IO_Completion_Interrupt 가 sleep 하는 주기, ms 단위

#define TYPE_READY 0   // 레디 큐에 있는 프로세스를 출력하는 것이라고 알림
#define TYPE_BLOCK 1   // 블록 큐에 있는 프로세스를 출력하는 것이라고 알림

#define TIMER_TURN 0   // timer 의 차례
#define IO_TURN 1      // IO의 차례

volatile int cur_proc; // 현재 실행되고 있는 프로세스의 id 값

std::condition_variable cv; // 조건 변수
std::mutex cv_m;            // 뮤텍스

struct proc_tbl_t { // 가상 프로세스
    int id;                  // id
    int priority;            // 우선순위
    int state;               // 상태
    int time_quantum;        // 시간 할당량
    std::thread th;          // 스레드
    std::mutex mu_lock;      // 뮤텍스
    struct proc_tbl_t* prev; // 이전 프로세스
    struct proc_tbl_t* next; // 다음 프로세스
} proc_tbl[10]; // 프로세스 10개 가진 배열

struct proc_tbl_t run_q;    // 준비 큐
struct proc_tbl_t block_q;  // 대기 큐

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
각 프로세스들은 루프를 돌면서 작동한다.
cv.wait() 함수로 cur_proc 가 자신의 번호가 될 때까지 기다리고,
자신의 차례가 되면 시스템 호출을 한다.

CPU-바운드 프로세스의 경우 id만 출력하고 다시 잠들고,
IO-바운드의 경우 syscal_reqio() 호출을 통해 입출력을 요구한다.

이때 IO/CPU 실행비율은 rand 값 범위로 설정한다.
예를 들어, r < 40 으로 설정해주면 40% 확률로 IO 시스템 호출 하도록
*/
void proc_1(int id) // 전달받은 id 값으로 프로세스 생성
{
    int r;
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        // cur_proc 이 여기 id 값과 일치하면, wait에서 깨어나 동작 실행
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
    std::cout << "운영체제 과제 1: 스케줄링 세부과제 1" << '\n' << "이름: 안광민" << '\n' << "학번: 2017280049" << '\n' << "제출일: 2022.04.06." << '\n';
    proc_tbl_t* p;

    run_q.next = run_q.prev = &(run_q);
    block_q.next = block_q.prev = &(block_q);
    cur_proc = -1;

    // 이게 IO_Completion_Interrup() 함수를 실행하는 스레드
    p = &(proc_tbl[0]);
    p->id = 0;
    p->priority = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(IO_Completion_Interrupt, 0);
    //    Print_Q(&run_q);

    // 여기부턴 가상 프로세스 생성
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
이 함수는 가상 프로세스가 입출력 장치를 구동했다고 가정하고 실행하고,
sys_scheduler() 함수에 SCHED_IO_REQ 를 전달해서 호출함
*/
void syscall_reqio()
{
    // Invoke IO Operation, 
    // Then schedule to other process
    std::cout << "\ncurrent process requests I/O";
    sys_scheduler(SCHED_IO_REQ);
}

/*
0.1초 간격으로 sleep 했다가 타이머 인터럽트를 발생시키며,
현재 실행 중인 프로세스의 time_quantum 을 감소시킨다.
이를 통해 가상 운영체제가 0.1초마다 타이머 인터럽트를 발생시키는 것처럼 한다.
*/
void sys_timer_int()
{
    proc_tbl_t* p;

    for (;;) {
        std::cout << "\ntimer interrupt\n";
        /* do Time Service */
        if (cur_proc == -1) {                       // 현재 실행 중인 프로세스가 없다면,
            sys_scheduler(SCHED_QUANTUM_EXPIRED);   // 이걸 호출해서 스케쥴링을 수행한다.
        }
        else {
            p = &(proc_tbl[cur_proc]); // 현재 실행 중인 프로세스를 가져와
            p->time_quantum--;         // time_quantum 을 1 감소시킨다.
            if (p->time_quantum <= 0)  // 만일 이 값이 0이 된다면,
            {
                std::cout << "\ntimes up";
                sys_scheduler(SCHED_QUANTUM_EXPIRED); // 이걸 호출해서 스케쥴링을 수행한다.
            }
        }
        Sleep(100);
    }
}

// 스케줄링
void sys_scheduler(int why)
{
    proc_tbl_t* p; // 프로세스 변수 선언

    /* 학생들은 이 함수내에서 코드를 작성해야 한다 */    
    // 이게 상호배제를 위한 Peterson 의 해법이다.
    interested[TIMER_TURN] = true;
    turn = TIMER_TURN;
    while (interested[IO_TURN] && turn == TIMER_TURN);
    // why 가 SCHED_IO_REQ 라면, IO 요청한 것이므로, 현재 실행중 프로세스 블록시키기
    if (why == SCHED_IO_REQ)
    {
        if (cur_proc != -1)            // 현재 실행중인 프로세스가 있다면
        {
            p = &(proc_tbl[cur_proc]); // 해당 프로세스를
            p->state = PROCESS_BLOCK;  // 블록 상태로 만들고
            Put_Tail_Q(&block_q, p);   // block_q 맨 뒤에 삽입함
            std::cout << '\n' << p->id << " process' state changes to block state\n";
        }
    }
    // why 가 SCHED_QUANTUM_EXPIRED 라면, 시간 할당이 다된 것이므로, 현재 실행중 프로세스 준비상태로 만들기
    else
    {
        if (cur_proc != -1) {          // 현재 실행중인 프로세스가 있다면
            p = &(proc_tbl[cur_proc]); // 해당 프로세스를
            p->state = PROCESS_READY;  // READY 상태로 만들고
            Put_Tail_Q(&run_q, p);     // ready_q 맨 뒤에 삽입함
            std::cout << '\n' << p->id << " process' state changes to ready state\n";
        }
    }
    // 새로운 프로세스 선택한 후 실행함
    Print_Q(&run_q, TYPE_READY);   // 현재 ready_q 에 있는 프로세스들 출력
    p = Get_Head_Q(&run_q);        // ready_q 에서 맨 앞 프로세스 가져옴        
    if (p == NULL) {               // 프로세스가 존재하지 않으면
        cur_proc = -1;             // 이 값을 -1로 바꾸고 리턴
        return;
    }
    cur_proc = p->id;              // 프로세스가 있다면, 그 프로세스 id 값으로 cur_proc 설정해줌
    p->state = PROCESS_RUN;        // 프로세스 상태를 RUN 으로 바꿈
    p->time_quantum = TIME_QUANTUM;// 시간 할당량 주기
    std::cout << p->id << " process' state changes to run state\n";
    // 이걸 호출하면 이제 기존에 실행하던 가상 프로세스는 수행을 멈추고,
    // 새로운 가상 프로세스가 PROCESS_RUN 상태가 되어 실행을 시작한다.
    cv.notify_all();   // switch to process p->id and run */
    // 상호배제를 위한 것
    interested[TIMER_TURN] = false;
}
/* IO Interrupt Generator */
void IO_Completion_Interrupt(int id)
{
    proc_tbl_t* p;

    for (;;) { // 무한반복하며 주기적으로 인터럽트 발생시킴
        /* 학생들은 이곳에 코드를 작성해야 한다 */
        // 상호배제를 위한 것
        interested[IO_TURN] = true;
        turn = IO_TURN;
        while (interested[TIMER_TURN] && turn == IO_TURN);
        std::cout << "\nI/O interrupt";
        Print_Q(&block_q, TYPE_BLOCK);// 현재 block_q 에 있는 프로세스들 출력
        p = Get_Head_Q(&block_q);     // block_q 에서 I/O 요청이 완료되기를 기다리는, 맨 앞의 가상 프로세스 하나를 꺼낸다.
        if (p != NULL)                // 현재 깨어나기를 기다리는 블록된 프로세스가 있다면
        {
            p->state = PROCESS_READY; // 프로세스의 상태를 PROCESS_READY 로 만들고
            Put_Tail_Q(&run_q, p);    // ready_q 맨 끝에 추가함
            std::cout << p->id << " process wakes up and its state changes to ready state\n";
        }
        // 상호배제를 위한 것
        interested[IO_TURN] = false;
        Sleep(IO_PERIOD);             // 일정 주기(IO_PERID 값) 만큼 sleep 한다.
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

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
