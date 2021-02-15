# Yuxuan Luo EC440 Challenge 1
- **Brief Description**
    This is a simple shell project. Execute without any parameter to enter the shell, enter with `-n` parameter to omit the prompt. Input `EOF`, which is invoked by `Ctrl + D`, to exit the shell.
    Certain rules apply to this shell:
    - `&` can only be placed at the end of the input
    - Only first command can redirect its input, last command redirect its output

- **Structure**
    1. Print prompt (omit if with `-n` parameter) and wait for input
    2. Parse the input, convert to `struct pipeline` data structure
    3. Fork and execute
    4. Go back to 1.

- **Reference**
    - Freeing array of string
    >```c
    >while( tokenArray[i] != NULL ){ 
    >   free(tokenArray[i])
    >   i++;
    >}
    >```

    ​		[stackOverflow](https://stackoverflow.com/questions/55191862/c-function-to-free-an-array-of-strings)

    - `strcat()` and `strcpy()` [stackOverflow](https://stackoverflow.com/questions/308695/how-do-i-concatenate-const-literal-strings-in-c)
    
    - `fork()` [GeeksForGeeks](https://www.geeksforgeeks.org/fork-system-call/)
      
    - differences among `exec*()` family [StackOverflow](https://stackoverflow.com/questions/55743496/difference-between-exec-execvp-execl-execv)
    
    - Detecting `EOF` [StackOverflow](https://stackoverflow.com/questions/1428911/detecting-eof-in-c)
      
    - Piping framework [StackOverflow](https://stackoverflow.com/a/30193150/12136673)