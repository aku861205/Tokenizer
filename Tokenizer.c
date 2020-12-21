#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#define TRUE 1
#define FALSE 0
#define DECIMAL 1
#define FLOATING 2
#define OCTAL 3
#define HEX 4
#define OPERATOR 5
#define WORD 6




//-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
/*
 * CURRENT struct 
 * ----------------------------
 *  string: the string of current token 
 *  currtoken: the possible current token  
 *  length: length of current string
 *  name: used for operator's name or keyword
 */
typedef struct current
{
    char* string;
    int currtoken;
    int length;
    char name[50];
}CURRENT;


/*
 * NODE node
 * ----------------------------
 *  name: name of each operator or keyword
 *  thisChar: the characters of each operator or keyword
 * 
 */

typedef struct node
{
    char name[50];
    char thisChars[10];
}NODE;



//-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
NODE* search (char*, NODE*);
int isSpecial (char);
int isOctalNum (char);
int isComment (char*, int*);
char* addChar (char*, char);
void update (CURRENT* , char*, int, int* , int);
void newToken (CURRENT*, char*, int* );
int endToken (CURRENT*, char*, int*, NODE*);
void thisToken (char*, int*, NODE*, NODE*);
void report (CURRENT*);
void addNullByte (char*);
void testFloat (CURRENT*, char*, int*);
int testOpeartor (CURRENT*, char*, int*, NODE*);
void testKeyword (CURRENT*, NODE*);
void fillList (NODE*, NODE*);
void insert (NODE*, char*, char*, int );




int main(int argc, char* argv[])
{
    
    //list: operators
    //list2: keywords
    NODE* list = (NODE*)malloc(50*sizeof(NODE));
    NODE* list2 = (NODE*)malloc(50*sizeof(NODE));

    fillList (list, list2);


    //str points to the address of the copy of input string
    char* str;
    //pos is tracking the position of character in str
    int pos = 0;
    
    //Check  invalid input
    if ( argv[1] == NULL || strcmp(argv[1], "") == 0 )
    {
        printf("NO INPUT!!!\n\n");
        return EXIT_FAILURE;
    }
    else
    {
        //copy input string to str 
        str = strdup(argv[1]);
        strcpy (str, argv[1]);

        //this loop dealing with token one by one until encounter \0
        while ( *(str+pos) != '\0')
            thisToken ( str, &pos, list, list2);
    }
    
    free(list);
    free(list2);
    return EXIT_SUCCESS;
}


/*
 * Function: isOctalNum
 * ----------------------------
 *   determine if this character is Octal (0-7)
 *
 *   ch: the character to be determined
 *
 *   returns: TRUE if it's octal
 */

int isOctalNum (char ch)
{
     if ('0' <= ch && ch <= '7')
        return TRUE;
    else
        return FALSE;
    
}



/*
 * Function: isSpecial
 * ----------------------------
 *   determinen if this character is not alphanumeric and is not space
 *
 *   ch: the character to be determined
 *
 *   returns: TRUE if it's non alphanumeric and not a space 
 */

int isSpecial (char ch) 
{
    if ( isalpha(ch) || isdigit(ch) || isspace(ch) )
        return FALSE;
    else
        return TRUE;
}



/*
 * Function: update
 * ----------------------------
 *   this function concatenate the string of current token 
 *   with the character in the current position, update token to parameter value, 
 *   and increment the length.
 *   
 *
 *   c: points to the struct of current token
 *   str: points to the first character of the copy input string
 *   pos: points to current position in the str
 *   token: the token which current is going to update to
 *   max: the target postion we want to move
 */

void update (CURRENT* c, char* str, int token, int* pos, int max)
{
    c->currtoken = token;
    for(int i = 0 ; i < max ; i++)
    {
        c->string = addChar(c->string, *(str+(*pos)));
        (*pos)++;
        (c->length)++;
    }
}



/*
 * Function: endToken
 * ----------------------------
 *   This function checks if encounter the end of current token,
 *   if not end: it updates string of current token, 
 *               moves character to next position,
 *               and changes the token if necessary and returns FALSE
 *   if current token ends: return TRUE.
 * 
 * 
 *   c: points to the struct of current token
 *   str: points to the first character of the copy input string
 *   pos: points to current position in the str
 *   list: contains all operators and corresponding name
 *    
 *   returns: TRUE or FALSE
 */

int endToken (CURRENT* c, char* str, int* pos, NODE* list)
{
    /* ch: the current character in the posion  of input string */
    /* next: next to current character                          */
    char ch = *(str+(*pos));
    char next = *(str+(*pos+1));

    /* Current character is \0: return TRUE*/
    if (ch == '\0')
    {
        return TRUE;
    }


    /* Current is delimeter: skip all the while spaces and return TRUE */
    if (isspace(ch))
    {
        while (isspace(*(str+(*pos))))
            ++*pos;
        return TRUE;
    }


    /* Call isComment function to check if encounter comment */
    if (isComment(str, pos))
    {
        return TRUE;
    }
   

    /* octal */
    if (c->currtoken == OCTAL)
    {
        /* if current character is '.' and following by digit,      */
        /* update the token to floating and move to next 2 position */
        if (ch == '.' && isdigit(next))
        {
            update (c, str, FLOATING, pos, 2);
            return FALSE;
        }


        /* if current character is non octal digit, */ 
        /* update to decimal, and move to next      */
        else if (isdigit(ch))
        {

            /* if current character is non octal digit, */
            /* update to decimal, and move to next      */
            if (!isOctalNum(ch))
            {
                update (c, str, DECIMAL, pos, 1);
                return FALSE;
            }

            /* if current character is octal digit, move to next position */
            else
            {
                update (c, str, c->currtoken, pos, 1);
                return FALSE;
            }  

        }
        /* if above octal condition don't meet, then current token end */
        else
        {
            return TRUE;
        }
    }


    /* Hex */
    if (c->currtoken == HEX)
    {
        /* if current character is not hex, current token ends */
        if (!(isxdigit(ch)))
        {
            return TRUE;
        }

        /* if current character is hex, move to next position */
        else
        {
             update (c, str, c->currtoken, pos, 1);
             return FALSE;
        }
    }


    /* floating point */
    if (c->currtoken == FLOATING)
    {

        /* if current character is a digit, move to next position */
        if (isdigit(ch))
        {
            update (c, str, c->currtoken, pos, 1);
            return FALSE;
        }
        /* if current character is a 'e' or 'E' letter */
        /* call test float function and end token      */
        if ( ch == 'e' || ch=='E' )
        {
            testFloat (c, str, pos);
            return TRUE;
        }
        /* current token ends */
        else
        {
            return TRUE;
        }

    }


    /*Operators*/
    if (c->currtoken == OPERATOR)
    {
        return testOpeartor(c,str,pos,list);
    }


    /*Decimal integer*/
    if (c->currtoken == DECIMAL)
    {
        if(isdigit(ch))
        {
            update(c, str, c->currtoken, pos, 1);
            return FALSE;
        }
        /* if current character is '.' and following by digit  */
        /* update token to float and move to next two position */
        else if( ch == '.' && isdigit(next))
        {
            update(c, str, FLOATING, pos, 2);
            return FALSE;
        }
        /* token end above condtion don't meet */
        else
        {
            return TRUE;
        }
    }


    /*word*/
    if (c->currtoken == WORD)
    {
        /* word token ends with non-alphanumeric character */
        if (isSpecial(ch))
        {
            return TRUE;
        }
        else
        {
            update(c, str, c->currtoken, pos, 1);
            return FALSE;
        }
    }

    return TRUE;
}



/*
 * Function: addChar
 * ----------------------------
 *   this function concatenate the string with the character, 
 *   and add '\0' in the end
 *   
 *   str: the string going to be concatenated
 *   ch: the character going to be concatenated
 *   
 *   return the pointer points to result string
 */

char* addChar (char* str, char ch)
{
    size_t len = strlen (str);
    char *str2 = malloc (len + 2);
    strcpy (str2, str);
    str2[len] = ch;
    str2[len + 1] = '\0';

    return str2;
}


/*
 * Function: newToken
 * ----------------------------
 *   this function starts a new Token, analyzes first few character, 
 *   and assigns a possible token to CURRENT. 
 *   
 *   c: points to the struct of current token
 *   str: points to the first character of the copy input string
 *   pos: points to current position in the str
 *  
 */

void newToken (CURRENT* c, char* str, int* pos)
{
    c->string = "";
    c->currtoken = -1;
    c->length = 0;
     
    /* skip white spaces */
    while (isspace(*(str+(*pos))))
    {
        ++*pos;
    }

    /* skip comment */
    isComment(str,pos);

    /* ch: character in current position */
    char ch = *(str+(*pos));
    char nextCh = *(str+(*pos)+1);
    char nextNext = *(str+(*pos)+2);

    /* new token starts with operator */
    if (isSpecial(ch))
    {
        update (c, str, OPERATOR, pos, 1);
        //printf("New Token:%s\nLength:%d\ng",c->string,c->length);
        return;
    }


    /* new token starts with letter is word */
    if (isalpha(ch))
    {
        update(c, str, WORD, pos, 1);
        return;
    }

    /* new token starts with '0' */
    if(ch == '0')
    {
        /* Assign new token to HEX if '0' followed by 'x' or 'X' and hexdigit */
        if(nextCh == 'x' || nextCh == 'X')
        {
            if(isxdigit(nextNext))
            {
                update(c, str, HEX, pos, 3);
                return;
            }
        }
        /* Assign new token to octal '0' follow any octal digit or no digit */
        update(c, str, OCTAL, pos, 1);
        return;
    
    }
    /* Assign new token to decimal if start with digit */
    if (isdigit(ch))
    {
        update(c, str, DECIMAL, pos, 1);
    }
    
}



/*
 * Function: thisToken
 * ----------------------------
 *   this function starts a new token by calling newToken function, 
 *   keep calling endToken function until find the end, print out report.
 *   
 *   str: points to the first character of the copy input string
 *   pos: points to current position in the str
 *   list: contains all the operators
 *   list2: contains all the keywords
 */

void thisToken (char* str, int* pos, NODE* list, NODE* list2)
{
    CURRENT* this = (CURRENT*)malloc(sizeof(CURRENT));

    /* start a new token */
    newToken(this, str, pos);

    /* keep calling endtoken until find the end of current token */
    int notEnd;
    do
    {
        notEnd = !endToken(this, str, pos, list);

    }while (notEnd);
    

    /* check the operator again */
    if (this->currtoken == OPERATOR)
    {
        testOpeartor(this, str, pos, list);
    }

    /* check if current word string is keyword */
    if (this->currtoken == WORD)
    {
        testKeyword(this, list2);
    }

    /* error check: if current string is not an empty string */
    /* print report                                          */
    if (strcmp(this->string,"") != FALSE)
    {
        report(this);
    }

    free(this);
}



/*
 * Function: report
 * ----------------------------
 *   this function prints out the information in the CURRENT  
 *   
 *   c: points to the struct of current token
 */

void report (CURRENT* c)
{
    /* add '\0' in the end of current string */
    addNullByte(c->string);

    switch (c->currtoken)
    {
    case DECIMAL:
        printf("%s: \"%s\"\n","decimal integer",c->string);
        break;

    case FLOATING:
        printf("%s: \"%s\"\n","floating point",c->string);
        break;

    case OCTAL:
        printf("%s: \"%s\"\n","octal integer",c->string);
        break;

    case HEX:
        printf("%s: \"%s\"\n","hexadecimal integer",c->string);
        break;

    case OPERATOR:
        printf("%s: \"%s\"\n",c->name,c->string);
        break;

    case WORD:
        printf("%s: \"%s\"\n","word",c->string);
        break;

    default:
        break;
    }
}



/*
 * Function: addNullByte
 * ----------------------------
 *   this function add null byte in the end of parm string
 *   
 *   str: the string going to be processed  
 */
void addNullByte (char* str)
{
    size_t len = strlen(str);
    char *out = (char*)malloc(len + 1);
    out[len] = '\0';
    strcpy(out, str);
    free(out);
}


/*
 * Function: testFloat
 * ----------------------------
 *   this function processes float when it encounters 'E' or 'e',
 *   and find the end of the float.
 *   
 *   c: points to the struct of current token
 *   str: points to the first character of the copy input string
 *   pos: points to current position in the str  
 */

void testFloat (CURRENT* c, char* str, int* pos)
{
    /* next: the character next to 'e' or 'E' */
    /* nextNext: the character in the next two position of 'e' or 'E' */
    char next = *(str+(*pos)+1);
    char nextNext = *(str+(*pos)+2);

    if ((next == '+'|| next == '-') && (isdigit(nextNext)))
    {
        update (c, str, FLOATING, pos, 2);
        while (isdigit(*(str+(*pos))))
            update (c, str, FLOATING, pos, 1); 
            
    }

    if (isdigit(next))
    {
        update (c, str, FLOATING, pos, 1);
        while (isdigit(*(str+(*pos))))
            update (c, str, FLOATING, pos, 1);

    }
    
}



/*
 * Function: search
 * ----------------------------
 *   this function search the parm list, and returns the node that is matched 
 *   parm str, if nothing found, it returns the "NOTFOUND" node indicate that  
 *   there is no this operator or key word in the list.
 *   
 *   str: the targers wants to find
 *   list: the list going to be searched
 */

NODE* search(char* str,NODE* list){

    int i = 0;
    do
    {
        if (strcmp((list+i)->thisChars,str)==FALSE)
        {
            return list+i;
        }
        i++;
    }while ((strcmp((list+i)->thisChars,"NOTFOUND")!=FALSE));

    /* If the loop doesn't find the targets*/
    return list+i;
}



/*
 * Function: testOperator
 * ----------------------------
 *   this function search the list, find the name of the character.
 *   If next character is still special character, it combined two characters,
 *   and search again.
 *   it'll return TRUE if current character is not special chracter or if nothing
 *   found in the list, and return false if it found the name.
 *   
 *   
 *   c: points to the struct of current token
 *   str: points to the first character of the copy input string
 *   pos: points to current position in the str 
 *   list : points to the operators list
 */

int testOpeartor (CURRENT* c, char* str, int* pos, NODE* list){


    char curr =  *(str+(*pos));
    char next = *(str+(*pos)+1);

    NODE* temp =(NODE*)malloc(1*sizeof(NODE));


    /* if current character is the first character in the current token */
    /* search the name and copy the name to name of current token       */
    if(c->length==1)
    {
        temp = search(c->string,list);
        strcpy(c->name,temp->name);   
    }

 
    /* if current character is alphanumeric, current operator token ends */
    if(!isSpecial(curr))
    {
        return TRUE;
    }

    /* if current character is non-alphanumeric                 */
    /* search the combination of previous charcters and current */
    char* combo= (char*)malloc(sizeof(char)*10);
    strcpy(combo,c->string);

    combo=addChar(combo,curr);
    temp = search(combo,list);


    /* if not found, current token ends                                  */
    /* if found the combination, copy the new name to current token name */
    if(strcmp(temp->thisChars,"NOTFOUND")==FALSE)
    {
        return TRUE;
    }
    else
    {
        strcpy(c->name,temp->name);
        update(c,str,c->currtoken,pos,1);
        return FALSE;
    }
    free(temp);
    free(combo);
}


/*
 * Function: testKeyword
 * ----------------------------
 *   this function search the keyword list 
 *   and see if the current word matches the keywords.
 *   if found, assign current token as OPERATOR and copy the name
 *   
 *   
 *   c: points to the struct of current token 
 *   list2 : points to the keyword list
 */

void testKeyword (CURRENT* c, NODE* list2)
{
     NODE* temp = search(c->string,list2);

     if (strcmp(temp->thisChars,"NOTFOUND")!=FALSE)
     {
         c->currtoken=OPERATOR;
         strcpy (c->name,temp->name);
     }

}



/*
 * Function: fillList
 * ----------------------------
 *   this function fills the two lists with the chars and correspondsing name
 *   
 *   list: operators list
 *   list2: keywords list
 */

void fillList(NODE* list, NODE* list2)
{

        /* fill operators list */
        insert (list,"(", "left parenthesis ",0);
        insert (list,")", "right parenthesis",1);
        insert (list,"[", "left bracket",2);
        insert (list,"]", "right bracket",3);
        insert (list,".", "structure member",4);
        insert (list,"->", "structure pointer",5);
        insert (list,"sizeof", "sizeof ",6);
        insert (list,",", "comma",7);
        insert (list,"!", "negate",8);
        insert (list,"~", "1s complement",9);
        insert (list,">>", "shift right",10);
        insert (list,"<<", "shift left",11);
        insert (list,"^", "bitwise XOR",12);
        insert (list,"|", "bitwise OR",13);
        insert (list,"++", "increment",14);
        insert (list,"--", "decrement",15);
        insert (list,"+", "addition",16);
        insert (list,"/", "division",17);
        insert (list,"||", "logical OR",18);
        insert (list,"&&", "logical AND",19);
        insert (list,"?", "conditional true",20);
        insert (list,":", "conditional false",21);
        insert (list,"==", "equality test",22);
        insert (list,"!=", "inequality test",23);
        insert (list,"<", "less than test",24);
        insert (list,">", "greater than test",25);
        insert (list,"<=", "less than or equal test",26);
        insert (list,">=", "greater than or equal test",27);
        insert (list,"=", "assignment",28);
        insert (list,"+=", "plus equals",29);
        insert (list,"-=", "minus equals",30);
        insert (list,"*=", "times equals",31);
        insert (list,"/=", "divide equals",32);
        insert (list,"%=", "mod equals",33);
        insert (list,">>=", "shift right equals",34);
        insert (list,"<<=", "shift left equals",35);
        insert (list,"&=", "bitwise AND equals",36);
        insert (list,"^=", "bitwise XOR equals",37);
        insert (list,"|=", "bitwise OR equals",38);
        insert (list,"&", "AND/address operator",39);
        insert (list,"-", "minus/subtract operator",40);
        insert (list,"*", "multiply/dereference operator",41);
        insert (list,"NOTFOUND", "NO THIS OPERATOR!",42);

        /* fill keywords list */
        insert (list2,"auto", "Keyword auto",0);
        insert (list2,"break", "Keyword break",1);
        insert (list2,"case", "Keyword case",2);
        insert (list2,"char", "Keyword char",3);
        insert (list2,"const", "Keyword const",4);
        insert (list2,"continue  ", "Keyword continue    ",5);
        insert (list2,"default", "Keyword default",6);
        insert (list2,"do", "Keyword do",7);
        insert (list2,"double", "Keyword double",8);
        insert (list2,"else", "Keyword else",9);
        insert (list2,"enum", "Keyword enum",10);
        insert (list2,"extern", "Keyword extern",11);
        insert (list2,"float", "Keyword float",12);
        insert (list2,"for", "Keyword for",13);
        insert (list2,"goto", "Keyword goto",14);
        insert (list2,"if", "Keyword if",15);
        insert (list2,"int", "Keyword int",16);
        insert (list2,"long", "Keyword long",17);
        insert (list2,"register", "Keyword register",18);
        insert (list2,"return", "Keyword return",19);
        insert (list2,"short", "Keyword short",20);
        insert (list2,"signed", "Keyword signed",21);
        insert (list2,"sizeof", "Keyword sizeof",22);
        insert (list2,"static", "Keyword static",23);
        insert (list2,"struct", "Keyword struct",24);
        insert (list2,"switch", "Keyword switch",25);
        insert (list2,"typedef", "Keyword typedef",26);
        insert (list2,"union", "Keyword union",27);
        insert (list2,"unsigned", "Keyword unsigned",28);
        insert (list2,"void", "Keyword void",29);
        insert (list2,"volatile", "Keyword volatile",30);
        insert (list2,"while", "Keyword while",31);
        insert (list2,"NOTFOUND", "NO THIS KEYWORD",32); 
}



/*
 * Function: insert
 * ----------------------------
 *   this function copy the operator or keyword character in the parameters
 *   to the field of node list
 *   
 *   list: the target list to be inserted
 *   thisChars: the actual characters of operators or keywords ex '+='
 *   name: the name of that characters
 *   i: position to be inseted
 */

void insert (NODE* list, char* thisChars, char* name, int i)
{
    strcpy((list+i)->thisChars, thisChars);
    strcpy((list+i)->name, name);
}



/*
 * Function: isComment
 * ----------------------------
 *   this function determine if encounter comments, if so, it skips the comment,
 *   and return true to end the token.
 *   
 *   str: points to the first character of the copy input string
 *   pos: points to current position in the str 
 */

int isComment (char* str, int* pos)
{
    char curr = *(str+(*pos));
    char next = *(str+(*pos)+1);

    /* line comment */
    /* keep moving to next position until newline*/
    if ((curr == '/' && next == '/'))
    { 
        *pos += 2;
        while (*(str+(*pos)) != '\n')
        {
            ++*pos;
        }
        /* skip newline character */
        ++*pos;
        return TRUE;
    }
    
    int end = FALSE;
    if ((curr == '/' && next == '*'))
    {
        /* skip '/' and '*' */
        *pos += 2;

        /* keep moving to next pos until encounter '*' and '/' */
        do
        {
            end = (*(str+(*pos)) == '*' && (*(str+(*pos)+1)) == '/');
            ++*pos;
        }while (!end);

        /* skip '/' */
        ++*pos;

        /* skip whitespace in the end of comment */
        while (isspace(*(str+(*pos))))
        {
            ++*pos;
        }
        return TRUE;
    }
    return FALSE;
}










