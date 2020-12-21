# Tokenizer
This is Assignment0 in CS 214:System Programming, Fall 2020 \
Completed by Hsinghui Ku
# Description
A tokenizer is a program that takes a single string of characters of an arbitrary length and breaks them
up into types of inputs. For instance, the single string “blah 8 stuff 1.3” contains two 'word'-type inputs,
an 'integer'-type input and a 'float'-type input. 

This program will recognize the following type of tokens:
* **Word** : A word token consists of an alphabetic character followed by any number of alphanumeric
characters. Some word tokens are “hi” “stufF” “ThinGs” “a1231” “C0MP14IN”
* **Decimal** integer : A decimal integer is a digit character followed by any number of digit characters: “12” “8” “2”
* **Octal** : An octal integer is a '0' character followed by any number of octal digits (0-7).
* **Hexidecimal** : A hexadecimal integer is “0x” (or “0X”) followed by any number of hexadecimal digits (0-9, af, A-F).
* **Floating Point** : A floating point token is a decimal integer that has a '.' at any position other than the last. It may
optionally contain an exponent in scientific notation at the end, for example: “3.14159e-10”
* **C operator** : A C operator is any of the operators listed on the C reference card attached to the assignment. Its token type is the name of the operator. 

## function description
This program is mainly based on the following functions:
* The main ( ) function checks if the input is valid and copies the input string from argv[ 1 ]. If there is no input in the command line, main prints out “NO INPUT!” to indicate the user. main ( ) function process each token until finding the end of the input string.
* The newToken ( ) function analyzes the beginning few characters for each token and assigns it a possible token.
* The endToken ( ) function analyzes each character, updates current string and token and returns boolean value to indicate if current token ends or not.
* The thisToken ( ) function keeps calling endToken function, analyzes the current string again when it finds the end of token, and prints out the report.
* The update ( ) function concatenates the current string and will move current position to target position

## Error Handling
If a user enters the non C operator character, it will still analyze the character and print out a message indicating there is no this operator in the list. And if there is no input, it will also indicate the user.
In the endToken( ) function, it calls testOperator ( ) to find the name and the end of each operator and return to thisToken ( ) function if the operator token ends. It won’t check the last character if it’s the end of the string, so the testOperator ( ) will be called again in the thisToken( ). Besides, thisToken( ) will check if the string is “ ” empty string before printing.

## Feature
* This program won’t terminate even if the input non-alpha numeric character is not on the list, and will print out a message to indicate this.
* It will recognize “+++” as increment: “++” and addition: “+” instead of addition :“+” and increment :“++”, because the longer token is prefered
* It will recognize the C keywords.
* It will skip both line comment and block comment
