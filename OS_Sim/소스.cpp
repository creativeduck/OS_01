//proc_tbl_t* p;
//int size = 0;
//int tmp_proc = -1;
//for (;;) // ���ѷ����� ���鼭 �ֱ������� �� ���μ����� wait ��(���� �켱���� ť���� ����� �ð�)�� Ȯ���Ѵ�.
//{
//    std::cout << "\nAging Interrupt\n";
//    // ���� �������� ���μ����� ��� ����
//    if (cur_proc != -1) {
//        tmp_proc = cur_proc;
//        cur_proc = -1;
//        std::cout << "\ncurrent process now bit sleep\n";
//        cv.notify_all();
//        age_cv.notify_all();
//    }
//
//    // ��� ���μ����� ���ؼ� wait ���� 1�ʸ��� ������Ŵ
//    // �̶� ���ť�� �ִ� �� ���� �� �÷��� �ǹǷ�, ���� ����
//    for (int i = 0; i < NUMBER_OF_PRIORITY; i++)
//    {
//        // �� �켱���� ť�� �ִ� �͵� ��� �˻��ؼ� üũ�ؾ� ��....
//        // ���� �������� �𸣰ڴٸ�...
//        size = Size_Q(&(ready_q[i]));
//        std::cout << "\nSize = " << size << '\n';
//        for (int j = 0; j < size; j++) // ���� �켱���� ť�� ũ�⸸ŭ Ž����
//        {
//            p = Get_Head_Q(&(ready_q[i])); // �� �켱���� ������� ready_q ���� �� �տ� �ִ� ���μ��� ������
//            p->wait += 1;                  // �� ���μ������� wait ���� 1 ������Ų��.
//            std::cout << p->id << " process waited " << p->wait << " times\n";
//            // �̶� ���� �� ���μ����� �غ� ť�� �ְ�, wait ���� MAX �� �Ѿ�ٸ�,
//            if (p->state == PROCESS_READY && p->wait > MAX_WAIT)
//            {
//                if (p->priority_level > 0) // ť�� ���� �켱������ 0���� ũ�ٸ�(�׺��� ������ �� �ǹǷ�)
//                    p->priority_level -= 1; // �켱���� �ϳ� �����ش�.
//                p->wait = 0;                // wait ���� 0���� �������ش�.
//                Put_Tail_Q(&(ready_q[p->priority_level]), p); // ���Ӱ� �켱������ �´� ť�� �����ϱ�
//                std::cout << "\nCurrent process is changed priority\n";
//            }
//            else                              // �� ��찡 �ƴ϶��
//            {
//                Put_Tail_Q(&(ready_q[i]), p); // �ٽ� ���� ť�� ����ֱ�
//            }
//        }
//    }
//    // �Ʊ� ����� ���μ��� �ٽ� ����
//    if (tmp_proc != -1) {
//        cur_proc = tmp_proc;
//        std::cout << "\ncurrent process now bit awake\n";
//        cv.notify_all();
//        age_cv.notify_all();
//    }
//
