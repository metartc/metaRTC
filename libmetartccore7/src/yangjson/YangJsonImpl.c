//
// Copyright (c) 2019-2023 yanggaofeng
// base on Dave Gamble cJSON
//
#include "YangJsonImpl.h"

#include <math.h>
#include <limits.h>
#include <ctype.h>
#include <float.h>

#if Yang_Enable_Json
// define isnan and isinf for ANSI C, if in C99 or above, isnan and isinf has been defined in math.h
#ifndef isinf
#define isinf(d) (isnan((d - d)) && !isnan(d))
#endif
#ifndef isnan
#define isnan(d) (d != d)
#endif

#ifndef NAN
#if Yang_OS_WIN
#define NAN sqrt(-1.0)
#else
#define NAN 0.0/0.0
#endif
#endif

typedef struct {
    const uint8_t *json;
    size_t position;
} yangjsonerror;
static yangjsonerror global_error = { NULL, 0 };

const char * yang_json_getErrorPtr(void)
{
    return (const char*) (global_error.json + global_error.position);
}

char * yang_json_getStringValue(const YangJson * const item)
{
    if (!yang_json_isString(item)) 
    {
        return NULL;
    }

    return item->valuestring;
}

double yang_json_getNumberValue(const YangJson * const item)
{
    if (!yang_json_isNumber(item)) 
    {
        return (double) NAN;
    }

    return item->valuedouble;
}

// Case insensitive string comparison, doesn't consider two NULL pointers equal though
static int yang_json_case_insensitive_strcmp(const uint8_t *string1, const uint8_t *string2)
{
    if ((string1 == NULL) || (string2 == NULL))
    {
        return 1;
    }

    if (string1 == string2)
    {
        return 0;
    }

    for(; tolower(*string1) == tolower(*string2); (void)string1++, string2++)
    {
        if (*string1 == '\0')
        {
            return 0;
        }
    }

    return tolower(*string1) - tolower(*string2);
}

typedef struct internal_hooks
{
    void *(*allocate)(size_t size);
    void (*deallocate)(void *pointer);
    void *(*reallocate)(void *pointer, size_t size);
} internal_hooks;

static internal_hooks global_hooks = { yang_malloc, yangfree, yang_realloc };

static uint8_t* yang_json_strdup(const uint8_t* string, const internal_hooks * const hooks)
{
    size_t length = 0;
    uint8_t *copy = NULL;

    if (string == NULL)
    {
        return NULL;
    }

    length = strlen((const char*)string) + sizeof("");
    copy = (uint8_t*)hooks->allocate(length);
    if (copy == NULL)
    {
        return NULL;
    }
    yang_memcpy(copy, string, length);

    return copy;
}

void yang_json_initHooks(YangJsonHooks* hooks)
{
    if (hooks == NULL)
    {
       // Reset hooks
        global_hooks.allocate = malloc;
        global_hooks.deallocate = free;
        global_hooks.reallocate = realloc;
        return;
    }

    global_hooks.allocate = malloc;
    if (hooks->malloc_fn != NULL)
    {
        global_hooks.allocate = hooks->malloc_fn;
    }

    global_hooks.deallocate = free;
    if (hooks->free_fn != NULL)
    {
        global_hooks.deallocate = hooks->free_fn;
    }

   // use realloc only if both free and malloc are used
    global_hooks.reallocate = NULL;
    if ((global_hooks.allocate == malloc) && (global_hooks.deallocate == free))
    {
        global_hooks.reallocate = realloc;
    }
}

// Internal constructor.
static YangJson *yang_json_new_item(const internal_hooks * const hooks)
{
    YangJson* node = (YangJson*)hooks->allocate(sizeof(YangJson));
    if (node)
    {
        yang_memset(node, '\0', sizeof(YangJson));
    }

    return node;
}

// Delete a JSON structure.
void yang_json_delete(YangJson *item)
{
    YangJson *next = NULL;
    while (item != NULL)
    {
        next = item->next;
        if (!(item->type & Yang_JSON_IsReference) && (item->child != NULL))
        {
            yang_json_delete(item->child);
        }
        if (!(item->type & Yang_JSON_IsReference) && (item->valuestring != NULL))
        {
            global_hooks.deallocate(item->valuestring);
        }
        if (!(item->type & Yang_JSON_StringIsConst) && (item->string != NULL))
        {
            global_hooks.deallocate(item->string);
        }
        global_hooks.deallocate(item);
        item = next;
    }
}

// get the decimal point character of the current locale
static uint8_t yang_json_get_decimal_point(void)
{
#ifdef ENABLE_LOCALES
    struct lconv *lconv = localeconv();
    return (uint8_t) lconv->decimal_point[0];
#else
    return '.';
#endif
}

typedef struct
{
    const uint8_t *content;
    size_t length;
    size_t offset;
    size_t depth;// How deeply nested (in arrays/objects) is the input at the current offset.
    internal_hooks hooks;
} parse_buffer;

// check if the given size is left to read in a given parse buffer (starting with 1)
#define can_read(buffer, size) ((buffer != NULL) && (((buffer)->offset + size) <= (buffer)->length))
// check if the buffer can be accessed at the given index (starting with 0)
#define can_access_at_index(buffer, index) ((buffer != NULL) && (((buffer)->offset + index) < (buffer)->length))
#define cannot_access_at_index(buffer, index) (!can_access_at_index(buffer, index))
// get a pointer to the buffer at the position
#define buffer_at_offset(buffer) ((buffer)->content + (buffer)->offset)

// Parse the input text to generate a number, and populate the result into item.
static yangbool parse_number(YangJson * const item, parse_buffer * const input_buffer)
{
    double number = 0;
    uint8_t *after_end = NULL;
    uint8_t number_c_string[64];
    uint8_t decimal_point = yang_json_get_decimal_point();
    size_t i = 0;

    if ((input_buffer == NULL) || (input_buffer->content == NULL))
    {
        return yangfalse;
    }

   // copy the number into a temporary buffer and replace '.' with the decimal point
    //of the current locale (for strtod)
   //This also takes care of '\0' not necessarily being available for marking the end of the input
    for (i = 0; (i < (sizeof(number_c_string) - 1)) && can_access_at_index(input_buffer, i); i++)
    {
        switch (buffer_at_offset(input_buffer)[i])
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '+':
            case '-':
            case 'e':
            case 'E':
                number_c_string[i] = buffer_at_offset(input_buffer)[i];
                break;

            case '.':
                number_c_string[i] = decimal_point;
                break;

            default:
                goto loop_end;
        }
    }
loop_end:
    number_c_string[i] = '\0';

    number = strtod((const char*)number_c_string, (char**)&after_end);
    if (number_c_string == after_end)
    {
        return yangfalse;// parse_error
    }

    item->valuedouble = number;

   // use saturation in case of overflow
    if (number >= INT_MAX)
    {
        item->valueint = INT_MAX;
    }
    else if (number <= (double)INT_MIN)
    {
        item->valueint = INT_MIN;
    }
    else
    {
        item->valueint = (int)number;
    }

    item->type = Yang_JSON_Number;

    input_buffer->offset += (size_t)(after_end - number_c_string);
    return yangtrue;
}

// don't ask me, but the original JSON_SetNumberValue returns an integer or double
double yang_json_setNumberHelper(YangJson *object, double number)
{
    if (number >= INT_MAX)
    {
        object->valueint = INT_MAX;
    }
    else if (number <= (double)INT_MIN)
    {
        object->valueint = INT_MIN;
    }
    else
    {
        object->valueint = (int)number;
    }

    return object->valuedouble = number;
}

char* yang_json_setValuestring(YangJson *object, const char *valuestring)
{
    char *copy = NULL;
   // if object's type is not JSON_String or is JSON_IsReference, it should not set valuestring
    if (!(object->type & Yang_JSON_String) || (object->type & Yang_JSON_IsReference))
    {
        return NULL;
    }
    if (strlen(valuestring) <= strlen(object->valuestring))
    {
        strcpy(object->valuestring, valuestring);
        return object->valuestring;
    }
    copy = (char*) yang_json_strdup((const uint8_t*)valuestring, &global_hooks);
    if (copy == NULL)
    {
        return NULL;
    }
    if (object->valuestring != NULL)
    {
        yang_json_free(object->valuestring);
    }
    object->valuestring = copy;

    return copy;
}

typedef struct
{
    uint8_t *buffer;
    size_t length;
    size_t offset;
    size_t depth;// current nesting depth (for formatted printing)
    yangbool noalloc;
    yangbool format;// is this print a formatted print
    internal_hooks hooks;
} printbuffer;

// realloc printbuffer if necessary to have at least "needed" bytes more
static uint8_t* ensure(printbuffer * const p, size_t needed)
{
    uint8_t *newbuffer = NULL;
    size_t newsize = 0;

    if ((p == NULL) || (p->buffer == NULL))
    {
        return NULL;
    }

    if ((p->length > 0) && (p->offset >= p->length))
    {
       // make sure that offset is valid
        return NULL;
    }

    if (needed > INT_MAX)
    {
       // sizes bigger than INT_MAX are currently not supported
        return NULL;
    }

    needed += p->offset + 1;
    if (needed <= p->length)
    {
        return p->buffer + p->offset;
    }

    if (p->noalloc) {
        return NULL;
    }

   // calculate new buffer size
    if (needed > (INT_MAX / 2))
    {
       // overflow of int, use INT_MAX if possible
        if (needed <= INT_MAX)
        {
            newsize = INT_MAX;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        newsize = needed * 2;
    }

    if (p->hooks.reallocate != NULL)
    {
       // reallocate with realloc if available
        newbuffer = (uint8_t*)p->hooks.reallocate(p->buffer, newsize);
        if (newbuffer == NULL)
        {
            p->hooks.deallocate(p->buffer);
            p->length = 0;
            p->buffer = NULL;

            return NULL;
        }
    }
    else
    {
       // otherwise reallocate manually
        newbuffer = (uint8_t*)p->hooks.allocate(newsize);
        if (!newbuffer)
        {
            p->hooks.deallocate(p->buffer);
            p->length = 0;
            p->buffer = NULL;

            return NULL;
        }
        
        yang_memcpy(newbuffer, p->buffer, p->offset + 1);
        p->hooks.deallocate(p->buffer);
    }
    p->length = newsize;
    p->buffer = newbuffer;

    return newbuffer + p->offset;
}

// calculate the new length of the string in a printbuffer and update the offset
static void update_offset(printbuffer * const buffer)
{
    const uint8_t *buffer_pointer = NULL;
    if ((buffer == NULL) || (buffer->buffer == NULL))
    {
        return;
    }
    buffer_pointer = buffer->buffer + buffer->offset;

    buffer->offset += strlen((const char*)buffer_pointer);
}

// securely comparison of floating-point variables
static yangbool compare_double(double a, double b)
{
    double maxVal = fabs(a) > fabs(b) ? fabs(a) : fabs(b);
    return (fabs(a - b) <= maxVal * DBL_EPSILON);
}

// Render the number nicely from the given item into a string.
static yangbool print_number(const YangJson * const item, printbuffer * const output_buffer)
{
    uint8_t *output_pointer = NULL;
    double d = item->valuedouble;
    int length = 0;
    size_t i = 0;
    uint8_t number_buffer[26] = {0};// temporary buffer to print the number into
    uint8_t decimal_point = yang_json_get_decimal_point();
    double test = 0.0;

    if (output_buffer == NULL)
    {
        return yangfalse;
    }

   // This checks for NaN and Infinity
    if (isnan(d) || isinf(d))
    {
        length = yang_sprintf((char*)number_buffer, "null");
    }
    else
    {
       // Try 15 decimal places of precision to avoid nonsignificant nonzero digits
        length = yang_sprintf((char*)number_buffer, "%1.15g", d);

       // Check whether the original double can be recovered
        if ((sscanf((char*)number_buffer, "%lg", &test) != 1) || !compare_double((double)test, d))
        {
           // If not, print with 17 decimal places of precision
            length = yang_sprintf((char*)number_buffer, "%1.17g", d);
        }
    }

   // yang_sprintf failed or buffer overrun occurred
    if ((length < 0) || (length > (int)(sizeof(number_buffer) - 1)))
    {
        return yangfalse;
    }

   // reserve appropriate space in the output
    output_pointer = ensure(output_buffer, (size_t)length + sizeof(""));
    if (output_pointer == NULL)
    {
        return yangfalse;
    }

   // copy the printed number to the output and replace locale
    //dependent decimal point with '.'
    for (i = 0; i < ((size_t)length); i++)
    {
        if (number_buffer[i] == decimal_point)
        {
            output_pointer[i] = '.';
            continue;
        }

        output_pointer[i] = number_buffer[i];
    }
    output_pointer[i] = '\0';

    output_buffer->offset += (size_t)length;

    return yangtrue;
}

// parse 4 digit hexadecimal number
static unsigned parse_hex4(const uint8_t * const input)
{
    unsigned int h = 0;
    size_t i = 0;

    for (i = 0; i < 4; i++)
    {
       // parse digit
        if ((input[i] >= '0') && (input[i] <= '9'))
        {
            h += (unsigned int) input[i] - '0';
        }
        else if ((input[i] >= 'A') && (input[i] <= 'F'))
        {
            h += (unsigned int) 10 + input[i] - 'A';
        }
        else if ((input[i] >= 'a') && (input[i] <= 'f'))
        {
            h += (unsigned int) 10 + input[i] - 'a';
        }
        else// invalid
        {
            return 0;
        }

        if (i < 3)
        {
           // shift left to make place for the next nibble
            h = h << 4;
        }
    }

    return h;
}

// converts a UTF-16 literal to UTF-8
//A literal can be one or two sequences of the form \uXXXX
static uint8_t utf16_literal_to_utf8(const uint8_t * const input_pointer, const uint8_t * const input_end, uint8_t **output_pointer)
{
    long unsigned int codepoint = 0;
    unsigned int first_code = 0;
    const uint8_t *first_sequence = input_pointer;
    uint8_t utf8_length = 0;
    uint8_t utf8_position = 0;
    uint8_t sequence_length = 0;
    uint8_t first_byte_mark = 0;

    if ((input_end - first_sequence) < 6)
    {
       // input ends unexpectedly
        goto fail;
    }

   // get the first utf16 sequence
    first_code = parse_hex4(first_sequence + 2);

   // check that the code is valid
    if (((first_code >= 0xDC00) && (first_code <= 0xDFFF)))
    {
        goto fail;
    }

   // UTF16 surrogate pair
    if ((first_code >= 0xD800) && (first_code <= 0xDBFF))
    {
        const uint8_t *second_sequence = first_sequence + 6;
        unsigned int second_code = 0;
        sequence_length = 12;// \uXXXX\uXXXX

        if ((input_end - second_sequence) < 6)
        {
           // input ends unexpectedly
            goto fail;
        }

        if ((second_sequence[0] != '\\') || (second_sequence[1] != 'u'))
        {
           // missing second half of the surrogate pair
            goto fail;
        }

       // get the second utf16 sequence
        second_code = parse_hex4(second_sequence + 2);
       // check that the code is valid
        if ((second_code < 0xDC00) || (second_code > 0xDFFF))
        {
           // invalid second half of the surrogate pair
            goto fail;
        }


       // calculate the unicode codepoint from the surrogate pair
        codepoint = 0x10000 + (((first_code & 0x3FF) << 10) | (second_code & 0x3FF));
    }
    else
    {
        sequence_length = 6;// \uXXXX
        codepoint = first_code;
    }

   // encode as UTF-8
     // takes at maximum 4 bytes to encode:
    //11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    if (codepoint < 0x80)
    {
       // normal ascii, encoding 0xxxxxxx
        utf8_length = 1;
    }
    else if (codepoint < 0x800)
    {
       // two bytes, encoding 110xxxxx 10xxxxxx
        utf8_length = 2;
        first_byte_mark = 0xC0;// 11000000
    }
    else if (codepoint < 0x10000)
    {
       // three bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx
        utf8_length = 3;
        first_byte_mark = 0xE0;// 11100000
    }
    else if (codepoint <= 0x10FFFF)
    {
       // four bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx 10xxxxxx
        utf8_length = 4;
        first_byte_mark = 0xF0;// 11110000
    }
    else
    {
       // invalid unicode codepoint
        goto fail;
    }

   // encode as utf8
    for (utf8_position = (uint8_t)(utf8_length - 1); utf8_position > 0; utf8_position--)
    {
       // 10xxxxxx
        (*output_pointer)[utf8_position] = (uint8_t)((codepoint | 0x80) & 0xBF);
        codepoint >>= 6;
    }
   // encode first byte
    if (utf8_length > 1)
    {
        (*output_pointer)[0] = (uint8_t)((codepoint | first_byte_mark) & 0xFF);
    }
    else
    {
        (*output_pointer)[0] = (uint8_t)(codepoint & 0x7F);
    }

    *output_pointer += utf8_length;

    return sequence_length;

fail:
    return 0;
}

// Parse the input text into an unescaped cinput, and populate item.
static yangbool parse_string(YangJson * const item, parse_buffer * const input_buffer)
{
    const uint8_t *input_pointer = buffer_at_offset(input_buffer) + 1;
    const uint8_t *input_end = buffer_at_offset(input_buffer) + 1;
    uint8_t *output_pointer = NULL;
    uint8_t *output = NULL;

   // not a string
    if (buffer_at_offset(input_buffer)[0] != '\"')
    {
        goto fail;
    }

    {
       // calculate approximate size of the output (overestimate)
        size_t allocation_length = 0;
        size_t skipped_bytes = 0;
        while (((size_t)(input_end - input_buffer->content) < input_buffer->length) && (*input_end != '\"'))
        {
           // is escape sequence
            if (input_end[0] == '\\')
            {
                if ((size_t)(input_end + 1 - input_buffer->content) >= input_buffer->length)
                {
                   // prevent buffer overflow when last input character is a backslash
                    goto fail;
                }
                skipped_bytes++;
                input_end++;
            }
            input_end++;
        }
        if (((size_t)(input_end - input_buffer->content) >= input_buffer->length) || (*input_end != '\"'))
        {
            goto fail;// string ended unexpectedly
        }

       // This is at most how much we need for the output
        allocation_length = (size_t) (input_end - buffer_at_offset(input_buffer)) - skipped_bytes;
        output = (uint8_t*)input_buffer->hooks.allocate(allocation_length + sizeof(""));
        if (output == NULL)
        {
            goto fail;// allocation failure
        }
    }

    output_pointer = output;
   // loop through the string literal
    while (input_pointer < input_end)
    {
        if (*input_pointer != '\\')
        {
            *output_pointer++ = *input_pointer++;
        }
       // escape sequence
        else
        {
            uint8_t sequence_length = 2;
            if ((input_end - input_pointer) < 1)
            {
                goto fail;
            }

            switch (input_pointer[1])
            {
                case 'b':
                    *output_pointer++ = '\b';
                    break;
                case 'f':
                    *output_pointer++ = '\f';
                    break;
                case 'n':
                    *output_pointer++ = '\n';
                    break;
                case 'r':
                    *output_pointer++ = '\r';
                    break;
                case 't':
                    *output_pointer++ = '\t';
                    break;
                case '\"':
                case '\\':
                case '/':
                    *output_pointer++ = input_pointer[1];
                    break;

               // UTF-16 literal
                case 'u':
                    sequence_length = utf16_literal_to_utf8(input_pointer, input_end, &output_pointer);
                    if (sequence_length == 0)
                    {
                       // failed to convert UTF16-literal to UTF-8
                        goto fail;
                    }
                    break;

                default:
                    goto fail;
            }
            input_pointer += sequence_length;
        }
    }

   // zero terminate the output
    *output_pointer = '\0';

    item->type = Yang_JSON_String;
    item->valuestring = (char*)output;

    input_buffer->offset = (size_t) (input_end - input_buffer->content);
    input_buffer->offset++;

    return yangtrue;

fail:
    if (output != NULL)
    {
        input_buffer->hooks.deallocate(output);
    }

    if (input_pointer != NULL)
    {
        input_buffer->offset = (size_t)(input_pointer - input_buffer->content);
    }

    return yangfalse;
}

// Render the cstring provided to an escaped version that can be printed.
static yangbool print_string_ptr(const uint8_t * const input, printbuffer * const output_buffer)
{
    const uint8_t *input_pointer = NULL;
    uint8_t *output = NULL;
    uint8_t *output_pointer = NULL;
    size_t output_length = 0;
   // numbers of additional characters needed for escaping
    size_t escape_characters = 0;

    if (output_buffer == NULL)
    {
        return yangfalse;
    }

   // empty string
    if (input == NULL)
    {
        output = ensure(output_buffer, sizeof("\"\""));
        if (output == NULL)
        {
            return yangfalse;
        }
        strcpy((char*)output, "\"\"");

        return yangtrue;
    }

   // set "flag" to 1 if something needs to be escaped
    for (input_pointer = input; *input_pointer; input_pointer++)
    {
        switch (*input_pointer)
        {
            case '\"':
            case '\\':
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
               // one character escape sequence
                escape_characters++;
                break;
            default:
                if (*input_pointer < 32)
                {
                   // UTF-16 escape sequence uXXXX
                    escape_characters += 5;
                }
                break;
        }
    }
    output_length = (size_t)(input_pointer - input) + escape_characters;

    output = ensure(output_buffer, output_length + sizeof("\"\""));
    if (output == NULL)
    {
        return yangfalse;
    }

   // no characters have to be escaped
    if (escape_characters == 0)
    {
        output[0] = '\"';
        yang_memcpy(output + 1, input, output_length);
        output[output_length + 1] = '\"';
        output[output_length + 2] = '\0';

        return yangtrue;
    }

    output[0] = '\"';
    output_pointer = output + 1;
   // copy the string
    for (input_pointer = input; *input_pointer != '\0'; (void)input_pointer++, output_pointer++)
    {
        if ((*input_pointer > 31) && (*input_pointer != '\"') && (*input_pointer != '\\'))
        {
           // normal character, copy
            *output_pointer = *input_pointer;
        }
        else
        {
           // character needs to be escaped
            *output_pointer++ = '\\';
            switch (*input_pointer)
            {
                case '\\':
                    *output_pointer = '\\';
                    break;
                case '\"':
                    *output_pointer = '\"';
                    break;
                case '\b':
                    *output_pointer = 'b';
                    break;
                case '\f':
                    *output_pointer = 'f';
                    break;
                case '\n':
                    *output_pointer = 'n';
                    break;
                case '\r':
                    *output_pointer = 'r';
                    break;
                case '\t':
                    *output_pointer = 't';
                    break;
                default:
                   // escape and print as unicode codepoint
                    yang_sprintf((char*)output_pointer, "u%04x", *input_pointer);
                    output_pointer += 4;
                    break;
            }
        }
    }
    output[output_length + 1] = '\"';
    output[output_length + 2] = '\0';

    return yangtrue;
}

// Invoke print_string_ptr (which is useful) on an item.
static yangbool print_string(const YangJson * const item, printbuffer * const p)
{
    return print_string_ptr((uint8_t*)item->valuestring, p);
}

// Predeclare these prototypes.
static yangbool parse_value(YangJson * const item, parse_buffer * const input_buffer);
static yangbool print_value(const YangJson * const item, printbuffer * const output_buffer);
static yangbool parse_array(YangJson * const item, parse_buffer * const input_buffer);
static yangbool print_array(const YangJson * const item, printbuffer * const output_buffer);
static yangbool parse_object(YangJson * const item, parse_buffer * const input_buffer);
static yangbool print_object(const YangJson * const item, printbuffer * const output_buffer);

// Utility to jump whitespace and cr/lf
static parse_buffer *buffer_skip_whitespace(parse_buffer * const buffer)
{
    if ((buffer == NULL) || (buffer->content == NULL))
    {
        return NULL;
    }

    if (cannot_access_at_index(buffer, 0))
    {
        return buffer;
    }

    while (can_access_at_index(buffer, 0) && (buffer_at_offset(buffer)[0] <= 32))
    {
       buffer->offset++;
    }

    if (buffer->offset == buffer->length)
    {
        buffer->offset--;
    }

    return buffer;
}

// skip the UTF-8 BOM (byte order mark) if it is at the beginning of a buffer
static parse_buffer *skip_utf8_bom(parse_buffer * const buffer)
{
    if ((buffer == NULL) || (buffer->content == NULL) || (buffer->offset != 0))
    {
        return NULL;
    }

    if (can_access_at_index(buffer, 4) && (strncmp((const char*)buffer_at_offset(buffer), "\xEF\xBB\xBF", 3) == 0))
    {
        buffer->offset += 3;
    }

    return buffer;
}

YangJson * yang_json_parseWithOpts(const char *value, const char **return_parse_end, yangbool require_null_terminated)
{
    size_t buffer_length;

    if (NULL == value)
    {
        return NULL;
    }

   // Adding null character size due to require_null_terminated.
    buffer_length = strlen(value) + sizeof("");

    return yang_json_parseWithLengthOpts(value, buffer_length, return_parse_end, require_null_terminated);
}

// Parse an object - create a new root, and populate.
YangJson * yang_json_parseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, yangbool require_null_terminated)
{
    parse_buffer buffer = { 0, 0, 0, 0, { 0, 0, 0 } };
    YangJson *item = NULL;

   // reset error position
    global_error.json = NULL;
    global_error.position = 0;

    if (value == NULL || 0 == buffer_length)
    {
        goto fail;
    }

    buffer.content = (const uint8_t*)value;
    buffer.length = buffer_length; 
    buffer.offset = 0;
    buffer.hooks = global_hooks;

    item = yang_json_new_item(&global_hooks);
    if (item == NULL)// memory fail
    {
        goto fail;
    }

    if (!parse_value(item, buffer_skip_whitespace(skip_utf8_bom(&buffer))))
    {
       // parse failure. ep is set.
        goto fail;
    }

   // if we require null-terminated JSON without appended garbage, skip and then check for a null terminator
    if (require_null_terminated)
    {
        buffer_skip_whitespace(&buffer);
        if ((buffer.offset >= buffer.length) || buffer_at_offset(&buffer)[0] != '\0')
        {
            goto fail;
        }
    }
    if (return_parse_end)
    {
        *return_parse_end = (const char*)buffer_at_offset(&buffer);
    }

    return item;

fail:
    if (item != NULL)
    {
        yang_json_delete(item);
    }

    if (value != NULL)
    {
        yangjsonerror local_error;
        local_error.json = (const uint8_t*)value;
        local_error.position = 0;

        if (buffer.offset < buffer.length)
        {
            local_error.position = buffer.offset;
        }
        else if (buffer.length > 0)
        {
            local_error.position = buffer.length - 1;
        }

        if (return_parse_end != NULL)
        {
            *return_parse_end = (const char*)local_error.json + local_error.position;
        }

        global_error = local_error;
    }

    return NULL;
}

// Default options for JSON_Parse
YangJson * yang_json_parse(const char *value)
{
    return yang_json_parseWithOpts(value, 0, 0);
}

YangJson * yang_json_parseWithLength(const char *value, size_t buffer_length)
{
    return yang_json_parseWithLengthOpts(value, buffer_length, 0, 0);
}



static uint8_t *print(YangJson *  item, yangbool format, const internal_hooks * const hooks)
{
    static const size_t default_buffer_size = 256;
    printbuffer buffer[1];
    uint8_t *printed = NULL;

    yang_memset(buffer, 0, sizeof(buffer));

   // create buffer
    buffer->buffer = (uint8_t*) hooks->allocate(default_buffer_size);
    buffer->length = default_buffer_size;
    buffer->format = format;
    buffer->hooks = *hooks;
    if (buffer->buffer == NULL)
    {
        goto fail;
    }

   // print the value
    if (!print_value(item, buffer))
    {
        goto fail;
    }
    update_offset(buffer);

   // check if reallocate is available
    if (hooks->reallocate != NULL)
    {
        printed = (uint8_t*) hooks->reallocate(buffer->buffer, buffer->offset + 1);
        if (printed == NULL) {
            goto fail;
        }
        buffer->buffer = NULL;
    }
    else// otherwise copy the JSON over to a new buffer
    {
        printed = (uint8_t*) hooks->allocate(buffer->offset + 1);
        if (printed == NULL)
        {
            goto fail;
        }
        yang_memcpy(printed, buffer->buffer, yang_min(buffer->length, buffer->offset + 1));
        printed[buffer->offset] = '\0';// just to be sure

       // free the buffer
        hooks->deallocate(buffer->buffer);
    }
    yang_free(item->outString);
    item->outString=printed;
    return printed;

fail:
    if (buffer->buffer != NULL)
    {
        hooks->deallocate(buffer->buffer);
    }

    if (printed != NULL)
    {
        hooks->deallocate(printed);
    }

    return NULL;
}

// Render a JSON item/entity/structure to text.
char * yang_json_print( YangJson *item)
{
    return (char*)print(item, yangtrue, &global_hooks);
}

char * yang_json_printUnformatted(YangJson *item)
{
    return (char*)print(item, yangfalse, &global_hooks);
}

char * yang_json_printBuffered(YangJson *item, int prebuffer, yangbool fmt)
{
    printbuffer p = { 0, 0, 0, 0, 0, 0, { 0, 0, 0 } };

    if (prebuffer < 0)
    {
        return NULL;
    }

    p.buffer = (uint8_t*)global_hooks.allocate((size_t)prebuffer);
    if (!p.buffer)
    {
        return NULL;
    }

    p.length = (size_t)prebuffer;
    p.offset = 0;
    p.noalloc = yangfalse;
    p.format = fmt;
    p.hooks = global_hooks;

    if (!print_value(item, &p))
    {
        global_hooks.deallocate(p.buffer);
        return NULL;
    }
    yang_free(item->outString);
    item->outString=p.buffer;
    return (char*)p.buffer;
}

yangbool yang_json_printPreallocated(YangJson *item, char *buffer, const int length, const yangbool format)
{
    printbuffer p = { 0, 0, 0, 0, 0, 0, { 0, 0, 0 } };

    if ((length < 0) || (buffer == NULL))
    {
        return yangfalse;
    }

    p.buffer = (uint8_t*)buffer;
    p.length = (size_t)length;
    p.offset = 0;
    p.noalloc = yangtrue;
    p.format = format;
    p.hooks = global_hooks;

    return print_value(item, &p);
}

// Parser core - when encountering text, process appropriately.
static yangbool parse_value(YangJson * const item, parse_buffer * const input_buffer)
{
    if ((input_buffer == NULL) || (input_buffer->content == NULL))
    {
        return yangfalse;// no input
    }

   // parse the different types of values
   // null
    if (can_read(input_buffer, 4) && (strncmp((const char*)buffer_at_offset(input_buffer), "null", 4) == 0))
    {
        item->type = Yang_JSON_NULL;
        input_buffer->offset += 4;
        return yangtrue;
    }
   // yangfalse
    if (can_read(input_buffer, 5) && (strncmp((const char*)buffer_at_offset(input_buffer), "false", 5) == 0))
    {
        item->type = Yang_JSON_False;
        input_buffer->offset += 5;
        return yangtrue;
    }
   // yangtrue
    if (can_read(input_buffer, 4) && (strncmp((const char*)buffer_at_offset(input_buffer), "true", 4) == 0))
    {
        item->type = Yang_JSON_True;
        item->valueint = 1;
        input_buffer->offset += 4;
        return yangtrue;
    }
   // string
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '\"'))
    {
        return parse_string(item, input_buffer);
    }
   // number
    if (can_access_at_index(input_buffer, 0) && ((buffer_at_offset(input_buffer)[0] == '-') || ((buffer_at_offset(input_buffer)[0] >= '0') && (buffer_at_offset(input_buffer)[0] <= '9'))))
    {
        return parse_number(item, input_buffer);
    }
   // array
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '['))
    {
        return parse_array(item, input_buffer);
    }
   // object
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '{'))
    {
        return parse_object(item, input_buffer);
    }

    return yangfalse;
}

// Render a value to text.
static yangbool print_value(const YangJson * const item, printbuffer * const output_buffer)
{
    uint8_t *output = NULL;

    if ((item == NULL) || (output_buffer == NULL))
    {
        return yangfalse;
    }

    switch ((item->type) & 0xFF)
    {
        case Yang_JSON_NULL:
            output = ensure(output_buffer, 5);
            if (output == NULL)
            {
                return yangfalse;
            }
            strcpy((char*)output, "null");
            return yangtrue;

        case Yang_JSON_False:
            output = ensure(output_buffer, 6);
            if (output == NULL)
            {
                return yangfalse;
            }
            strcpy((char*)output, "false");
            return yangtrue;

        case Yang_JSON_True:
            output = ensure(output_buffer, 5);
            if (output == NULL)
            {
                return yangfalse;
            }
            strcpy((char*)output, "true");
            return yangtrue;

        case Yang_JSON_Number:
            return print_number(item, output_buffer);

        case Yang_JSON_Raw:
        {
            size_t raw_length = 0;
            if (item->valuestring == NULL)
            {
                return yangfalse;
            }

            raw_length = strlen(item->valuestring) + sizeof("");
            output = ensure(output_buffer, raw_length);
            if (output == NULL)
            {
                return yangfalse;
            }
            yang_memcpy(output, item->valuestring, raw_length);
            return yangtrue;
        }

        case Yang_JSON_String:
            return print_string(item, output_buffer);

        case Yang_JSON_Array:
            return print_array(item, output_buffer);

        case Yang_JSON_Object:
            return print_object(item, output_buffer);

        default:
            return yangfalse;
    }
}

// Build an array from input text.
static yangbool parse_array(YangJson * const item, parse_buffer * const input_buffer)
{
    YangJson *head = NULL;// head of the linked list
    YangJson *current_item = NULL;

    if (input_buffer->depth >= Yang_JSON_NESTING_LIMIT)
    {
        return yangfalse;// to deeply nested
    }
    input_buffer->depth++;

    if (buffer_at_offset(input_buffer)[0] != '[')
    {
       // not an array
        goto fail;
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ']'))
    {
       // empty array
        goto success;
    }

   // check if we skipped to the end of the buffer
    if (cannot_access_at_index(input_buffer, 0))
    {
        input_buffer->offset--;
        goto fail;
    }

   // step back to character in front of the first element
    input_buffer->offset--;
   // loop through the comma separated array elements
    do
    {
       // allocate next item
        YangJson *new_item = yang_json_new_item(&(input_buffer->hooks));
        if (new_item == NULL)
        {
            goto fail;// allocation failure
        }

       // attach next item to list
        if (head == NULL)
        {
           // start the linked list
            current_item = head = new_item;
        }
        else
        {
           // add to the end and advance
            current_item->next = new_item;
            new_item->prev = current_item;
            current_item = new_item;
        }

       // parse next value
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_value(current_item, input_buffer))
        {
            goto fail;// failed to parse value
        }
        buffer_skip_whitespace(input_buffer);
    }
    while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || buffer_at_offset(input_buffer)[0] != ']')
    {
        goto fail;// expected end of array
    }

success:
    input_buffer->depth--;

    if (head != NULL) {
        head->prev = current_item;
    }

    item->type = Yang_JSON_Array;
    item->child = head;

    input_buffer->offset++;

    return yangtrue;

fail:
    if (head != NULL)
    {
        yang_json_delete(head);
    }

    return yangfalse;
}

// Render an array to text
static yangbool print_array(const YangJson * const item, printbuffer * const output_buffer)
{
    uint8_t *output_pointer = NULL;
    size_t length = 0;
    YangJson *current_element = item->child;

    if (output_buffer == NULL)
    {
        return yangfalse;
    }

   // Compose the output array.
   // opening square bracket
    output_pointer = ensure(output_buffer, 1);
    if (output_pointer == NULL)
    {
        return yangfalse;
    }

    *output_pointer = '[';
    output_buffer->offset++;
    output_buffer->depth++;

    while (current_element != NULL)
    {
        if (!print_value(current_element, output_buffer))
        {
            return yangfalse;
        }
        update_offset(output_buffer);
        if (current_element->next)
        {
            length = (size_t) (output_buffer->format ? 2 : 1);
            output_pointer = ensure(output_buffer, length + 1);
            if (output_pointer == NULL)
            {
                return yangfalse;
            }
            *output_pointer++ = ',';
            if(output_buffer->format)
            {
                *output_pointer++ = ' ';
            }
            *output_pointer = '\0';
            output_buffer->offset += length;
        }
        current_element = current_element->next;
    }

    output_pointer = ensure(output_buffer, 2);
    if (output_pointer == NULL)
    {
        return yangfalse;
    }
    *output_pointer++ = ']';
    *output_pointer = '\0';
    output_buffer->depth--;

    return yangtrue;
}

// Build an object from the text.
static yangbool parse_object(YangJson * const item, parse_buffer * const input_buffer)
{
    YangJson *head = NULL;// linked list head
    YangJson *current_item = NULL;

    if (input_buffer->depth >= Yang_JSON_NESTING_LIMIT)
    {
        return yangfalse;// to deeply nested
    }
    input_buffer->depth++;

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '{'))
    {
        goto fail;// not an object
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '}'))
    {
        goto success;// empty object
    }

   // check if we skipped to the end of the buffer
    if (cannot_access_at_index(input_buffer, 0))
    {
        input_buffer->offset--;
        goto fail;
    }

   // step back to character in front of the first element
    input_buffer->offset--;
   // loop through the comma separated array elements
    do
    {
       // allocate next item
        YangJson *new_item = yang_json_new_item(&(input_buffer->hooks));
        if (new_item == NULL)
        {
            goto fail;// allocation failure
        }

       // attach next item to list
        if (head == NULL)
        {
           // start the linked list
            current_item = head = new_item;
        }
        else
        {
           // add to the end and advance
            current_item->next = new_item;
            new_item->prev = current_item;
            current_item = new_item;
        }

       // parse the name of the child
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_string(current_item, input_buffer))
        {
            goto fail;// failed to parse name
        }
        buffer_skip_whitespace(input_buffer);

       // swap valuestring and string, because we parsed the name
        current_item->string = current_item->valuestring;
        current_item->valuestring = NULL;

        if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != ':'))
        {
            goto fail;// invalid object
        }

       // parse the value
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_value(current_item, input_buffer))
        {
            goto fail;// failed to parse value
        }
        buffer_skip_whitespace(input_buffer);
    }
    while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '}'))
    {
        goto fail;// expected end of object
    }

success:
    input_buffer->depth--;

    if (head != NULL) {
        head->prev = current_item;
    }

    item->type = Yang_JSON_Object;
    item->child = head;

    input_buffer->offset++;
    return yangtrue;

fail:
    if (head != NULL)
    {
        yang_json_delete(head);
    }

    return yangfalse;
}

// Render an object to text.
static yangbool print_object(const YangJson * const item, printbuffer * const output_buffer)
{
    uint8_t *output_pointer = NULL;
    size_t length = 0;
    YangJson *current_item = item->child;

    if (output_buffer == NULL)
    {
        return yangfalse;
    }

   // Compose the output:
    length = (size_t) (output_buffer->format ? 2 : 1);// fmt: {\n
    output_pointer = ensure(output_buffer, length + 1);
    if (output_pointer == NULL)
    {
        return yangfalse;
    }

    *output_pointer++ = '{';
    output_buffer->depth++;
    if (output_buffer->format)
    {
        *output_pointer++ = '\n';
    }
    output_buffer->offset += length;

    while (current_item)
    {
        if (output_buffer->format)
        {
            size_t i;
            output_pointer = ensure(output_buffer, output_buffer->depth);
            if (output_pointer == NULL)
            {
                return yangfalse;
            }
            for (i = 0; i < output_buffer->depth; i++)
            {
                *output_pointer++ = '\t';
            }
            output_buffer->offset += output_buffer->depth;
        }

       // print key
        if (!print_string_ptr((uint8_t*)current_item->string, output_buffer))
        {
            return yangfalse;
        }
        update_offset(output_buffer);

        length = (size_t) (output_buffer->format ? 2 : 1);
        output_pointer = ensure(output_buffer, length);
        if (output_pointer == NULL)
        {
            return yangfalse;
        }
        *output_pointer++ = ':';
        if (output_buffer->format)
        {
            *output_pointer++ = '\t';
        }
        output_buffer->offset += length;

       // print value
        if (!print_value(current_item, output_buffer))
        {
            return yangfalse;
        }
        update_offset(output_buffer);

       // print comma if not last
        length = ((size_t)(output_buffer->format ? 1 : 0) + (size_t)(current_item->next ? 1 : 0));
        output_pointer = ensure(output_buffer, length + 1);
        if (output_pointer == NULL)
        {
            return yangfalse;
        }
        if (current_item->next)
        {
            *output_pointer++ = ',';
        }

        if (output_buffer->format)
        {
            *output_pointer++ = '\n';
        }
        *output_pointer = '\0';
        output_buffer->offset += length;

        current_item = current_item->next;
    }

    output_pointer = ensure(output_buffer, output_buffer->format ? (output_buffer->depth + 1) : 2);
    if (output_pointer == NULL)
    {
        return yangfalse;
    }
    if (output_buffer->format)
    {
        size_t i;
        for (i = 0; i < (output_buffer->depth - 1); i++)
        {
            *output_pointer++ = '\t';
        }
    }
    *output_pointer++ = '}';
    *output_pointer = '\0';
    output_buffer->depth--;

    return yangtrue;
}

// Get Array size/item / object item.
int yang_json_getArraySize(const YangJson *array)
{
    YangJson *child = NULL;
    size_t size = 0;

    if (array == NULL)
    {
        return 0;
    }

    child = array->child;

    while(child != NULL)
    {
        size++;
        child = child->next;
    }

   // FIXME: Can overflow here. Cannot be fixed without breaking the API

    return (int)size;
}

static YangJson* get_array_item(const YangJson *array, size_t index)
{
    YangJson *current_child = NULL;

    if (array == NULL)
    {
        return NULL;
    }

    current_child = array->child;
    while ((current_child != NULL) && (index > 0))
    {
        index--;
        current_child = current_child->next;
    }

    return current_child;
}

YangJson * yang_json_getArrayItem(const YangJson *array, int index)
{
    if (index < 0)
    {
        return NULL;
    }

    return get_array_item(array, (size_t)index);
}

static YangJson *get_object_item(const YangJson * const object, const char * const name, const yangbool case_sensitive)
{
    YangJson *current_element = NULL;

    if ((object == NULL) || (name == NULL))
    {
        return NULL;
    }

    current_element = object->child;
    if (case_sensitive)
    {
        while ((current_element != NULL) && (current_element->string != NULL) && (strcmp(name, current_element->string) != 0))
        {
            current_element = current_element->next;
        }
    }
    else
    {
        while ((current_element != NULL) && (yang_json_case_insensitive_strcmp((const uint8_t*)name, (const uint8_t*)(current_element->string)) != 0))
        {
            current_element = current_element->next;
        }
    }

    if ((current_element == NULL) || (current_element->string == NULL)) {
        return NULL;
    }

    return current_element;
}

YangJson * yang_json_getObjectItem(const YangJson * const object, const char * const string)
{
    return get_object_item(object, string, yangfalse);
}

YangJson * yang_json_getObjectItemCaseSensitive(const YangJson * const object, const char * const string)
{
    return get_object_item(object, string, yangtrue);
}

yangbool yang_json_hasObjectItem(const YangJson *object, const char *string)
{
    return yang_json_getObjectItem(object, string) ? 1 : 0;
}

// Utility for array list handling.
static void suffix_object(YangJson *prev, YangJson *item)
{
    prev->next = item;
    item->prev = prev;
}

// Utility for handling references.
static YangJson *create_reference(const YangJson *item, const internal_hooks * const hooks)
{
    YangJson *reference = NULL;
    if (item == NULL)
    {
        return NULL;
    }

    reference = yang_json_new_item(hooks);
    if (reference == NULL)
    {
        return NULL;
    }

    yang_memcpy(reference, item, sizeof(YangJson));
    reference->string = NULL;
    reference->type |= Yang_JSON_IsReference;
    reference->next = reference->prev = NULL;
    return reference;
}

static yangbool add_item_to_array(YangJson *array, YangJson *item)
{
    YangJson *child = NULL;

    if ((item == NULL) || (array == NULL) || (array == item))
    {
        return yangfalse;
    }

    child = array->child;
   //
    // To find the last item in array quickly, we use prev in array

    if (child == NULL)
    {
       // list is empty, start new one
        array->child = item;
        item->prev = item;
        item->next = NULL;
    }
    else
    {
       // append to the end
        if (child->prev)
        {
            suffix_object(child->prev, item);
            array->child->prev = item;
        }
    }

    return yangtrue;
}

// Add item to array/object.
yangbool yang_json_addItemToArray(YangJson *array, YangJson *item)
{
    return add_item_to_array(array, item);
}

#if defined(__clang__) || (defined(__GNUC__)  && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 5))))
    #pragma GCC diagnostic push
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
// helper function to cast away const
static void* cast_away_const(const void* string)
{
    return (void*)string;
}
#if defined(__clang__) || (defined(__GNUC__)  && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 5))))
    #pragma GCC diagnostic pop
#endif


static yangbool add_item_to_object(YangJson * const object, const char * const string, YangJson * const item, const internal_hooks * const hooks, const yangbool constant_key)
{
    char *new_key = NULL;
    int new_type = Yang_JSON_Invalid;

    if ((object == NULL) || (string == NULL) || (item == NULL) || (object == item))
    {
        return yangfalse;
    }

    if (constant_key)
    {
        new_key = (char*)cast_away_const(string);
        new_type = item->type | Yang_JSON_StringIsConst;
    }
    else
    {
        new_key = (char*)yang_json_strdup((const uint8_t*)string, hooks);
        if (new_key == NULL)
        {
            return yangfalse;
        }

        new_type = item->type & ~Yang_JSON_StringIsConst;
    }

    if (!(item->type & Yang_JSON_StringIsConst) && (item->string != NULL))
    {
        hooks->deallocate(item->string);
    }

    item->string = new_key;
    item->type = new_type;

    return add_item_to_array(object, item);
}

yangbool yang_json_addItemToObject(YangJson *object, const char *string, YangJson *item)
{
    return add_item_to_object(object, string, item, &global_hooks, yangfalse);
}

// Add an item to an object with constant string as key
yangbool yang_json_addItemToObjectCS(YangJson *object, const char *string, YangJson *item)
{
    return add_item_to_object(object, string, item, &global_hooks, yangtrue);
}

yangbool yang_json_addItemReferenceToArray(YangJson *array, YangJson *item)
{
    if (array == NULL)
    {
        return yangfalse;
    }

    return add_item_to_array(array, create_reference(item, &global_hooks));
}

yangbool yang_json_addItemReferenceToObject(YangJson *object, const char *string, YangJson *item)
{
    if ((object == NULL) || (string == NULL))
    {
        return yangfalse;
    }

    return add_item_to_object(object, string, create_reference(item, &global_hooks), &global_hooks, yangfalse);
}

YangJson* yang_json_addNullToObject(YangJson * const object, const char * const name)
{
    YangJson *null = yang_json_createNull();
    if (add_item_to_object(object, name, null, &global_hooks, yangfalse))
    {
        return null;
    }

    yang_json_delete(null);
    return NULL;
}

YangJson* yang_json_addTrueToObject(YangJson * const object, const char * const name)
{
    YangJson *true_item = yang_json_createTrue();
    if (add_item_to_object(object, name, true_item, &global_hooks, yangfalse))
    {
        return true_item;
    }

    yang_json_delete(true_item);
    return NULL;
}

YangJson* yang_json_addFalseToObject(YangJson * const object, const char * const name)
{
    YangJson *false_item = yang_json_createFalse();
    if (add_item_to_object(object, name, false_item, &global_hooks, yangfalse))
    {
        return false_item;
    }

    yang_json_delete(false_item);
    return NULL;
}

YangJson* yang_json_addBoolToObject(YangJson * const object, const char * const name, const yangbool boolean)
{
    YangJson *bool_item = yang_json_createBool(boolean);
    if (add_item_to_object(object, name, bool_item, &global_hooks, yangfalse))
    {
        return bool_item;
    }

    yang_json_delete(bool_item);
    return NULL;
}

YangJson* yang_json_addNumberToObject(YangJson * const object, const char * const name, const double number)
{
    YangJson *number_item = yang_json_createNumber(number);
    if (add_item_to_object(object, name, number_item, &global_hooks, yangfalse))
    {
        return number_item;
    }

    yang_json_delete(number_item);
    return NULL;
}

YangJson* yang_json_addStringToObject(YangJson * const object, const char * const name, const char * const string)
{
    YangJson *string_item = yang_json_createString(string);
    if (add_item_to_object(object, name, string_item, &global_hooks, yangfalse))
    {
        return string_item;
    }

    yang_json_delete(string_item);
    return NULL;
}

YangJson* yang_json_addRawToObject(YangJson * const object, const char * const name, const char * const raw)
{
    YangJson *raw_item = yang_json_createRaw(raw);
    if (add_item_to_object(object, name, raw_item, &global_hooks, yangfalse))
    {
        return raw_item;
    }

    yang_json_delete(raw_item);
    return NULL;
}

YangJson* yang_json_addObjectToObject(YangJson * const object, const char * const name)
{
    YangJson *object_item = yang_json_createObject();
    if (add_item_to_object(object, name, object_item, &global_hooks, yangfalse))
    {
        return object_item;
    }

    yang_json_delete(object_item);
    return NULL;
}

YangJson* yang_json_addArrayToObject(YangJson * const object, const char * const name)
{
    YangJson *array = yang_json_createArray();
    if (add_item_to_object(object, name, array, &global_hooks, yangfalse))
    {
        return array;
    }

    yang_json_delete(array);
    return NULL;
}

YangJson * yang_json_detachItemViaPointer(YangJson *parent, YangJson * const item)
{
    if ((parent == NULL) || (item == NULL))
    {
        return NULL;
    }

    if (item != parent->child)
    {
       // not the first element
        item->prev->next = item->next;
    }
    if (item->next != NULL)
    {
       // not the last element
        item->next->prev = item->prev;
    }

    if (item == parent->child)
    {
       // first element
        parent->child = item->next;
    }
    else if (item->next == NULL)
    {
       // last element
        parent->child->prev = item->prev;
    }

   // make sure the detached item doesn't point anywhere anymore
    item->prev = NULL;
    item->next = NULL;

    return item;
}

YangJson * yang_json_detachItemFromArray(YangJson *array, int which)
{
    if (which < 0)
    {
        return NULL;
    }

    return yang_json_detachItemViaPointer(array, get_array_item(array, (size_t)which));
}

void yang_json_deleteItemFromArray(YangJson *array, int which)
{
    yang_json_delete(yang_json_detachItemFromArray(array, which));
}

YangJson * yang_json_detachItemFromObject(YangJson *object, const char *string)
{
    YangJson *to_detach = yang_json_getObjectItem(object, string);

    return yang_json_detachItemViaPointer(object, to_detach);
}

YangJson * yang_json_detachItemFromObjectCaseSensitive(YangJson *object, const char *string)
{
    YangJson *to_detach = yang_json_getObjectItemCaseSensitive(object, string);

    return yang_json_detachItemViaPointer(object, to_detach);
}

void yang_json_deleteItemFromObject(YangJson *object, const char *string)
{
    yang_json_delete(yang_json_detachItemFromObject(object, string));
}

void yang_json_deleteItemFromObjectCaseSensitive(YangJson *object, const char *string)
{
    yang_json_delete(yang_json_detachItemFromObjectCaseSensitive(object, string));
}

// Replace array/object items with new ones.
yangbool yang_json_insertItemInArray(YangJson *array, int which, YangJson *newitem)
{
    YangJson *after_inserted = NULL;

    if (which < 0)
    {
        return yangfalse;
    }

    after_inserted = get_array_item(array, (size_t)which);
    if (after_inserted == NULL)
    {
        return add_item_to_array(array, newitem);
    }

    newitem->next = after_inserted;
    newitem->prev = after_inserted->prev;
    after_inserted->prev = newitem;
    if (after_inserted == array->child)
    {
        array->child = newitem;
    }
    else
    {
        newitem->prev->next = newitem;
    }
    return yangtrue;
}

yangbool yang_json_replaceItemViaPointer(YangJson * const parent, YangJson * const item, YangJson * replacement)
{
    if ((parent == NULL) || (replacement == NULL) || (item == NULL))
    {
        return yangfalse;
    }

    if (replacement == item)
    {
        return yangtrue;
    }

    replacement->next = item->next;
    replacement->prev = item->prev;

    if (replacement->next != NULL)
    {
        replacement->next->prev = replacement;
    }
    if (parent->child == item)
    {
        if (parent->child->prev == parent->child)
        {
            replacement->prev = replacement;
        }
        parent->child = replacement;
    }
    else
    {  //
         // To find the last item in array quickly, we use prev in array.
        // We can't modify the last item's next pointer where this item was the parent's child

        if (replacement->prev != NULL)
        {
            replacement->prev->next = replacement;
        }
        if (replacement->next == NULL)
        {
            parent->child->prev = replacement;
        }
    }

    item->next = NULL;
    item->prev = NULL;
    yang_json_delete(item);

    return yangtrue;
}

yangbool yang_json_replaceItemInArray(YangJson *array, int which, YangJson *newitem)
{
    if (which < 0)
    {
        return yangfalse;
    }

    return yang_json_replaceItemViaPointer(array, get_array_item(array, (size_t)which), newitem);
}

static yangbool replace_item_in_object(YangJson *object, const char *string, YangJson *replacement, yangbool case_sensitive)
{
    if ((replacement == NULL) || (string == NULL))
    {
        return yangfalse;
    }

   // replace the name in the replacement
    if (!(replacement->type & Yang_JSON_StringIsConst) && (replacement->string != NULL))
    {
        yang_json_free(replacement->string);
    }
    replacement->string = (char*)yang_json_strdup((const uint8_t*)string, &global_hooks);
    replacement->type &= ~Yang_JSON_StringIsConst;

    return yang_json_replaceItemViaPointer(object, get_object_item(object, string, case_sensitive), replacement);
}

yangbool yang_json_replaceItemInObject(YangJson *object, const char *string, YangJson *newitem)
{
    return replace_item_in_object(object, string, newitem, yangfalse);
}

yangbool yang_json_replaceItemInObjectCaseSensitive(YangJson *object, const char *string, YangJson *newitem)
{
    return replace_item_in_object(object, string, newitem, yangtrue);
}

// Create basic types:
YangJson * yang_json_createNull(void)
{
    YangJson *item = yang_json_new_item(&global_hooks);
    if(item)
    {
        item->type = Yang_JSON_NULL;
    }

    return item;
}

YangJson * yang_json_createTrue(void)
{
    YangJson *item = yang_json_new_item(&global_hooks);
    if(item)
    {
        item->type = Yang_JSON_True;
    }

    return item;
}

YangJson * yang_json_createFalse(void)
{
    YangJson *item = yang_json_new_item(&global_hooks);
    if(item)
    {
        item->type = Yang_JSON_False;
    }

    return item;
}

YangJson * yang_json_createBool(yangbool boolean)
{
    YangJson *item = yang_json_new_item(&global_hooks);
    if(item)
    {
        item->type = boolean ? Yang_JSON_True : Yang_JSON_False;
    }

    return item;
}

YangJson * yang_json_createNumber(double num)
{
    YangJson *item = yang_json_new_item(&global_hooks);
    if(item)
    {
        item->type = Yang_JSON_Number;
        item->valuedouble = num;

       // use saturation in case of overflow
        if (num >= INT_MAX)
        {
            item->valueint = INT_MAX;
        }
        else if (num <= (double)INT_MIN)
        {
            item->valueint = INT_MIN;
        }
        else
        {
            item->valueint = (int)num;
        }
    }

    return item;
}

YangJson * yang_json_createString(const char *string)
{
    YangJson *item = yang_json_new_item(&global_hooks);
    if(item)
    {
        item->type = Yang_JSON_String;
        item->valuestring = (char*)yang_json_strdup((const uint8_t*)string, &global_hooks);
        if(!item->valuestring)
        {
            yang_json_delete(item);
            return NULL;
        }
    }

    return item;
}

YangJson * yang_json_createStringReference(const char *string)
{
    YangJson *item = yang_json_new_item(&global_hooks);
    if (item != NULL)
    {
        item->type = Yang_JSON_String | Yang_JSON_IsReference;
        item->valuestring = (char*)cast_away_const(string);
    }

    return item;
}

YangJson * yang_json_createObjectReference(const YangJson *child)
{
    YangJson *item = yang_json_new_item(&global_hooks);
    if (item != NULL) {
        item->type = Yang_JSON_Object | Yang_JSON_IsReference;
        item->child = (YangJson*)cast_away_const(child);
    }

    return item;
}

YangJson * yang_json_CreateArrayReference(const YangJson *child) {
    YangJson *item = yang_json_new_item(&global_hooks);
    if (item != NULL) {
        item->type = Yang_JSON_Array | Yang_JSON_IsReference;
        item->child = (YangJson*)cast_away_const(child);
    }

    return item;
}

YangJson * yang_json_createRaw(const char *raw)
{
    YangJson *item = yang_json_new_item(&global_hooks);
    if(item)
    {
        item->type = Yang_JSON_Raw;
        item->valuestring = (char*)yang_json_strdup((const uint8_t*)raw, &global_hooks);
        if(!item->valuestring)
        {
            yang_json_delete(item);
            return NULL;
        }
    }

    return item;
}

YangJson * yang_json_createArray(void)
{
    YangJson *item = yang_json_new_item(&global_hooks);
    if(item)
    {
        item->type=Yang_JSON_Array;
    }

    return item;
}

YangJson * yang_json_createObject(void)
{
    YangJson *item = yang_json_new_item(&global_hooks);
    if (item)
    {
        item->type = Yang_JSON_Object;
    }

    return item;
}

// Create Arrays:
YangJson * yang_json_createIntArray(const int *numbers, int count)
{
    size_t i = 0;
    YangJson *n = NULL;
    YangJson *p = NULL;
    YangJson *a = NULL;

    if ((count < 0) || (numbers == NULL))
    {
        return NULL;
    }

    a = yang_json_createArray();

    for(i = 0; a && (i < (size_t)count); i++)
    {
        n = yang_json_createNumber(numbers[i]);
        if (!n)
        {
            yang_json_delete(a);
            return NULL;
        }
        if(!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    if (a && a->child) {
        a->child->prev = n;
    }

    return a;
}

YangJson * yang_json_createFloatArray(const float *numbers, int count)
{
    size_t i = 0;
    YangJson *n = NULL;
    YangJson *p = NULL;
    YangJson *a = NULL;

    if ((count < 0) || (numbers == NULL))
    {
        return NULL;
    }

    a = yang_json_createArray();

    for(i = 0; a && (i < (size_t)count); i++)
    {
        n = yang_json_createNumber((double)numbers[i]);
        if(!n)
        {
            yang_json_delete(a);
            return NULL;
        }
        if(!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    if (a && a->child) {
        a->child->prev = n;
    }

    return a;
}

YangJson * yang_json_createDoubleArray(const double *numbers, int count)
{
    size_t i = 0;
    YangJson *n = NULL;
    YangJson *p = NULL;
    YangJson *a = NULL;

    if ((count < 0) || (numbers == NULL))
    {
        return NULL;
    }

    a = yang_json_createArray();

    for(i = 0; a && (i < (size_t)count); i++)
    {
        n = yang_json_createNumber(numbers[i]);
        if(!n)
        {
            yang_json_delete(a);
            return NULL;
        }
        if(!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    if (a && a->child) {
        a->child->prev = n;
    }

    return a;
}

YangJson * yang_json_createStringArray(const char *const *strings, int count)
{
    size_t i = 0;
    YangJson *n = NULL;
    YangJson *p = NULL;
    YangJson *a = NULL;

    if ((count < 0) || (strings == NULL))
    {
        return NULL;
    }

    a = yang_json_createArray();

    for (i = 0; a && (i < (size_t)count); i++)
    {
        n = yang_json_createString(strings[i]);
        if(!n)
        {
            yang_json_delete(a);
            return NULL;
        }
        if(!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p,n);
        }
        p = n;
    }

    if (a && a->child) {
        a->child->prev = n;
    }
    
    return a;
}

// Duplication
YangJson * yang_json_duplicate(const YangJson *item, yangbool recurse)
{
    YangJson *newitem = NULL;
    YangJson *child = NULL;
    YangJson *next = NULL;
    YangJson *newchild = NULL;

   // Bail on bad ptr
    if (!item)
    {
        goto fail;
    }
   // Create new item
    newitem = yang_json_new_item(&global_hooks);
    if (!newitem)
    {
        goto fail;
    }
   // Copy over all vars
    newitem->type = item->type & (~Yang_JSON_IsReference);
    newitem->valueint = item->valueint;
    newitem->valuedouble = item->valuedouble;
    if (item->valuestring)
    {
        newitem->valuestring = (char*)yang_json_strdup((uint8_t*)item->valuestring, &global_hooks);
        if (!newitem->valuestring)
        {
            goto fail;
        }
    }
    if (item->string)
    {
        newitem->string = (item->type&Yang_JSON_StringIsConst) ? item->string : (char*)yang_json_strdup((uint8_t*)item->string, &global_hooks);
        if (!newitem->string)
        {
            goto fail;
        }
    }
   // If non-recursive, then we're done!
    if (!recurse)
    {
        return newitem;
    }
   // Walk the ->next chain for the child.
    child = item->child;
    while (child != NULL)
    {
        newchild = yang_json_duplicate(child, yangtrue);// Duplicate (with recurse) each item in the ->next chain
        if (!newchild)
        {
            goto fail;
        }
        if (next != NULL)
        {
           // If newitem->child already set, then crosswire ->prev and ->next and move on
            next->next = newchild;
            newchild->prev = next;
            next = newchild;
        }
        else
        {
           // Set newitem->child and move to it
            newitem->child = newchild;
            next = newchild;
        }
        child = child->next;
    }
    if (newitem && newitem->child)
    {
        newitem->child->prev = newchild;
    }

    return newitem;

fail:
    if (newitem != NULL)
    {
        yang_json_delete(newitem);
    }

    return NULL;
}

static void skip_oneline_comment(char **input)
{
    *input += yang_static_strlen("//");

    for (; (*input)[0] != '\0'; ++(*input))
    {
        if ((*input)[0] == '\n') {
            *input += yang_static_strlen("\n");
            return;
        }
    }
}

static void skip_multiline_comment(char **input)
{
    *input += yang_static_strlen("/*");

    for (; (*input)[0] != '\0'; ++(*input))
    {
        if (((*input)[0] == '*') && ((*input)[1] == '/'))
        {
            *input += yang_static_strlen("*/");
            return;
        }
    }
}

static void minify_string(char **input, char **output) {
    (*output)[0] = (*input)[0];
    *input += yang_static_strlen("\"");
    *output += yang_static_strlen("\"");


    for (; (*input)[0] != '\0'; (void)++(*input), ++(*output)) {
        (*output)[0] = (*input)[0];

        if ((*input)[0] == '\"') {
            (*output)[0] = '\"';
            *input += yang_static_strlen("\"");
            *output += yang_static_strlen("\"");
            return;
        } else if (((*input)[0] == '\\') && ((*input)[1] == '\"')) {
            (*output)[1] = (*input)[1];
            *input += yang_static_strlen("\"");
            *output += yang_static_strlen("\"");
        }
    }
}

void yang_json_minify(char *json)
{
    char *into = json;

    if (json == NULL)
    {
        return;
    }

    while (json[0] != '\0')
    {
        switch (json[0])
        {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                json++;
                break;

            case '/':
                if (json[1] == '/')
                {
                    skip_oneline_comment(&json);
                }
                else if (json[1] == '*')
                {
                    skip_multiline_comment(&json);
                } else {
                    json++;
                }
                break;

            case '\"':
                minify_string(&json, (char**)&into);
                break;

            default:
                into[0] = json[0];
                json++;
                into++;
        }
    }

   // and null-terminate.
    *into = '\0';
}

yangbool yang_json_isInvalid(const YangJson * const item)
{
    if (item == NULL)
    {
        return yangfalse;
    }

    return (item->type & 0xFF) == Yang_JSON_Invalid;
}

yangbool yang_json_isFalse(const YangJson * const item)
{
    if (item == NULL)
    {
        return yangfalse;
    }

    return (item->type & 0xFF) == Yang_JSON_False;
}

yangbool yang_json_isTrue(const YangJson * const item)
{
    if (item == NULL)
    {
        return yangfalse;
    }

    return (item->type & 0xff) == Yang_JSON_True;
}


yangbool yang_json_isBool(const YangJson * const item)
{
    if (item == NULL)
    {
        return yangfalse;
    }

    return (item->type & (Yang_JSON_True | Yang_JSON_False)) != 0;
}
yangbool yang_json_isNull(const YangJson * const item)
{
    if (item == NULL)
    {
        return yangfalse;
    }

    return (item->type & 0xFF) == Yang_JSON_NULL;
}

yangbool yang_json_isNumber(const YangJson * const item)
{
    if (item == NULL)
    {
        return yangfalse;
    }

    return (item->type & 0xFF) == Yang_JSON_Number;
}

yangbool yang_json_isString(const YangJson * const item)
{
    if (item == NULL)
    {
        return yangfalse;
    }

    return (item->type & 0xFF) == Yang_JSON_String;
}

yangbool yang_json_isArray(const YangJson * const item)
{
    if (item == NULL)
    {
        return yangfalse;
    }

    return (item->type & 0xFF) == Yang_JSON_Array;
}

yangbool yang_json_isObject(const YangJson * const item)
{
    if (item == NULL)
    {
        return yangfalse;
    }

    return (item->type & 0xFF) == Yang_JSON_Object;
}

yangbool yang_json_isRaw(const YangJson * const item)
{
    if (item == NULL)
    {
        return yangfalse;
    }

    return (item->type & 0xFF) == Yang_JSON_Raw;
}

yangbool yang_json_compare(const YangJson * const a, const YangJson * const b, const yangbool case_sensitive)
{
    if ((a == NULL) || (b == NULL) || ((a->type & 0xFF) != (b->type & 0xFF)))
    {
        return yangfalse;
    }

   // check if type is valid
    switch (a->type & 0xFF)
    {
        case Yang_JSON_False:
        case Yang_JSON_True:
        case Yang_JSON_NULL:
        case Yang_JSON_Number:
        case Yang_JSON_String:
        case Yang_JSON_Raw:
        case Yang_JSON_Array:
        case Yang_JSON_Object:
            break;

        default:
            return yangfalse;
    }

   // identical objects are equal
    if (a == b)
    {
        return yangtrue;
    }

    switch (a->type & 0xFF)
    {
       // in these cases and equal type is enough
        case Yang_JSON_False:
        case Yang_JSON_True:
        case Yang_JSON_NULL:
            return yangtrue;

        case Yang_JSON_Number:
            if (compare_double(a->valuedouble, b->valuedouble))
            {
                return yangtrue;
            }
            return yangfalse;

        case Yang_JSON_String:
        case Yang_JSON_Raw:
            if ((a->valuestring == NULL) || (b->valuestring == NULL))
            {
                return yangfalse;
            }
            if (strcmp(a->valuestring, b->valuestring) == 0)
            {
                return yangtrue;
            }

            return yangfalse;

        case Yang_JSON_Array:
        {
            YangJson *a_element = a->child;
            YangJson *b_element = b->child;

            for (; (a_element != NULL) && (b_element != NULL);)
            {
                if (!yang_json_compare(a_element, b_element, case_sensitive))
                {
                    return yangfalse;
                }

                a_element = a_element->next;
                b_element = b_element->next;
            }

           // one of the arrays is longer than the other
            if (a_element != b_element) {
                return yangfalse;
            }

            return yangtrue;
        }

        case Yang_JSON_Object:
        {
            YangJson *a_element = NULL;
            YangJson *b_element = NULL;
            yang_json_arrayForEach(a_element, a)
            {
               // TODO This has O(n^2) runtime, which is horrible!
                b_element = get_object_item(b, a_element->string, case_sensitive);
                if (b_element == NULL)
                {
                    return yangfalse;
                }

                if (!yang_json_compare(a_element, b_element, case_sensitive))
                {
                    return yangfalse;
                }
            }

            //doing this twice, once on a and b to prevent yangtrue comparison if a subset of b
           // TODO: Do this the proper way, this is just a fix for now
            yang_json_arrayForEach(b_element, b)
            {
                a_element = get_object_item(a, b_element->string, case_sensitive);
                if (a_element == NULL)
                {
                    return yangfalse;
                }

                if (!yang_json_compare(b_element, a_element, case_sensitive))
                {
                    return yangfalse;
                }
            }

            return yangtrue;
        }

        default:
            return yangfalse;
    }
}

void * yang_json_malloc(size_t size)
{
    return global_hooks.allocate(size);
}

void yang_json_free(void *object)
{
    global_hooks.deallocate(object);
}
#endif
