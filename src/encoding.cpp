#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "freetype/freetype.h"

#include "ttmkfdir.h"
#include "encoding.h"

using namespace std;

namespace {
    const char *
    EncodingBase (void)
    {
	static char base[PATH_MAX] = {0,};

	if (!base[0]) {
	    strncpy (base, string(cmdline::instance()->option ("encoding")).c_str (), sizeof (base));
	    char *p = strrchr (base, '/');

	    if (p)
		*(p + 1) = 0;
	    else 
		base[0] = 0;
	}

	return base;
    }

    FILE *
    NextFile (FILE *f, char *name)
    {
	char file_name [1024];
	char command[1024];

	if (fscanf (f, "%*s %[^\n]\n", file_name) == 1) {

	    if (file_name[0] == '/') {
		name[0] = 0;
	    } else {
		strcpy (name, EncodingBase ());
	    }
	
	    strcat (name, file_name);

	    sprintf (command, "exec %s < %s", (toupper(name[strlen (name) - 1]) == 'Z')
		     ? "gzip -d" : "cat", name);
	    return popen (command, "r");
	}

	return 0;
    }
}

void
Encodings_t::ReadEncodings (void)
{
    int n;
    FILE *input;
    char name[PATH_MAX];

    FILE *f = fopen (string(cmdline::instance()->option ("encoding")).c_str (), "r");

    if (!f)
	return;
    
    /* read and ignore leading number */
    fscanf(f, "%d\n", &n);
    
    while ((input = NextFile (f, name)) != 0) {
	extern int yylex (char *, Encodings_t &);
	extern void yyrestart (FILE*);
	
	yyrestart (input);
	yylex (name, *this);

	pclose (input);
    }

    fclose (f);
}

Encodings_t::Encodings_t (void)
{
    ReadEncodings ();

    for (int i = 0; Encoding::builtin_encodings[i].size; i++)
	new Encoding (this, Encoding::builtin_encodings + i);
}

Encoding::Encoding (Encodings_t *dest, BuiltinEncoding_t *b):
    size (b->size), enc_size (b->enc_size), start_code (b->start_code)
{
    for (int i = 0; b->aliases[i]; i++)
	names.push_back (b->aliases[i]);

    NumericMapping *m = new NumericMapping (size, b->mapdata.platform, b->mapdata.encoding);

    for (int i = 0; i < size; i++)
	(*m)[i] = b->mapdata.mappingtable[i];

    AddMapping (m);
    dest->insert (std::make_pair(m->cmapkey(), this));;
}
