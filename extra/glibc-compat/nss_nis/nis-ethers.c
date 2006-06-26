/* Copyright (C) 1996, 1997 Free Software Foundation, Inc.
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
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <bits/libc-lock.h>
#include <rpcsvc/yp.h>
#include <rpcsvc/ypclnt.h>
#include <netinet/if_ether.h>

#include "nss-nis.h"

/* Protect global state against multiple changers */
__libc_lock_define_initialized (static, lock)

struct ether
{
  const char *e_name;
  struct ether_addr e_addr;
};

/* Get the declaration of the parser function.  */
#define ENTNAME etherent
#define STRUCTURE ether
#define EXTERN_PARSER
#include "../nss_files/files-parse.c"

struct response
{
  char *val;
  struct response *next;
};

static struct response *start = NULL;
static struct response *next = NULL;

static int
saveit (int instatus, char *inkey, int inkeylen, char *inval,
	int invallen, char *indata)
{
  if (instatus != YP_TRUE)
    return instatus;

  if (inkey && inkeylen > 0 && inval && invallen > 0)
    {
      if (start == NULL)
	{
	  start = malloc (sizeof (struct response));
	  next = start;
	}
      else
	{
	  next->next = malloc (sizeof (struct response));
	  next = next->next;
	}
      next->next = NULL;
      next->val = malloc (invallen + 1);
      strncpy (next->val, inval, invallen);
      next->val[invallen] = '\0';
    }

  return 0;
}

enum nss_status
internal_nis_setetherent (void)
{
  char *domainname;
  struct ypall_callback ypcb;
  enum nss_status status;

  yp_get_default_domain (&domainname);

  while (start != NULL)
    {
      if (start->val != NULL)
	free (start->val);
      next = start;
      start = start->next;
      free (next);
    }
  start = NULL;

  ypcb.foreach = saveit;
  ypcb.data = NULL;
  status = yperr2nss (yp_all (domainname, "ethers.byname", &ypcb));
  next = start;

  return status;
}

enum nss_status
_nss_nis_setetherent (void)
{
  enum nss_status result;

  __libc_lock_lock (lock);

  result = internal_nis_setetherent ();

  __libc_lock_unlock (lock);

  return result;
}

enum nss_status
_nss_nis_endetherent (void)
{
  __libc_lock_lock (lock);

  while (start != NULL)
    {
      if (start->val != NULL)
	free (start->val);
      next = start;
      start = start->next;
      free (next);
    }
  start = NULL;
  next = NULL;

  __libc_lock_unlock (lock);

  return NSS_STATUS_SUCCESS;
}

static enum nss_status
internal_nis_getetherent_r (struct ether *eth, char *buffer, size_t buflen)
{
  struct parser_data *data = (void *) buffer;
  int parse_res;

  if (start == NULL)
    internal_nis_setetherent ();

  /* Get the next entry until we found a correct one. */
  do
    {
      char *p;

      if (next == NULL)
	return NSS_STATUS_NOTFOUND;
      p = strncpy (buffer, next->val, buflen);
      next = next->next;

      while (isspace (*p))
        ++p;

      parse_res = _nss_files_parse_etherent (p, eth, data, buflen);
      if (parse_res == -1 && errno == ERANGE)
        return NSS_STATUS_TRYAGAIN;
    }
  while (!parse_res);

  return NSS_STATUS_SUCCESS;
}

enum nss_status
_nss_nis_getetherent_r (struct ether *result, char *buffer, size_t buflen)
{
  int status;

  __libc_lock_lock (lock);

  status = internal_nis_getetherent_r (result, buffer, buflen);

  __libc_lock_unlock (lock);

  return status;
}

enum nss_status
_nss_nis_gethostton_r (const char *name, struct ether *eth,
		       char *buffer, size_t buflen)
{
  struct parser_data *data = (void *) buffer;
  enum nss_status retval;
  char *domain, *result, *p;
  int len, parse_res;

  if (name == NULL)
    {
      __set_errno (EINVAL);
      return NSS_STATUS_UNAVAIL;
    }

  if (yp_get_default_domain (&domain))
    return NSS_STATUS_UNAVAIL;

  retval = yperr2nss (yp_match (domain, "ethers.byname", name,
				strlen (name), &result, &len));

  if (retval != NSS_STATUS_SUCCESS)
    {
      if (retval == NSS_STATUS_TRYAGAIN)
        __set_errno (EAGAIN);
      return retval;
    }

  if ((size_t) (len + 1) > buflen)
    {
      free (result);
      __set_errno (ERANGE);
      return NSS_STATUS_TRYAGAIN;
    }

  p = strncpy (buffer, result, len);
  buffer[len] = '\0';
  while (isspace (*p))
    ++p;
  free (result);

  parse_res = _nss_files_parse_etherent (p, eth, data, buflen);

  if (parse_res == -1 && errno == ERANGE)
    return NSS_STATUS_TRYAGAIN;
  else if (parse_res == 0)
    return NSS_STATUS_NOTFOUND;

  return NSS_STATUS_SUCCESS;
}

enum nss_status
_nss_nis_getntohost_r (struct ether_addr *addr, struct ether *eth,
		       char *buffer, size_t buflen)
{
  struct parser_data *data = (void *) buffer;
  enum nss_status retval;
  char *domain, *result, *p;
  int len, nlen, parse_res;
  char buf[33];

  if (addr == NULL)
    {
      __set_errno (EINVAL);
      return NSS_STATUS_UNAVAIL;
    }

  if (yp_get_default_domain (&domain))
    return NSS_STATUS_UNAVAIL;

  nlen = sprintf (buf, "%x:%x:%x:%x:%x:%x",
		  (int) addr->ether_addr_octet[0],
		  (int) addr->ether_addr_octet[1],
		  (int) addr->ether_addr_octet[2],
		  (int) addr->ether_addr_octet[3],
		  (int) addr->ether_addr_octet[4],
		  (int) addr->ether_addr_octet[5]);

  retval = yperr2nss (yp_match (domain, "ethers.byaddr", buf,
				nlen, &result, &len));

  if (retval != NSS_STATUS_SUCCESS)
    {
      if (retval == NSS_STATUS_TRYAGAIN)
        __set_errno (EAGAIN);
      return retval;
    }

  if ((size_t) (len + 1) > buflen)
    {
      free (result);
      __set_errno (ERANGE);
      return NSS_STATUS_TRYAGAIN;
    }

  p = strncpy (buffer, result, len);
  buffer[len] = '\0';
  while (isspace (*p))
    ++p;
  free (result);

  parse_res = _nss_files_parse_etherent (p, eth, data, buflen);

  if (parse_res == -1 && errno == ERANGE)
    return NSS_STATUS_TRYAGAIN;
  else if (parse_res == 0)
    return NSS_STATUS_NOTFOUND;

  return NSS_STATUS_SUCCESS;
}
