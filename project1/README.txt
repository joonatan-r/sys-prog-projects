my-cat prints the contents of the files given to it as arguments. Maximum line
length is set to 1000 characters. If run without arguments simply returns 0.

my-grep searches for the string given as the first argument from the files given
as other arguments or from stdin, if the string is the only argument. It prints
the lines that contain the string. If run without arguments returns 1.

my-zip writes a run-length-encoded version of one or several files given as
arguments to stdin. This is then meant to be redirected to a new file which
hopefully takes up less space than the original. When given multiple files their
contents are handled as if they had been one continuous string. When the read
string contains the same character multiple times in a row, it's replaced with
the number of the characters followed by a single occurrence of the character.
If there's only one occurrence of a character, "1" is still written before it.
In the encoded format all the rows of the same character are replaced by a
binary 4-byte integer followed by an ASCII-character. If run without arguments
returns 1.

my-unzip prints the contents from one or several files produced by my-zip and
given as arguments back in their unencoded format. When given multiple files
their contents are handled as if they had been one continuous string. If run
without arguments returns 1.
