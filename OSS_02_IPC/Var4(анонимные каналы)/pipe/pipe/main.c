#define _CRT_SECURE_NO_WARNINGS


#define WINDIR 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

#define LEN 256

int show_err(const char * msg);


int main() {	
	/*setlocale(LC_ALL, "russian");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);*/


	/*дескрипторы каналов*/
	HANDLE fd_r_1, fd_w_1, fd_r_2, fd_w_2;
	fd_r_1 = fd_w_1 = fd_r_2 = fd_w_2 = INVALID_HANDLE_VALUE;

	while(TRUE) {
		/*создаем два анонимных канала*/		
		BOOL isOk = CreatePipe(&fd_r_1,  /*дескриптор для чтения*/
							   &fd_w_1,  /*дескриптор для записи*/
							   NULL,     /*доступ AD*/
							   LEN         /*размер канала в байтах (0-значение OS поумолчанию)*/);
		if(!isOk) {
			show_err("Не удалось создать анономный канал");
			break;
		} 

		isOk = CreatePipe(&fd_r_2,  /*дескриптор для чтения*/
		                  &fd_w_2,  /*дескриптор для записи*/
		                  NULL,     /*доступ AD*/
		                  LEN         /*размер канала в байтах (0-значение OS поумолчанию)*/);

		if(!isOk) {
			show_err("Не удалось создать анономный канал");
			break;
		}

		/*делаем не блокирующим конец, из которого будем читать (т.к. не знаем, сколько нужно прочитать)*/
		int lpMode = PIPE_NOWAIT;
		isOk = SetNamedPipeHandleState(fd_r_2,
									   &lpMode,    /*не блокирующий*/
									   NULL,       /*максимальное количество байт накопленных перед отправкой*/
									   NULL        /*максимальная задержка на сбор байт перед отправкой*/);
		if(!isOk) {
			show_err("Не удалось включить не блокирующий режим");
			break;
		}

		/*включаем наследование для тех концов канала, которые должны быть у потомка*/
		isOk = SetHandleInformation(fd_r_1,                  /* дескриптор канала*/
									HANDLE_FLAG_INHERIT ,    /* заменяемые флажки (здесь наследование и закрытие дескриптора)*/
									1                        /* новые значения флажков*/);
		if(!isOk) {
			show_err("Не удалось отключить наследование дескриптору канала");
			break;
		}
				
		isOk = SetHandleInformation(fd_w_2,                  /* дескриптор канала*/
									HANDLE_FLAG_INHERIT,     /* заменяемые флажки (здесь наследование и закрытие дескриптора)*/
									1                        /* новые значения флажков*/);
		if(!isOk) {
			show_err("Не удалось отключить наследование дескриптору канала");
			break;
		} 

		
		/*DWORD flags;
		isOk = GetHandleInformation(fd_r_1,&flags);*/

		/*создаем дочерний процесс*/
		STARTUPINFO sti;				        // структура
		ZeroMemory(&sti, sizeof(STARTUPINFO));	// обнулить
		sti.cb = sizeof(STARTUPINFO);			// указать размер
		/*устанавливаем потомку дескрипторы stdin, stdout и stderr*/
		sti.dwFlags = STARTF_USESTDHANDLES; //!!!обязательно!!!
		sti.hStdInput  = fd_r_1;
		sti.hStdOutput = fd_w_2;
		sti.hStdError  = fd_w_2;

		PROCESS_INFORMATION procInf;
		isOk = CreateProcess("c:\\Windows\\system32\\cmd.exe",   /*путь к программе*/
							 "",      /*параметры коммандной строки*/
							 NULL,    /*доступ AD для нового процесса*/
							 NULL,    /*доступ AD для нового потока*/
							 TRUE,    /*Флаг наследования текущего процесса (разрешаем наследовать дескрипторы)*/
							 CREATE_NEW_CONSOLE /*0*/, /*Флаги способов создания процесса*/
							 NULL,    /*указатель на структуру переменных окружения, для процесса (NULL - значит как у родителя)*/
							 NULL,    /*Текущий диск или каталог (NULL - значит как у родителя)*/
							 &sti,    /*Используется для настройки свойств процесса, например расположения окон и заголовок*/
							 &procInf /*сведения о процессе (сами заполнятся)*/);
		if(!isOk) {
			show_err("Не удалось создать дочерний процесс");
			break;
		} 
			
		BOOL isRepeat = TRUE;
		while(isRepeat) {
			char buf[LEN];
			DWORD len = 0;
			
			while(TRUE) 
				if(ReadFile(fd_r_2, buf, LEN-2, &len, NULL)){
					buf[len] = '\0';
					printf(buf);
					/*проверка конца ввода*/
					if(buf[len - 1] == '>')
						break;					
				} else if(ERROR_BROKEN_PIPE == GetLastError()) {
					/*разрыв канала*/
					show_err("-->");
					isRepeat = FALSE;
					break;
				} else if(GetExitCodeProcess(procInf.hProcess, &len)){
					/*проверяем, что дочерний процесс жив*/
					if(len == STILL_ACTIVE)
						break;
					else {
						printf("Дочерний процесс завершился с кодом %d\n",len);
						isRepeat = FALSE;
						break;
					}
				} else
					show_err("Не удалось проверить дочерний процесс");
											
			if(isRepeat) {
				//printf("\nВведите команду: ");
				gets(buf);
				size_t len_ = strlen(buf);
				if(len_) {
					buf[len_++] = '\n';
					//buf[len + 2] = '\0';
					isOk = WriteFile(fd_w_1, buf, len_, &len, NULL);
					if(!isOk)
						show_err("Ошибка записи в канал");
					else if(ERROR_BROKEN_PIPE == GetLastError()) {
						show_err("Ошибка записи в канал");
						break;
					} else if(len_ != len)
						show_err("Ошибка записи в канал все целиком");
				}
			}
			
		}

		/*убиваем процесс и закрываем его дескриптор*/
		TerminateProcess(procInf.hProcess, NO_ERROR);		
		CloseHandle(procInf.hProcess);

		/*этот финт, чтоб не морочиться с операторами условия выше*/
		break; 		
	}

	/*закрываем дескрипторы*/
	CloseHandle(fd_w_1);
	CloseHandle(fd_r_2);
	CloseHandle(fd_r_1);
	CloseHandle(fd_w_2);

	system("pause");
	return 0;
}

int show_err(const char * msg) {
	int      no = GetLastError();
	char  str_err[10000] = {0};
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
		printf("%s:\n%s\n", msg, str_err);
	else
		printf("%s:\nНомер ошибки %d\n", msg, no);

	return no;
}
