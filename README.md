
This is an  implement a command line interpreter, or as it is more commonly known, a shell. The shell should operate in this basic way: when you type in a command (in response to its prompt), the shell creates a child process that executes the command you entered; once the child process has finished, the shell prompts for more user input. The shell you implement will be similar to, but much simpler than, the one you run every day in an Unix/Linux-based OS.

It repeatedly prints a prompt “tinyshell> ” (note the space after the > sign), parses the input that the user just typed followed by a newline (i.e., ’\n’), executes the command specified by the input, and waits for the command to finish.
Termination: Your shell repeats its execution cycle until the user types exit, without anything following it other than a newline.

Delimiters: You can assume that the command and each of its arguments will be separated by one or more tab or space characters. For example, “ps -ef > filename &” is a valid command; however, “ps -ef>filename&” is not. In fact, your shell should interpret the latter as a command with two tokens: “ps”
and “-ef>filename&”; and it should attempt to execute ps with -ef>filename& as its argument. Processing: Your shell should be able to parse the input (aka command) and run the program corresponding
to the command. For example, if the user types ls -la /tmp, your shell should run the program ls with all the given arguments and print the output on the screen (aka stdout). Note that the shell itself does not “implement” ls and many other commands like this. All it does is to find the executable in the environment $PATH (e.g., for ls, the actual executable file is at /bin/ls) and create a new process to run the executable.


The shell supports some internal commands such as cd, echo, pwd, though it does not support all of them
