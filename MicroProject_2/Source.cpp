#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <unistd.h>

// Состояния программиста.
enum StateE {
    START,
    WRITING,
    DEBUGING,
    REVIEWING,
    PUSHING_TASK,
    WAIT_TASK,
    CHOOSE_TASK,
    COMPLETE,
    END_TASKS,
    END_WORK,
    NULL_STATE
};

// Типы заданий.
enum class TaskE {
    REVIEW_ACCEPT,
    DEBUG,
    REVIEW, 
};

// Структура описывающая задание.
struct Task {
    TaskE task;
    int requester;
    int executer;
};

// Структура с данными для потока.
struct Info {
  int taskAmt;
  int myNumber;  
  pthread_mutex_t* wrMutex;
  pthread_mutex_t* workMutex;
};

// Количество программистов.
int progersAmt = 3;
// Количество программистов, которые закончили работу.
int readyProgers = 0;

// Очередь с заданиями для каждого программиста.
std::vector<std::queue<Task>> tasks(progersAmt, std::queue<Task>());
// Семафор для каждого программиста.
std::vector<sem_t> sems(progersAmt, sem_t());

// Оперетор вывода типа задания.
std::ostream& operator<< (std::ostream& str, const TaskE& task){
    switch (task)
    {
    case TaskE::REVIEW:
        str<< "REVIEW task";
        break;
    case TaskE::DEBUG:
        str<< "DEBUG task";
        break;
    case TaskE::REVIEW_ACCEPT:
        str<< "REVIEW ACCEPT";
        break;
    
    default:
        break;
    } 
    return str;
}

// Метод, описывающий программиста.
void* proger(void* param){
    // Извлечение данных.
    Info* info = (Info*)param;
    int taskAmt = info->taskAmt;
    int myNum = info->myNumber;
    pthread_mutex_t* wrMut = info->wrMutex;
    pthread_mutex_t* workMut = info->workMutex;

    // Переменные, описывающие состояние.
    int val =0;
    StateE curState = StateE::START;
    int completedTask;
    int myRequester;
    Task newTask;
    Task myTask;
    bool almostWork = true;


    while(almostWork){
        // Заблокируем мьютекс для вывода сообщений.
        pthread_mutex_lock(wrMut);
        switch (curState){
        case StateE::START:
            std::cout << "Programmer : " 
                << myNum << ", start working" <<  std::endl;

            break;

        case StateE::WRITING:
            std::cout << "Programmer : " << myNum 
                << ", start writing task : " << completedTask + 1 <<  std::endl;
            break;

        case StateE::REVIEWING:
            std::cout << "Programmer : " << myNum 
                << ", start reviewing task from programmer : " << myRequester 
                <<  std::endl;
            break;

        case StateE::DEBUGING:
            std::cout << "Programmer : " << myNum 
                << ", start debugging task : " << completedTask+1 <<  std::endl;
            break;

        case StateE::PUSHING_TASK:
            std::cout << "Programmer : " << myNum 
                << ", send " << newTask.task << " to " 
                << newTask.executer << std::endl;
            break;

        case StateE::WAIT_TASK:
            // Если семафор отрицательный, значит программист будет ждать (спать).
            sem_getvalue(&sems[myNum],&val);
            if(val <= 0){
                std::cout << "Programmer : " << myNum 
                    << ", wait new task (sleep)" << std::endl;
            }
            break;

        case StateE::CHOOSE_TASK:
            // Если заданий нет, это знак, что можно идти домой.
            if(tasks[myNum].empty()){
                std::cout << "Programmer : " << myNum 
                    << ", ends his work for today" << std::endl;
                break;
            }
            std::cout << "Programmer : " << myNum 
                << ", get "<< tasks[myNum].front().task << std::endl;
            break;

        case StateE::END_TASKS:
            std::cout << "Programmer : " << myNum 
                << ", dont have any tasks for today and wait for review requests"
                << std::endl;
            break;
        case StateE::END_WORK:
            std::cout << "Programmer : " << myNum << ", go home" << std::endl;
            break;
        default : break;
        }
        pthread_mutex_unlock(wrMut);

        switch (curState) {
        case StateE::START:
            // Проинициализируем состояние.
            completedTask = 0;
            curState = StateE::WRITING;
            break;

        case StateE::WRITING:
            // Пишем код и отправляем его случайному программисту (кроме себя).
            usleep(1000);
            newTask = Task{ TaskE::REVIEW, myNum, 
                (myNum + rand() % (progersAmt-1) + 1) % progersAmt };
            curState = StateE::PUSHING_TASK;
            break;

        case StateE::REVIEWING:
            // Ревьюим код и с вероятностью 0.7 принимаем его.
            usleep(500);
            newTask = Task{ 
                (rand() % 10 < 7) ? TaskE::REVIEW_ACCEPT : TaskE::DEBUG,
                myNum,
                myRequester};
            curState = StateE::PUSHING_TASK;
            break;

        case StateE::DEBUGING:
            // Исправляем код и отправляем его программисту, 
            // который ревьюил до этого.
            usleep(1500);
            newTask = Task{TaskE::REVIEW, myNum, myRequester};
            curState = StateE::PUSHING_TASK;
            break;

        case StateE::PUSHING_TASK:
            // Вставляем в очередь заданий Исполнитея 
            // задание и взводим ему семафор.
            tasks[newTask.executer].push(newTask);
            sem_post(&sems[newTask.executer]);
            curState = StateE::WAIT_TASK;
            break;

        case StateE::WAIT_TASK:
            // Ждём, когда кто-нибудь взведёт нам семафор.
            sem_wait(&sems[myNum]);
            curState = StateE::CHOOSE_TASK;
            break;

        case StateE::CHOOSE_TASK:
            // Если очередь заданий пуста, то нам пора домой.
            if(tasks[myNum].empty()){
                curState = StateE::END_WORK;
                break;
            }
            myTask = tasks[myNum].front();
            tasks[myNum].pop();

            myRequester = myTask.requester;
            if (myTask.task == TaskE::REVIEW_ACCEPT) completedTask++;
            // Проверяем не закончили ли мы все задачи.
            if (completedTask == taskAmt && myTask.task== TaskE::REVIEW_ACCEPT) {
                pthread_mutex_lock(workMut);
                curState = StateE::END_TASKS;
                readyProgers++;
                // Если мы последний программист, который закончил работу,
                // то взведём всем семафоры с пустой очередью и они уйдут.
                if (readyProgers == progersAmt) {
                    for (int i = 0; i < progersAmt; i++){
                        sem_post(&sems[i]);
                    }
                }
                pthread_mutex_unlock(workMut);
                break;
            }
            curState = 
                (myTask.task == TaskE::DEBUG) ? StateE::DEBUGING : 
                (myTask.task == TaskE::REVIEW) ? StateE::REVIEWING : 
                (myTask.task == TaskE::REVIEW_ACCEPT) ? StateE::WRITING : 
                StateE::NULL_STATE;
            break;
        case StateE::END_TASKS:
            // Если мы закончили все задания, то выводим сообщение и ждём.
            curState = StateE::WAIT_TASK;
            break;
        case StateE::END_WORK:
            // Закончили работу, идём домой.
            almostWork = false;
            curState = StateE::NULL_STATE;
            break;
        default: break;
        }
    }

    return nullptr;
}


int main(int argc, char *argv[]) {
    // Считаем количестов задач каждому программисту.
    std::vector<int> tasksAmt(3);
    std::cout << "Enter the tasks amount for three programmers (from 1 to 10): ";
    for (int i = 0; i < 3; i++){
        std::cin >> tasksAmt[i]; 
        if (tasksAmt[i] < 1 || tasksAmt[i] > 10){
            std::cout << std::endl << "Incorrect tasks amount";
            return 0;
        }
    }
    std::cout << std::endl;

    std::ofstream ofs;
    // Если в аргументах указали путь к фалу, то будем писать в него.
    if (argc == 2){
        std::cout << "Result will be written to file: " << argv[1] << std::endl; 
        ofs = std::ofstream(argv[1]);
        std::cout.rdbuf(ofs.rdbuf());
    }
    

    // Инициализируем семафоры и мьютексы.
    for(int i = 0; i < progersAmt; i++){
        sem_init(&sems[i], 0, 0);
    }
    pthread_mutex_t mutex, workMutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&workMutex, NULL);

    // Запускаем потоки.
    std::vector<pthread_t> threads = 
                                std::vector<pthread_t>(progersAmt, pthread_t());
    Info* info = new Info[progersAmt];
    for(int i = 0; i < progersAmt; i++){
        info[i] = Info{ tasksAmt[i], i, &mutex, &workMutex };
        pthread_create(&threads[i], NULL, proger, &info[i]);
    }

    // Освобождаем ресурсы.
    for(int i = 0; i < progersAmt; i++){
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&workMutex);
    delete[] info;
    ofs.close();

    return 0;
}