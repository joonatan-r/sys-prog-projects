wish is a command line interpreter that can be used to run programs with the
desired arguments. They can also be run parallelly and the outputs can be
redirected to a file instead of stdin. In addition to interpreting input one
line at a time wish can be given a file as an argument where it reads the lines
to be interpreted. Execution is stopped when encountering end of file in either
mode. In addition to programs wish recognizes the built-in commands exit, cd and
path. Exit takes no arguments and stops the execution of wish. Cd takes exactly
one argument and changes the current directory to the one given as an argument.
Path changes the list of directories where wish finds the programs to execute.
Initially the list contains the directory /bin. Without arguments path empties
the list so that only built-in commands can be run. With several arguments path
replaces the current list with the directories given as arguments.

Redireccting output to a file is done with the ">" character following the
arguments and followed by a single file name. If the file exists, it is
overwritten. Running programs parallelly is done by separating the commands with
the "&" character. Excess "&" characters are ignored. Parallelly run programs
can also be redirected to their own files.

Commands or programs and their arguments are separated by one or several spaces
and/or tabs or their combination. Redirection and parallel symbols don't require
whitespace.

When wish encounters an error, it prints a message describing it and continues
its execution. Programs run by wish take care of their errors and messages
themselves.
