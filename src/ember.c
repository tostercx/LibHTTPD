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

#include "config.h"

#if defined(_WIN32)
#else
#include <unistd.h>
#endif

#include "httpd.h"
#include "httpd_priv.h"


/*
** NOTE : The #ifdef includes the entire file!
**        If HAVE_EMBER is not defined then none of this code is compiled.
*/

#ifdef HAVE_EMBER

#include <ember.h>

/**************************************************************************
** GLOBAL VARIABLES
**************************************************************************/


/**************************************************************************
** PRIVATE ROUTINES
**************************************************************************/

int _httpd_executeEmber(server, data)
	httpd	*server;
	char	*data;
{
	ember	*script;

	script = eCreateScript();
	script->www.parseHtml = 0;
	script->www.outputHtml = 1;
	script->www.rawHTTP = 1;
	script->useRuntimeConfig = 1;
	script->stdoutFD = server->clientSock;
	eBufferSource(script, data);
	if (eParseScript(script) < 0)
	{
		fprintf(stdout,"Error at line %d of script '%s'.\n",
			eGetLineNum(script), eGetSourceName(script));
		fprintf(stdout,"Error is '%s'\n\n", eGetErrorMsg(script));
		return(-1);
	}
	if (eRunScript(script) < 0)
	{
		fprintf(stdout,"Runtime error at line %d of script '%s'.\n",
			eGetLineNum(script), eGetCurFileName(script));
		fprintf(stdout,"Error is '%s'\n\n", eGetErrorMsg(script));
	}
	eFreeScript(script);
	return(0);
}

/**************************************************************************
** PUBLIC ROUTINES
**************************************************************************/



#endif /* HAVE_EMBER*/
