#include <stdio.h> 
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	printf("Proc ID is: %d\n", (int)getpid());

	//      Создаем копию текущего процесса (вместе с адресным пр-вом, ссылками на дескрипторы файлов. 
	// На страницы (mmap'нутой памяти) ставится защита от записи. 
	// При попытке изменения бросается исключение, дублируется страница (если стоял флагPRIVATE).)
	pid_t pid = fork(); // В родителе вернет pid дочернего, в дочернем процессе 0

    switch (pid) {
        case -1:
			perror("Fork err:");
			return -1;

		case 0:
			printf("We are in child. Proc ID is: %d\n", (int)getpid());
			printf("Cat'ting file..\n");

            //      Заменяем программу в текущем процессе. 
            // Те запускаем main программы - первого параметра.
            // (unmup'аем сегменты и копируем аргументы. Создаем новое адрессное пространство. 
            // Не всегда скопируются (те останутся открытыми) файлы, зависит от состояния флага close-on-exec)
            // Не возвращает управление.
			execlp("cat", "cat", argv[1], NULL);

		default: 
			int stat;
			printf("We are in parent. Child pid is: %d\n", (int)pid);
			// Ждем завершения дочернего 
			// (завершение/завершение по сигналу/появление сигнала, требующего функции обработчика)
			// Получаем код возврата процесса (Через stat);
			// Ждет всех потомков, возвращает номер одного из заверенных
			if(wait(&stat) == -1) {
				perror("Wait err:");
				return -1;
			}

			printf("\nEnding parent..\n");        
	}

	return 0;
}