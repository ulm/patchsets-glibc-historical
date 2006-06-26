#ifndef	_NETDB_H
#include <glibc-compat/include/rpc/netdb.h>
#include <resolv/netdb.h>

/* Macros for accessing h_errno from inside libc.  */
# ifdef _LIBC_REENTRANT
#  include <tls.h>
#  if USE___THREAD
#   undef  h_errno
#   ifndef NOT_IN_libc
#    define h_errno __libc_h_errno
#   else
#    define h_errno h_errno	/* For #ifndef h_errno tests.  */
#   endif
extern __thread int h_errno attribute_tls_model_ie;
#   define __set_h_errno(x)	(h_errno = (x))
#  else
static inline int
__set_h_errno (int __err)
{
  return *__h_errno_location () = __err;
}
#  endif
# else
#  undef  h_errno
#  define __set_h_errno(x) (h_errno = (x))
extern int h_errno;
# endif	/* _LIBC_REENTRANT */

/* Document internal interfaces.  */
extern int __gethostent_r (struct hostent *__restrict __result_buf,
			   char *__restrict __buf, size_t __buflen,
			   struct hostent **__restrict __result,
			   int *__restrict __h_errnop);
extern int __old_gethostent_r (struct hostent *__restrict __result_buf,
			       char *__restrict __buf, size_t __buflen,
			       struct hostent **__restrict __result,
			       int *__restrict __h_errnop);

extern int __gethostbyaddr_r (__const void *__restrict __addr,
			      socklen_t __len, int __type,
			      struct hostent *__restrict __result_buf,
			      char *__restrict __buf, size_t __buflen,
			      struct hostent **__restrict __result,
			      int *__restrict __h_errnop);
extern int __old_gethostbyaddr_r (__const void *__restrict __addr,
				  socklen_t __len, int __type,
				  struct hostent *__restrict __result_buf,
				  char *__restrict __buf, size_t __buflen,
				  struct hostent **__restrict __result,
				  int *__restrict __h_errnop);

extern int __gethostbyname_r (__const char *__restrict __name,
			      struct hostent *__restrict __result_buf,
			      char *__restrict __buf, size_t __buflen,
			      struct hostent **__restrict __result,
			      int *__restrict __h_errnop);
extern int __old_gethostbyname_r (__const char *__restrict __name,
				  struct hostent *__restrict __result_buf,
				  char *__restrict __buf, size_t __buflen,
				  struct hostent **__restrict __result,
				  int *__restrict __h_errnop);

extern int __gethostbyname2_r (__const char *__restrict __name, int __af,
			       struct hostent *__restrict __result_buf,
			       char *__restrict __buf, size_t __buflen,
			       struct hostent **__restrict __result,
			       int *__restrict __h_errnop);
extern int __old_gethostbyname2_r (__const char *__restrict __name, int __af,
				   struct hostent *__restrict __result_buf,
				   char *__restrict __buf, size_t __buflen,
				   struct hostent **__restrict __result,
				   int *__restrict __h_errnop);

extern int __getnetent_r (struct netent *__restrict __result_buf,
			  char *__restrict __buf, size_t __buflen,
			  struct netent **__restrict __result,
			  int *__restrict __h_errnop);
extern int __old_getnetent_r (struct netent *__restrict __result_buf,
			      char *__restrict __buf, size_t __buflen,
			      struct netent **__restrict __result,
			      int *__restrict __h_errnop);

extern int __getnetbyaddr_r (uint32_t __net, int __type,
			     struct netent *__restrict __result_buf,
			     char *__restrict __buf, size_t __buflen,
			     struct netent **__restrict __result,
			     int *__restrict __h_errnop);
extern int __old_getnetbyaddr_r (uint32_t __net, int __type,
				 struct netent *__restrict __result_buf,
				 char *__restrict __buf, size_t __buflen,
				 struct netent **__restrict __result,
				 int *__restrict __h_errnop);

extern int __getnetbyname_r (__const char *__restrict __name,
			     struct netent *__restrict __result_buf,
			     char *__restrict __buf, size_t __buflen,
			     struct netent **__restrict __result,
			     int *__restrict __h_errnop);
extern int __old_getnetbyname_r (__const char *__restrict __name,
				 struct netent *__restrict __result_buf,
				 char *__restrict __buf, size_t __buflen,
				 struct netent **__restrict __result,
				 int *__restrict __h_errnop);

extern int __getservent_r (struct servent *__restrict __result_buf,
			   char *__restrict __buf, size_t __buflen,
			   struct servent **__restrict __result);
extern int __old_getservent_r (struct servent *__restrict __result_buf,
			       char *__restrict __buf, size_t __buflen,
			       struct servent **__restrict __result);

extern int __getservbyname_r (__const char *__restrict __name,
			      __const char *__restrict __proto,
			      struct servent *__restrict __result_buf,
			      char *__restrict __buf, size_t __buflen,
			      struct servent **__restrict __result);
extern int __old_getservbyname_r (__const char *__restrict __name,
				  __const char *__restrict __proto,
				  struct servent *__restrict __result_buf,
				  char *__restrict __buf, size_t __buflen,
				  struct servent **__restrict __result);

extern int __getservbyport_r (int __port,
			      __const char *__restrict __proto,
			      struct servent *__restrict __result_buf,
			      char *__restrict __buf, size_t __buflen,
			      struct servent **__restrict __result);
extern int __old_getservbyport_r (int __port,
				  __const char *__restrict __proto,
				  struct servent *__restrict __result_buf,
				  char *__restrict __buf, size_t __buflen,
				  struct servent **__restrict __result);

extern int __getprotoent_r (struct protoent *__restrict __result_buf,
			    char *__restrict __buf, size_t __buflen,
			    struct protoent **__restrict __result);
extern int __old_getprotoent_r (struct protoent *__restrict __result_buf,
				char *__restrict __buf, size_t __buflen,
				struct protoent **__restrict __result);

extern int __getprotobyname_r (__const char *__restrict __name,
			       struct protoent *__restrict __result_buf,
			       char *__restrict __buf, size_t __buflen,
			       struct protoent **__restrict __result);
extern int __old_getprotobyname_r (__const char *__restrict __name,
				   struct protoent *__restrict __result_buf,
				   char *__restrict __buf, size_t __buflen,
				   struct protoent **__restrict __result);

extern int __getprotobynumber_r (int __proto,
				 struct protoent *__restrict __res_buf,
				 char *__restrict __buf, size_t __buflen,
				 struct protoent **__restrict __result);
extern int __old_getprotobynumber_r (int __proto,
				     struct protoent *__restrict __res_buf,
				     char *__restrict __buf, size_t __buflen,
				     struct protoent **__restrict __result);

extern int __getnetgrent_r (char **__restrict __hostp,
			    char **__restrict __userp,
			    char **__restrict __domainp,
			    char *__restrict __buffer, size_t __buflen);

extern int ruserpass (const char *host, const char **aname,
		      const char **apass);


/* The following declarations and definitions have been removed from
   the public header since we don't want people to use them.  */

#define AI_V4MAPPED	0x0008	/* IPv4-mapped addresses are acceptable.  */
#define AI_ALL		0x0010	/* Return both IPv4 and IPv6 addresses.	 */
#define AI_ADDRCONFIG	0x0020	/* Use configuration of this host to choose
				  returned address type.  */
#define AI_DEFAULT    (AI_V4MAPPED | AI_ADDRCONFIG)

#endif /* !_NETDB_H */
