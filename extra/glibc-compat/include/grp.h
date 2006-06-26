#ifndef _GRP_H
#include <grp/grp.h>

/* Now define the internal interfaces.  */
extern int __getgrent_r (struct group *__resultbuf, char *buffer,
			 size_t __buflen, struct group **__result);
extern int __old_getgrent_r (struct group *__resultbuf, char *buffer,
			     size_t __buflen, struct group **__result);
extern int __fgetgrent_r (FILE * __stream, struct group *__resultbuf,
			  char *buffer, size_t __buflen,
			  struct group **__result);

/* Search for an entry with a matching group ID.  */
extern int __getgrgid_r (__gid_t __gid, struct group *__resultbuf,
			 char *__buffer, size_t __buflen,
			 struct group **__result);
extern int __old_getgrgid_r (__gid_t __gid, struct group *__resultbuf,
			     char *__buffer, size_t __buflen,
			     struct group **__result);

/* Search for an entry with a matching group name.  */
extern int __getgrnam_r (__const char *__name, struct group *__resultbuf,
			 char *__buffer, size_t __buflen,
			 struct group **__result);
extern int __old_getgrnam_r (__const char *__name, struct group *__resultbuf,
			     char *__buffer, size_t __buflen,
			     struct group **__result);

#endif
