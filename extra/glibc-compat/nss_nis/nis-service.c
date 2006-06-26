/* Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Thorsten Kukuk <kukuk@vt.uni-paderborn.de>, 1996.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <nss.h>
#include <glibc-compat/include/netdb.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <bits/libc-lock.h>
#include <rpcsvc/yp.h>
#include <rpcsvc/ypclnt.h>

#include "nss-nis.h"

/* Get the declaration of the parser function.  */
#define ENTNAME servent
#define EXTERN_PARSER
#include "../nss_files/files-parse.c"

__libc_lock_define_initialized (static, lock)

struct response_t
{
  char *val;
  struct response_t *next;
};

struct intern_t
{
  struct response_t *start;
  struct response_t *next;
};
typedef struct intern_t intern_t;

static intern_t intern = { NULL, NULL };

static int
saveit (int instatus, char *inkey, int inkeylen, char *inval,
        int invallen, char *indata)
{
  intern_t *intern = (intern_t *) indata;

  if (instatus != YP_TRUE)
    return instatus;

  if (inkey && inkeylen > 0 && inval && invallen > 0)
    {
      if (intern->start == NULL)
        {
          intern->start = malloc (sizeof (struct response_t));
          intern->next = intern->start;
        }
      else
        {
          intern->next->next = malloc (sizeof (struct response_t));
          intern->next = intern->next->next;
        }
      intern->next->next = NULL;
      intern->next->val = malloc (invallen + 1);
      strncpy (intern->next->val, inval, invallen);
      intern->next->val[invallen] = '\0';
    }

  return 0;
}

static enum nss_status
internal_nis_setservent (intern_t *intern)
{
  char *domainname;
  struct ypall_callback ypcb;
  enum nss_status status;

  if (yp_get_default_domain (&domainname))
    return NSS_STATUS_UNAVAIL;

  while (intern->start != NULL)
    {
      if (intern->start->val != NULL)
        free (intern->start->val);
      intern->next = intern->start;
      intern->start = intern->start->next;
      free (intern->next);
    }
  intern->start = NULL;

  ypcb.foreach = saveit;
  ypcb.data = (char *) intern;
  status = yperr2nss (yp_all (domainname, "services.byname", &ypcb));
  intern->next = intern->start;

  return status;
}
enum nss_status
_nss_nis_setservent (void)
{
  enum nss_status status;

  __libc_lock_lock (lock);

  status = internal_nis_setservent (&intern);

  __libc_lock_unlock (lock);

  return status;
}

static enum nss_status
internal_nis_endservent (intern_t * intern)
{
  while (intern->start != NULL)
    {
      if (intern->start->val != NULL)
        free (intern->start->val);
      intern->next = intern->start;
      intern->start = intern->start->next;
      free (intern->next);
    }
  intern->start = NULL;

  return NSS_STATUS_SUCCESS;
}

enum nss_status
_nss_nis_endservent (void)
{
  enum nss_status status;

  __libc_lock_lock (lock);

  status = internal_nis_endservent (&intern);

  __libc_lock_unlock (lock);

  return status;
}

static enum nss_status
internal_nis_getservent_r (struct servent *serv, char *buffer,
			   size_t buflen, intern_t *data)
{
  struct parser_data *pdata = (void *) buffer;
  int parse_res;
  char *p;

  if (data->start == NULL)
    internal_nis_setservent (data);

  /* Get the next entry until we found a correct one. */
  do
    {
      if (data->next == NULL)
	return NSS_STATUS_NOTFOUND;
      p = strncpy (buffer, data->next->val, buflen);
      data->next = data->next->next;
      while (isspace (*p))
        ++p;

      parse_res = _nss_files_parse_servent (p, serv, pdata, buflen);
      if (parse_res == -1 && errno == ERANGE)
        return NSS_STATUS_TRYAGAIN;
    }
  while (!parse_res);

  return NSS_STATUS_SUCCESS;
}

enum nss_status
_nss_nis_getservent_r (struct servent *serv, char *buffer, size_t buflen)
{
  enum nss_status status;

  __libc_lock_lock (lock);

  status = internal_nis_getservent_r (serv, buffer, buflen, &intern);

  __libc_lock_unlock (lock);

  return status;
}

enum nss_status
_nss_nis_getservbyname_r (const char *name, char *protocol,
			  struct servent *serv, char *buffer, size_t buflen)
{
  intern_t data = { NULL, NULL };
  enum nss_status status;
  int found;

  if (name == NULL)
    {
      __set_errno (EINVAL);
      return NSS_STATUS_UNAVAIL;
    }

  status = internal_nis_setservent (&data);
  if (status != NSS_STATUS_SUCCESS)
    return status;

  found = 0;
  while (!found &&
         ((status = internal_nis_getservent_r (serv, buffer, buflen, &data))
          == NSS_STATUS_SUCCESS))
    {
      if (protocol == NULL || strcmp (serv->s_proto, protocol) == 0)
	{
	  char **cp;

	  if (strcmp (serv->s_name, name) == 0)
	    found = 1;
	  else
	    for (cp = serv->s_aliases; *cp; cp++)
	      if (strcmp (name, *cp) == 0)
		found = 1;
	}
    }

  internal_nis_endservent (&data);

  if (!found && status == NSS_STATUS_SUCCESS)
    return NSS_STATUS_NOTFOUND;
  else
    return status;
}

enum nss_status
_nss_nis_getservbyport_r (int port, char *protocol, struct servent *serv,
			  char *buffer, size_t buflen)
{
  intern_t data = { NULL, NULL };
  enum nss_status status;
  int found;

  if (protocol == NULL)
    {
      __set_errno (EINVAL);
      return NSS_STATUS_UNAVAIL;
    }

  status = internal_nis_setservent (&data);
  if (status != NSS_STATUS_SUCCESS)
    return status;

  found = 0;
  while (!found &&
         ((status = internal_nis_getservent_r (serv, buffer, buflen, &data))
          == NSS_STATUS_SUCCESS))
    {
      if (htons (serv->s_port) == port)
        {
          if  (strcmp (serv->s_proto, protocol) == 0)
            {
              found = 1;
            }
        }
    }

  internal_nis_endservent (&data);

  if (!found && status == NSS_STATUS_SUCCESS)
    return NSS_STATUS_NOTFOUND;
  else
    return status;
}
