#include "KWIQMacros.h"

#if KWIQ && !NDEBUG

#include <glib.h>
void report_g_conv_error(const char* file,int line, GError* err)
{
    if (err != NULL){
	char* errstr="";
	switch(err->code){
	case G_CONVERT_ERROR_NO_CONVERSION:
	    errstr="no conversion";break;
	case G_CONVERT_ERROR_ILLEGAL_SEQUENCE:
	    errstr="illegal seqence";break;
	case G_CONVERT_ERROR_FAILED:
	    errstr="failed";break;
	case  G_CONVERT_ERROR_PARTIAL_INPUT:
	    errstr="paritial input";break;
	case G_CONVERT_ERROR_BAD_URI:
	    errstr="bad uri";break;
	case G_CONVERT_ERROR_NOT_ABSOLUTE_PATH:
	    errstr="not absolute path";break;
	default:	   
 	    errstr="unknown error";
	    break;
	}
	g_printerr("%s:%d: Text conversion failed: %s, (code %d)\n",file,line, errstr,err->code);
	g_printerr("\t error message was: %s",err->message);
    }
}

#endif
