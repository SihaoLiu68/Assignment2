Names           NetID
Sihao Liu       sliu68
Yanwei Yang     yyang105
CSC254 Assignment2: Syntax Error Recovery

Our program translates the code to C++, extends the language with if and while statements, 
implements the exception-based syntax error recovery, and output the syntax tree.

The directory A2FINAL has files: parse.cpp, scan.cpp, scan.h, Makefile, test.txt,
It works as follows:
(1) Edit test.txt to test different programs. If you want to test in another textfile, replace the 'test.txt' in Makefile line 2 with your textfile.
(2) In the terminal, type 'make' in the command line.
(3) the program will output either syntax error messages or a correct syntax tree.

In addition to if, while, and end, we extended scan.cpp with relational operations and $$ which marks the end of file.
We create a string ast in each method to represent what will be appended to the linear, parenthesized syntax tree. The final ast is saved as string AST and printed in int main().
We modified each method from void function() to string function(), and for void term_tail() and void factor_tail(), we modified them into string term_tail(term()) and string factor_tail(factor())
The parser parses conditions in the order "C -> E ro E". In the syntax tree, to print in order of "ro E E", we use three strings to save each element while parsing and put them in order afterward.(line 264)
The parser parses add/minus operations in the order "factor ao factor". In the syntax tree, to print in order of "ao factor factor", we use term_tail(term()) which saves the return of term(), and append it after 'ao'.(line 370)
The parser parses multiply/divide operations in the order "factor mo factor". In the syntax tree, to print in order of "mo factor factor", we use factor_tail(factor()) which saves the return of factor(), and append it after 'mo'.(line 466)
The parse has an exception-based error recovery implementation. The exception-based error recovery will only be able to detect the unexpected tokens, and will continue to delete  input tokens until it sees one with which it can recommence parsing. 
The handlers are implemented in every non-terminal, and when it entered the handler code, it will push the error message onto the stack and continue to parse. After the parsing is completed, the program will print the error stack if error stack is not empty or print the parsed AST if error stack is empty. One sample handler could be found at line 131.
