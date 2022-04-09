//proc_tbl_t* p;
//int size = 0;
//int tmp_proc = -1;
//for (;;) // 무한루프를 돌면서 주기적으로 각 프로세스의 wait 값(현재 우선순위 큐에서 대기한 시간)을 확인한다.
//{
//    std::cout << "\nAging Interrupt\n";
//    // 현재 실행중인 프로세스는 잠깐 멈춤
//    if (cur_proc != -1) {
//        tmp_proc = cur_proc;
//        cur_proc = -1;
//        std::cout << "\ncurrent process now bit sleep\n";
//        cv.notify_all();
//        age_cv.notify_all();
//    }
//
//    // 모든 프로세스에 대해서 wait 값을 1초마다 증가시킴
//    // 이때 블록큐에 있는 건 굳이 안 올려도 되므로, 하지 않음
//    for (int i = 0; i < NUMBER_OF_PRIORITY; i++)
//    {
//        // 각 우선순위 큐에 있는 것들 모두 검사해서 체크해야 함....
//        // 어케 구현할진 모르겠다만...
//        size = Size_Q(&(ready_q[i]));
//        std::cout << "\nSize = " << size << '\n';
//        for (int j = 0; j < size; j++) // 현재 우선순위 큐의 크기만큼 탐색함
//        {
//            p = Get_Head_Q(&(ready_q[i])); // 각 우선순위 순서대로 ready_q 에서 맨 앞에 있는 프로세스 가져옴
//            p->wait += 1;                  // 각 프로세스마다 wait 값을 1 증가시킨다.
//            std::cout << p->id << " process waited " << p->wait << " times\n";
//            // 이때 만일 이 프로세스가 준비 큐에 있고, wait 값이 MAX 를 넘어섰다면,
//            if (p->state == PROCESS_READY && p->wait > MAX_WAIT)
//            {
//                if (p->priority_level > 0) // 큐에 대한 우선순위가 0보다 크다면(그보다 낮으면 안 되므로)
//                    p->priority_level -= 1; // 우선순위 하나 높여준다.
//                p->wait = 0;                // wait 값을 0으로 설정해준다.
//                Put_Tail_Q(&(ready_q[p->priority_level]), p); // 새롭게 우선순위에 맞는 큐에 삽입하기
//                std::cout << "\nCurrent process is changed priority\n";
//            }
//            else                              // 그 경우가 아니라면
//            {
//                Put_Tail_Q(&(ready_q[i]), p); // 다시 원래 큐에 집어넣기
//            }
//        }
//    }
//    // 아까 멈췄던 프로세스 다시 시작
//    if (tmp_proc != -1) {
//        cur_proc = tmp_proc;
//        std::cout << "\ncurrent process now bit awake\n";
//        cv.notify_all();
//        age_cv.notify_all();
//    }
//
