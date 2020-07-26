/*
** Copyright (c) 2017  Hughes Technologies Pty Ltd. 
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "config.h"

#if defined(_WIN32)
#else
#include <unistd.h>
#endif

#include "httpd.h"
#include "httpd_priv.h"


/**************************************************************************
** GLOBAL VARIABLES
**************************************************************************/


/**************************************************************************
** PRIVATE ROUTINES
**************************************************************************/

static int scanCidr(val, result, length)
	char	*val;
	u_int	*result,
		*length;
{
	u_int	res, res1, res2, res3, res4, res5;
	char	*cp;

	cp = val;
	res1 = atoi(cp);
	cp = index(cp,'.');
	if (!cp)
		return(-1);
	cp++;
	res2 = atoi(cp);
	cp = index(cp,'.');
	if (!cp)
		return(-1);
	cp++;
	res3 = atoi(cp);
	cp = index(cp,'.');
	if (!cp)
		return(-1);
	cp++;
	res4 = atoi(cp);
	cp = index(cp,'/');
	if (!cp)
	{
		res5 = 32;
	}
	else
	{
		cp++;
		res5 = atoi(cp);
	}

	if (res1>255 || res2>255 || res3>255 || res4>255 || res5>32)
	{
		return(-1);
	}
	res = (res1 << 24) + (res2 << 16) + (res3 << 8) + res4;
	*result = res;
	*length = res5;
	return(0);
}


static int _isInCidrBlock(server, addr1, len1, addr2, len2)
	httpd	*server;
	int	addr1, len1,
		addr2, len2;
{
	int	count,
		mask;

	/* if (addr1 == 0 && len1 == 0)
	{
		return(1);
	}*/

	if(len2 < len1)
	{
		_httpd_writeErrorLog(server, NULL, LEVEL_ERROR, 
		    "IP Address must be more specific than network block");
		return(0);
	}

	mask = count = 0;
	while(count < len1)
	{
		mask = (mask << 1) + 1;
		count++;
	}
	mask = mask << (32 - len1);
	if ( (addr1 & mask) == (addr2 & mask))
	{
		return(1);
	}
	else
	{
		return(0);
	}
}


/**************************************************************************
** PUBLIC ROUTINES
**************************************************************************/

httpAcl *httpdAddAcl(server, acl, cidr, action)
	httpd	*server;
	httpAcl	*acl;
        char	*cidr;
	int	action;
{
	httpAcl	*cur;
	u_int	addr,
		len;

	/*
	** Check the ACL info is reasonable
	*/
	if(scanCidr(cidr, &addr, &len) < 0)
	{
		_httpd_writeErrorLog(server, NULL, LEVEL_ERROR, 
			"Invalid IP address format");
		return(NULL);
	}
	if (action != HTTP_ACL_PERMIT && action != HTTP_ACL_DENY)
	{
		_httpd_writeErrorLog(server, NULL, LEVEL_ERROR, 
			"Invalid acl action");
		return(NULL);
	}

	/*
	** Find a spot to put this ACE
	*/	
	if (acl)
	{
		cur = acl;
		while(cur->next)
		{
			cur = cur->next;
		}
		cur->next = (httpAcl*)malloc(sizeof(httpAcl));
		cur = cur->next;
	}
	else
	{
		cur = (httpAcl*)malloc(sizeof(httpAcl));
		acl = cur;
	}

	/*
	** Add the details and return
	*/
	cur->addr = addr;
	cur->len = len;
	cur->action = action;
	cur->next = NULL;
	return(acl);
}


int httpdCheckAcl(server, request, acl)
	httpd	*server;
	httpReq	*request;
	httpAcl	*acl;
{
	httpAcl	*cur;
	u_int	addr, len;
	int	res,
		action;


	action = HTTP_ACL_DENY;
	scanCidr(request->clientAddr, &addr, &len);
	cur = acl;
	while(cur)
	{
		res = _isInCidrBlock(server, cur->addr, cur->len, addr, len);
		if (res == 1)
		{
			action = cur->action;
			break;
		}
		cur = cur->next;
	}
	if (action == HTTP_ACL_DENY)
	{
		_httpd_send403(server, request);
		_httpd_writeErrorLog(server, request, LEVEL_ERROR, 
    			"Access denied by ACL");
	}
	return(action);
}


void httpdSetDefaultAcl(server, acl)
	httpd	*server;
	httpAcl	*acl;
{
	server->defaultAcl = acl;
}
