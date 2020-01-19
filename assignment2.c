#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
void multipiping(char *args[],int pipes[],int size,int bin, int bout){
	if(size == 2){
		int stat;
		pid_t pi = fork();
		if(pi == 0){
			pid_t po = fork();
		if (po == 0){
				dup2(pipes[1],1);
				close(pipes[0]);
				if(bin == 2){
					int in;
					in = open(args[2], O_RDONLY);
					dup2(in,0);
					close(in);					
				}
				char *cargs[] = {args[0],NULL};
				execvp(args[0], cargs);	
		}
		waitpid(po,&stat, 0);
		dup2(pipes[0],0);
		close(pipes[1]);
		if(bout == 2){
			int out;
			out = open(args[2 + bin + bout], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			dup2(out,1);
			close(out);
		}
		char *gargs[] = {args[2 + bin],NULL};
		execvp(args[2 + bin], gargs);
		}
	}
	else{
	for(int i = 0;i<=size;i=i+2){
		if(fork() == 0){
			if(i == 0){
				if (bin == 2){
					int in;
					in = open(args[2], O_RDONLY);
					dup2(in,0);
					close(in);
				}
			}
			if(i != 0){
				dup2(pipes[i-2],0);
			}
			if(i != size){
				dup2(pipes[i+1],1);
			}
			for (int k = 0;k<size;k++){
				close(pipes[k]);
			}
			if(i == 0){
				char *cat_args[] = {args[i],NULL};
				execvp(args[i], cat_args);
			}
			else{
				if (i == size){
					if(bout == 2){
			int out;
			out = open(args[i + bin + bout], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			dup2(out,1);
			close(out);						
					}
				}
				char *cat_args[] = {args[i + bin],NULL};
				execvp(args[i + bin], cat_args);
			}
		}
	}
	}
}
enum {ARGSIZE = 20};
int main(int argc, char *argv[]){
	char str[100];
	printf("shell>");
	scanf("%[^\n]%*c",str);
	while(1){
          char *args[ARGSIZE],
            *delim = " \n",
            *token;
        int argIndex = 0;
        for (int i = 0; i < ARGSIZE; i++)  
            args[i] = NULL;		
		for (token = strtok (str, delim);        
        token && argIndex + 1 < ARGSIZE; 
        token = strtok (NULL, delim)) {
            args[argIndex++] = token;
        }
        if(argIndex == 1){
		if(strcmp(args[0],"pwd") == 0){
			char s[150];
			printf("%s\n", getcwd(s,150));
		}
		else if(strcmp(args[0],"exit") == 0){
			break;
		}
  		else{
			pid_t p = fork();
			if(p == 0){
				char *sargs[] = {args[0],NULL};
				execvp(args[0],sargs);
				return 0;
			}		
			waitpid(-1, NULL, 0);
		}		
		}
		else if(argIndex == 2){
			if(strcmp(args[0],"cd") == 0){
			chdir(args[1]);
			}
			else if(strcmp(args[0],"mkdir") == 0){
				mkdir(args[1]); 
			}
			else if(strcmp(args[0],"rmdir") == 0){
				rmdir(args[1]); 
			}
			else{}
		}
		else if(argIndex > 2){
			int nm = 0;
			int bin = 0;
			int bout = 0;
			for(int i = 0;i<argIndex;i++){
				if(strcmp(args[i],"|") == 0){
					nm++;
				}
				if(strcmp(args[i],"<") == 0){
					bin = 2;
				}
				if(strcmp(args[i],">") == 0){
					bout = 2;
				}	
			}
			if(strcmp(args[1],"|") == 0 || (argIndex >= 4 && strcmp(args[3],"|") == 0)){  			
			int status;
  			int size = nm*2;
  			int pipes[size];
  			for(int k = 0;k < size/2;k++){
  				pipe(pipes + 2*k);
  			}
  			multipiping(args, pipes, size, bin, bout);
  			for (int k = 0;k<size;k++){
  				close(pipes[k]);
  			}
  			for (int i = 0; i < 5; i++){
    			wait(&status);
  			}  			
			}
			else{ 
			if(strcmp(args[1],"<") == 0 || strcmp(args[1],">") == 0){ 
			pid_t p = fork();
			if(p == 0){
			int in, out;
  			char *grep_args[] = {args[0], NULL};
  			if(strcmp(args[1],"<") == 0){
  				in = open(args[2], O_RDONLY);	
  				dup2(in, 0);
  			}
  			if(strcmp(args[1],">") == 0){
  				out = open(args[2], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
  				dup2(out, 1);
  			}
  			if(argIndex == 5 && strcmp(args[3],">") == 0){
  				out = open(args[4], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
  				dup2(out, 1);
  			}
  			close(in);  			
  			close(out);
  			execvp(args[0], grep_args);
  			}
  			waitpid(-1, NULL, 0);
			} 
  			}
  		}
  		else{}
		printf("%s" , "shell>");
		scanf("%[^\n]%*c",str);
	}
}
