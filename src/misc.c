/* author: Tobias Assarsson
 * email: tux@kth.se
 * description:
 * 		implementations of various usefull functions like "die" and str_split.
 */
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include "misc.h"

/* errdie:
 * 		kills the program with a message.
 * @msg : message.
 */
void errdie(const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vprintf(msg, ap);
	va_end(ap);
	printf(": %s\n", strerror(errno));
	exit(1);
}

/* die:
 * 		kills the program with a message.
 * @msg : message.
 */
void die(const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vprintf(msg, ap);
	va_end(ap);
	printf("\n");
	exit(1);
}


/* option_get:
 * 		search for option opt in argv and return the value.
 * @argv : pointer array of options.
 * @opt : option to search for.
 * @return : option searched for, NULL if not found.
 */
const char* option_get(char* const* argv, const char* opt)
{
	for(; *argv; argv++)
		if(strcmp(*argv, opt) == 0)
			break;
	if(*argv == NULL)
		return NULL;
	return *(argv + 1);
}

/* option_present:
 * 		checks wether the option is present or not.
 * @argv : pointer array of options.
 * @opt : option to look for.
 * @return : non 0 if present, 0 if not found.
 */
int option_present(char* const* argv, const char* opt)
{
	for(; *argv; argv++)
		if(strcmp(*argv, opt) == 0)
			break;
	return (*argv) ? 1:0;
}

void phex(const char* pref, unsigned char* data, size_t len, ...)
{
	va_list ap;
	size_t i;
	va_start(ap, len);
	vprintf(pref, ap);
	va_end(ap);
	for(i = 0; i < len-1; i++) {
		printf("%.2X, ", data[i]);
	}
	printf("%.2X\n", data[i]);
}

/* strsplit:
 * 		splits an arbitrary string into tokens.
 * @str : string to be splitted.
 * @delims : delimiters to be used.
 * @dataptr : stores a ptr to "data" for user to free later on.
 * @out_n : can be NULL, used to store number of tokens created.
 * @return : a pointer to an NULL-terminated pointer array containing the tokens.
 */
char** strsplit(const char* str, const char* delims, char** dataptr, size_t* out_n)
{
	char* data = strdup(str);
	char* saveptr;
	char* ptr;
	char** retval = (char**)malloc(sizeof(char*) * 50);
	size_t i;
	size_t current_max = 50;
	*dataptr = data;
	
	if(retval == NULL || data == NULL)
		return NULL;
	ptr = strtok_r(data, delims, &saveptr);

	for(i = 0; ;){
		for(; i < current_max; i++){
				retval[i] = ptr;
				ptr = strtok_r(NULL, delims, &saveptr);
				if(ptr == NULL){
					retval[i+1] = NULL;
					if(out_n != NULL)
						*out_n = i+1;
					return retval;
				}
		}
		current_max += 50;
		char** newptr = (char**)realloc(retval, (sizeof(char*) * current_max));
		if(newptr == NULL) {
			free(data);
			free(retval);
			return NULL;
		}
		retval = newptr;
	}
}

/* getstring:
 * 		creates a string from format.
 * @size : maximum size of string
 * @fmt : format string
 * @return : new formatted string
 */
char* getstring(size_t size, const char* fmt, ...)
{
	char* str = (char*)malloc(size);
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(str, size, fmt, ap);
	va_end(ap);
	return str;
}

