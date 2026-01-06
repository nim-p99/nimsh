//   Nimesh's Shell  //   NIMSH 


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


//Function declarations for builtin shell commands:

int nimsh_cd(char **args);
int nimsh_help(char **args);
int nimsh_exit(char **args);

// List of builtin commands, followed by their corresponding functions
char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[]) (char**) = {
	&nimsh_cd,
	&nimsh_help,
	&nimsh_exit
};

int nimsh_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

// builtin function implementations
int nimsh_cd(char **args)
{
	if (args[1] == NULL) {
		fprintf(stderr, "nimsh: expected argument to \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("nimsh");
		}
	}
	return 1;
}

int nimsh_help(char **args)
{
	int i;
	printf("NIMSH (Nimesh Shell)\n");
	printf("Enter program names and arguments\n");
	printf("Built in commands:\n");
	
	for (i = 0; i < nimsh_num_builtins(); i++) {
		printf(" %s\n", builtin_str[i]);
	}
	printf("use command 'man' for info on other programs.\n");
	return 1;
}

int nimsh_exit(char **args)
{
	return 0;
}

int nimsh_launch(char **args)
{
	pid_t pid, wpid;
	int status;
	
	pid = fork();
	if (pid == 0) {
		// child process
		if (execvp(args[0], args) == -1) {
			perror("nimsh");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		// error forking
		perror("nimsh");
	} else {
		// parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

int nimsh_execute(char **args)
{
	int i;
	
	if (args[0] == NULL) {
		// empty command was entered
		return 1;
	}
	
	for (i = 0; i < nimsh_num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}
	
	return nimsh_launch(args);
}

#define NIMSH_RL_BUFSIZE 1024
char *nimsh_read_line(void)
{
  char *line = NULL;
  ssize_t bufsize = 0;

  if (getline(&line, &bufsize, stdin) == -1){
    if (feof(stdin)) {
        exit(EXIT_SUCCESS);
    } else {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }
  return line;
}



#define NIMSH_TOK_BUFSIZE 64
#define NIMSH_TOK_DELIM " \t\r\n\a"
char **nimsh_split_line(char *line)
{
	int bufsize = NIMSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char));  // array of strings
	char *token;  // string token;
	
	if (!tokens) {
		fprintf(stderr, "nimsh: allocation error\n");
		exit(EXIT_FAILURE);
	}
	
	token = strtok(line, NIMSH_TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;
		
		if (position >= bufsize) {
			bufsize += NIMSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "nimsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, NIMSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

void nimsh_loop(void)
{
	char *line;
	char **args;
	int status;
	
	do {
		printf("> ");
		line = nimsh_read_line();
		args = nimsh_split_line(line);
		status = nimsh_execute(args);
		
		free(line);
		free(args);
	} while (status);
}

int main(int argc, char **argv)
{
	// load config files, if any
  	
	// run command loop
	nimsh_loop();
	
	// perform any shutdown/ cleanup
	printf("thank you for using Nim's shell.");
	return EXIT_SUCCESS;
}
