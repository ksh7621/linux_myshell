#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#define MAX_CMD_ARG 10
#define BUFSIZE 256

const char* prompt = "myshell> ";
char* cmdvector[MAX_CMD_ARG];
char  cmdline[BUFSIZE];

void fatal(char* str) {
	perror(str);
	exit(1);
}

int makelist(char* s, const char* delimiters, char** list, int MAX_LIST) {
	int i = 0;
	int numtokens = 0;
	char* snew = NULL;

	if ((s == NULL) || (delimiters == NULL)) return -1;

	snew = s + strspn(s, delimiters);	/* Skip delimiters */
	if ((list[numtokens] = strtok(snew, delimiters)) == NULL)
		return numtokens;

	numtokens = 1;

	while (1) {
		if ((list[numtokens] = strtok(NULL, delimiters)) == NULL)
			break;
		if (numtokens == (MAX_LIST - 1)) return -1;
		numtokens++;
	}
	return numtokens;
}

int main(int argc, char** argv) {
	int i = 0;
	pid_t pid;
	int bgflag = 0; //foreground
	char* copy_cmd[MAX_CMD_ARG];

	while (1) {
		fputs(prompt, stdout); //prompt 출력
		fgets(cmdline, BUFSIZE, stdin); //입력
		cmdline[strlen(cmdline) - 1] = '\0';

		makelist(cmdline, "", copy_cmd, MAX_CMD_ARG); //복사				

		char tmp_cmd[10];
		strcpy(tmp_cmd, copy_cmd[0]); //copy_cmd 복사 tmp_cmd로
		int cnt = makelist(tmp_cmd, " ", cmdvector, MAX_CMD_ARG); // " "기준 
		
		
		if (strcmp(cmdvector[0], "cd") == 0) {
			chdir(cmdvector[1]);
			continue;
		}

		if (strcmp(cmdvector[0], "exit") == 0) {
			return 0;
		}

		if (strcmp(cmdvector[cnt - 1], "&") == 0)
		{
			bgflag = 1;
			copy_cmd[0][strlen(copy_cmd[0]) - 1] = '\0';
		}

		switch (pid = fork()) { //프로세스 생성
		case 0:
			makelist(copy_cmd[0], " \t", cmdvector, MAX_CMD_ARG);
			execvp(cmdvector[0], cmdvector); 	
			fatal("main()");
			break;
		case -1:
			fatal("main()");
		default:
			if (bgflag == 0) wait(NULL); // 부모 대기	
			fflush(stdout);
			break;
		}

	}

	return 0;
}
