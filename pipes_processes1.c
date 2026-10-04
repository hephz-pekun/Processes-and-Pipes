// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  
int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str[] = "howard.edu";
    char fixed_str2[] = "gobison.org";
    char input_str[100]; 
    pid_t p; 
  
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
  
    printf("Enter a string to concatenate:");
    fflush(stdout);

    scanf("%99s", input_str); 
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    // Parent process 
    else if (p > 0) 
    { 
        char final_str[200] = {0};

        close(fd1[0]);    //close read end of pipe 1
        close(fd2[1]);    //close write end of pipe2

        // send first string to child
        write(fd1[1], input_str, strlen(input_str) + 1);
        close(fd1[1]);    //close immmediately after

        // read modified string from child
        read(fd2[0], final_str, sizeof(final_str));
        close(fd2[0]);    //close immediately

        //Append gobison.org
        strcat(final_str, fixed_str2);

        printf("%s\n", final_str);

        wait(NULL);
    } 
  
    // child process 
    else
    { 
        char concat_str[200] = {0};
        char second_input[100] = {0};
        
        close(fd1[1]);  // Close writing end of first pipes 
        close(fd2[0]); 
      
        // Read a string using first pipe 
        read(fd1[0], concat_str, sizeof(concat_str)); 
        close(fd1[0]); //close after reading
  
        // Append howard.edu
        strcat(concat_str, fixed_str);
 
        printf( "%s\n", concat_str);
 
        // Get second input
        printf("Enter second string: ");
        fflush(stdout);
        
        scanf("%99s", second_input);
 
        // Append second input
        strcat(concat_str, second_input);
 
        // Send back to parent
        write(fd2[1], concat_str, strlen(concat_str) + 1);
        close(fd2[1]); //close after writing
      
        exit(0); 
    } 
    return 0;
} 