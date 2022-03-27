#include "substance.h"

#define char_to_digit(ch) (ch-48)

#define digit_to_char(dig) ((char)(dig)+48)

static inline size_t ten_raisedto(size_t exponent)
{
    size_t result = 1;
    while(exponent != 0)
    {
        result *= 10;
        exponent--;
    }
    return result;
}

//returns zero if there isn't any number
static inline size_t read_uint(const char * restrict str, const char ** restrict endptr)
{
    size_t result = 0;
    size_t len;
    for(len = 0; isdigit(str[len]); len++);
    while(len != 0)
    {
        len--;
        result += ten_raisedto(len)*char_to_digit(*str);
        str++;
    }
    *endptr = str;
    return result;
}

static inline bool is_str_substance(const char * restrict str)
{
    //do pairs of brackets () match?
    {
        size_t alone = 0;
        for (size_t i = 0; str[i] != '\0'; i++)
        {
            switch (str[i])
            {
            case '(':
                alone++;
                if(str[i+1] == ')')//there must not be an empty pair "()" 
                {
                    return false;
                }
                break;
            case ')':
                if(alone == 0)
                {
                    return false;
                }
                alone--;
                break;
            }
        }
    }

    while(isdigit(*str))
    {
        str++;
    }

    while (*str != '\0')
    {
        switch (*str)
        {
            case '(':
                do
                {
                    str++;
                } while (*str == '(');
                
            default:
                elem_symbol_t sym;
                if(init_elem_symbol(str,&sym) == false)
                {
                    return false;
                }
                if(get_symbol_len(sym) == 2)
                {
                    str++;
                }

            case ')':
                str++;
                while(isdigit(*str))
                {
                    str++;
                }
        }
    }
    

    return true;
}

static inline void _init_substance(const char * str, const char * endstr, substance_t * const restrict sub)
{
    //get the number of substances
    size_t nsubstances = 0;
    for(size_t i = 0; &str[i] != endstr; i++)
    {
        if(isupper(str[i]))
        {
            nsubstances++;
        }
        else if(str[i] == '(')
        {
            nsubstances++;
            {//pass the brackets
                i++;
                size_t count = 1;
                while(count != 0)
                {
                    switch(str[i])
                    {
                        case '(':
                            count++;
                            break;
                        case ')':
                            count--;
                            break;
                    }
                    i++;
                }
                i--;
            }
        }
    }

    printf("%zu: ",nsubstances);

    for(size_t i = 0; str+i != endstr; i++)
    {
        putchar(str[i]);
    }
    putchar('\n');

    //fill/read
    if(nsubstances == 1)
    {
        if(*str == '(')
        {
            endstr--;
            while(*endstr != ')')
            {
                endstr--;
            }
            _init_substance(str+1,endstr,sub);
            str = endstr+1;
            size_t subscript = read_uint(str,&str);
            if(subscript != 0)
            {
                sub->amount *= subscript;
            }
            //printf("%zu\n",sub->amount);
        }
        else
        {
            sub->is_simple_substance = true;

            {//fill elem
                elem_symbol_t symb;
                init_elem_symbol(str,&symb);
                init_elem(symb,&sub->substance.elem);
            }

            //move to subscript
            str++;
            if(islower(*str))
            {
                str++;
            }
            //get subscript
            sub->amount = read_uint(str,&str);
            if(sub->amount == 0)
            {
                sub->amount = 1;
            }

            sub->molar_mass = sub->substance.elem.molar_mass;
        }
    }
    else
    {
        sub->is_simple_substance = false;
        
        substance_t * substances = malloc(sizeof(substance_t)*nsubstances);

        for(size_t i = 0; i < nsubstances; i++)
        {
            if(*str == '(')
            {
                const char * end;
                end = str+1;
                {
                    size_t count = 1;
                    while(count != 0)
                    {
                        switch(*end)
                        {
                            case '(':
                                count++;
                                break;
                            case ')':
                                count--;
                                break;
                        }
                        end++;
                    }
                }
                while(isdigit(*end))
                {
                    end++;
                }
                _init_substance(str,end,&substances[i]);
                str = end;
            }
            else
            {
                substances[i].is_simple_substance = true;

                {//fill the elem
                    elem_symbol_t symb;
                    init_elem_symbol(str,&symb);
                    init_elem(symb,&substances[i].substance.elem);
                }

                //get subscript
                str++;
                if(islower(*str))
                {
                    str++;
                }
                substances[i].amount = read_uint(str,&str);
                if(substances[i].amount == 0)
                {
                    substances[i].amount = 1;
                }
                
                //get molar mass
                substances[i].molar_mass = substances[i].substance.elem.molar_mass;
            }

        }

        sub->substance.compound.substances = substances;
        sub->substance.compound.nsubstances = nsubstances;

        //in expressions like these: H2O CO2 HCl C6H12O6
        //the amount is 1
        sub->amount = 1;

        //get molar mass
        sub->molar_mass = 0;
        for(size_t i = 0; i < nsubstances; i++)
        {
            sub->molar_mass += substances[i].molar_mass * substances[i].amount;
        }
    }
}

bool init_substance(const char * str, substance_t * const restrict sub)
{
    if(is_str_substance(str) == false)
    {
        return false;
    }

    sub->amount = read_uint(str,&str);
    if(sub->amount == 0)
    {
        sub->amount = 1;
    }
    sub->is_simple_substance = false;
    sub->substance.compound.nsubstances = 1;
    sub->substance.compound.substances = malloc(sizeof(substance_t));

    const char * end = str;
    while(*end != '\0')
    {
        end++;
    }
    _init_substance(str,end,sub->substance.compound.substances);

    sub->molar_mass = sub->substance.compound.substances->molar_mass * sub->substance.compound.substances->amount;

    return true;
}

static inline size_t print_size_t(size_t num, char * restrict str, size_t strlen, char ** const restrict end)
{
    size_t numlen = 1;
    size_t divisor;

    for(size_t _num = num; (_num/=10) != 0; numlen++);

    if(numlen >= strlen)
    {
        return 0;
    }

    divisor = ten_raisedto(numlen)/10;
    while (divisor != 0)
    {
        *str = digit_to_char(num/divisor);
        str++;
        num %= divisor;
        divisor /= 10;
    }
    *str = '\0';

    *end = str;
    

    return numlen;
}

size_t _print_substance(const substance_t * const restrict sub, char * buffer, size_t buffer_len)
{
    if(sub->is_simple_substance)
    {
        char symbol_len = get_symbol_len(sub->substance.elem.symbol);
        if(buffer_len <= symbol_len)
        {
            return 0;
        }

        print_symbol(sub->substance.elem.symbol, buffer);

        buffer += symbol_len;
        buffer_len -= symbol_len;

        if(sub->amount != 1)
        {
            size_t printsize_return = print_size_t(sub->amount,buffer,buffer_len,&buffer);
            if(printsize_return == 0)
            {
                return 0;
            }
            buffer_len -= printsize_return;
        }
    }
    else
    {
        if(sub->amount != 1)
        {
            if(buffer_len == 1)
            {
                *buffer = '\0';
                return 0;
            }
            buffer_len--;
            *buffer = '(';
            buffer++;
        }

        for(size_t i = 0; i < sub->substance.compound.nsubstances; i++)
        {
            size_t printsub_return = _print_substance(sub->substance.compound.substances+i,buffer,buffer_len);
            if(printsub_return == 0)
            {
                return 0;
            }

            buffer += buffer_len-printsub_return;
            buffer_len = printsub_return;
        }

        if(sub->amount != 1)
        {
            if(buffer_len == 1)
            {
                *buffer = '\0';
                return 0;
            }
            buffer_len--;
            *buffer = ')';
            buffer++;

            size_t printsize_return = print_size_t(sub->amount,buffer,buffer_len,&buffer);
            if(printsize_return == 0)
            {
                return 0;
            }
            buffer_len -= printsize_return;
        }
    }

    return buffer_len;
}

bool print_substance(const substance_t * const restrict sub, char * buffer, size_t buffer_len)
{
    size_t printsizer;

    if(sub->amount != 1)
    {
        printsizer = print_size_t(sub->amount,buffer,buffer_len,&buffer);
        if(printsizer == 0)
        {
            return false;
        }
    }

    return _print_substance(sub->substance.compound.substances,buffer,buffer_len-printsizer);
}

void destroy_substance(substance_t * const restrict sub)
{
    if(sub->is_simple_substance == false)
    {
        for(size_t i = 0; i < sub->substance.compound.nsubstances; i++)
        {
            destroy_substance(sub->substance.compound.substances+i);
        }
        free(sub->substance.compound.substances);
    }
}
