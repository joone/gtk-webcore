#ifndef GLibHelpers_h
#define GLibHelpers_h

inline
bool isEmptyString(const gchar* str)
{
    return str==0 || *str=='\0';
}

inline
void assignToString(gchar** dest, const gchar* source)
{
    assert(dest);
    if (*dest == source) return;
    if (*dest) g_free(*dest);
    if (source)
	*dest = g_strdup(source);
    else 
	*dest = 0; // or should it be ""?
}

inline
void freeString(gchar * string) {
    if (string) g_free(string);
}


#endif
