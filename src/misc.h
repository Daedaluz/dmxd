#ifndef __DAEDALUS_MISC_H__
#define __DAEDALIS_MISC_H__

/* author: Tobias Assarsson
 * email: tux@kth.se
 * description:
 * 		some debug macros and various functions to get commandline
 * 		options/arguments.
 */

#ifdef DEBUG
#define WHERESTR "[%s:%s:%d] "
#define WHEREARG __FILE__, __func__, __LINE__
#define _debug1(...) do { dprintf(1, __VA_ARGS__); } while(0)
#define _debug(_fmt, ...) do { _debug1(WHERESTR _fmt "\n", WHEREARG, ##__VA_ARGS__); } while(0)
#define BUG(EXP) do { if(EXP) _debug("BUG: " #EXP "\n"); } while(0)
#else /* !DEBUG */
#define _debug1(...) do { } while(0)
#define _debug(_fmt, ...) do { } while(0)
#define BUG(EXP) do { } while(0)
#endif /* DEBUG */

void errdie(const char* msg, ...);
void die(const char* msg, ...);
const char* option_get(char* const* argv, const char* opt);
int option_present(char* const* argv, const char* opt);
void phex(const char* pref, unsigned char* data, size_t len, ...);
char** strsplit(const char* str, const char* delims, char** dataptr, size_t* out_n);
char* getstring(size_t size, const char* fmt, ...);

#endif /* __DAEDALUS_MISC_H__ */

