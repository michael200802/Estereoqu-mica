#include "is_correct.h"
#include "../Data/constants.h"
#include "../Data/uma/get_uma.h"
#include <ctype.h>

#include <stdio.h>

#define END ""
#define ERROR NULL

//get the next compound
//if there isn't any compound left, it returns END
//if an error occurrs, it returns ERROR/NULL
//if success, it returns str  
static inline const char * goto_next_compound(const char * str)
{
    while(isspace(*str))
    {
        str++;
    }

    if(*str == '\0')
    {
        return END;
    }

    if(*str == '+')
    {
        str++;
        while(isspace(*str))
        {
            str++;
        }
    }
    else
    {
        return ERROR;
    }

    return str;
}

//if success, it returns the next compound if any (if there isn't a next one, it return END)
//if failure, it returns ERROR
static inline const char * _is_compoundsum_correct(const char * str)
{
    size_t count = 0;//counts the current number of '(' without its ')'
    size_t i;//iterator
    const char * end;//end of the compund
    char symbol[MAXLEN_OF_SYMBOL+1];

    //only accepted characters shall be allowed
    for (i = 0;isspace(str[i]) == 0 && str[i] != '+' && str[i] != '\0'; i++)
    {
        if(isalnum(str[i]) == 0)
        {
            switch (str[i])
            {
            //allowed
            case '(':
                count++;
                break;
            case ')':
                if(count == 0)
                {
                    return ERROR;
                }
                count--;
                break;

            //not allowed
            default:
                return ERROR;
                break;
            }
        }
    }
    if(count != 0)
    {
        return ERROR;//there isn't any ')' or '(' without its pair
    }

    end = str+i;//this is the end

    //skip the coefficient
    while(isdigit(*str))
    {
        str++;
    }

    do
    {
        if(*str == '(')
        {
            str++;
        }

        //first character must be an alphabetic letter which is upper
        if(isalpha(*str) == 0)
        {
            return ERROR;
        }
        if(isupper(*str) == 0)
        {
            return ERROR;
        }

        //get the symbol
        symbol[0] = *str;
        str++;
        if(isalpha(*str))//there is a second letter of the element
        {
            if(islower(*str))
            {
                symbol[1] = *str;
                symbol[2] = '\0';
                str++;
            }
            else
            {
                symbol[1] = '\0';
            }
        }
        else
        {
            symbol[1] = '\0';
        }

        //the symbol represents an element?
        if(get_uma(symbol) == -1)
        {
            return ERROR;
        }

        printf("Sym: %s\n",symbol);

        do
        {
            while(isdigit(*str))//skipt the subscript
            {
                str++;
            }
        } while (*str == ')' ? str++ : false);//another subscript may be after a ')'
    }
    while(str != end);

    return goto_next_compound(end);
}

//is it correct?

inline bool is_compound_correct(const char * str)
{
    return _is_compoundsum_correct(str) == END; 
}

bool is_compoundsum_correct(const char * str)
{
    while(isspace(*str))
    {
        return false;
    }

    while(str != END)
    {
        str = _is_compoundsum_correct(str);
        if(str == ERROR)
        {
            return false;
        }
    }

    return true;
}