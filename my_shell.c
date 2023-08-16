#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
int bg=0;
int killfg=0;
pid_t freturn,bfreturn;

/* Splits the string by space and returns the array of tokens
*
*/

char **tokenize(char *line)
{
	bg=0;
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	if(strcmp(token,"&")==0 && i+1==strlen(line)){
		bg=1;
		continue;
	}
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

struct bgnode
{
	int proid;
	int status;
	struct bgnode *next;
};
struct bgnode *head=NULL;


int latest;
void handler(int sig)                                                      //SIGINT'S handler function
{
	
	kill(latest,SIGTERM);
	printf("\n");
	wait(-1,NULL,0);

}



int main(int argc, char* argv[]) 
{
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i,total;
	pid_t bpwait=9;
	pid_t reapvar;
	while(1) {
		
					
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();
		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
		
		struct sigaction san;             //sigaction stucture.
		san.sa_handler=handler;
		san.sa_flags=0;
		sigemptyset (&san.sa_mask) ;
		sigaction(SIGINT,&san,NULL);


    if(tokens[0]==NULL)        // if we hit enter return to the prompt
    	continue;

		if(tokens[0]!=NULL)
		{							
							char *cd="cd";
							if(!strcmp(tokens[0],"exit"))              //exit command implementation.
										{
												struct bgnode *x;
												x=head;
												while(x!=NULL)
												{
													kill(x->proid,SIGTERM);        //terminating each bg process cleanly.
													x=x->next;
												}
												free(head);
												break;
										}
					    if(!strcmp(tokens[0],cd))												// implementation of cd command
						    {
									 if(tokens[2]==NULL)
								{ 
									 int cdstat=9;
								         cdstat=chdir(tokens[1]);
								         if(cdstat==-1)
								         {
								            printf("Shell:Incorrect command \n");
								         }
							        }
							     else
								     {
								     	printf("bash : cd : too many arguments\n");
								     }
						    }	


				else
					{				     	//to handle commands which have executables ready at bin folder(bg and fg)										
							pid_t fretval,wretp;							
							fretval=fork();
							if(fretval==0) // this is child process (either bg or fg)
							{								
								execvp(tokens[0],tokens);
								printf(" Sorry ! command not found \n");
								kill(getpid(),SIGTERM);
								waitpid(-1,NULL,WNOHANG);
							}
							else           // this parent process (either bg or fg)
							{
								if(bg==0)
								{
									latest=fretval;                // if it is a parent of foreground processs,parent has to block till it is teminated then reap it
									wretp=waitpid(fretval);
																		
									if(head!=NULL)                                      
									{
										struct bgnode *trav;
										trav=head;
										int flag=-3;
										while(trav!=NULL)                           // reaping any zombied bg processes. 
										{
											flag=waitpid(trav->proid,&(trav->status),WNOHANG);
											if(flag>0)
											{												
												printf("[%d] bg process reaped \n",flag);
												flag=-1;
											}
											trav=trav->next;
										}										
									}
								}
								else                // for bg process store its pid in ll and keep it separate group.
								{									
									pid_t p=fretval;
									int s=setpgid(p,0);
									if(head==NULL)
									{
										struct bgnode *temp=malloc(sizeof(struct bgnode));
										temp->proid=fretval;
										temp->status=0;
										temp->next=NULL;
										head=temp;
									}	
									else
									{
										struct bgnode *temp3=malloc(sizeof(struct bgnode));
										struct bgnode *temp2;
										temp2=head;
										while(temp2->next!=NULL)
										{
											temp2=temp2->next;
										}
										temp3->proid=fretval;
										temp3->status=0;
										temp3->next=NULL;
										temp2->next=temp3;
									}
								}
						 }						
					}
			}
       
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
	}
	
	return 0;
}
