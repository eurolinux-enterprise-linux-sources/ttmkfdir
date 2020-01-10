#include <cctype>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "directory.h"

directory::~directory (void)
{
}

void
directory::scan (const std::string &dir)
{
    DIR *ttfdir = opendir(dir.c_str ());

    this->clear ();
    dirpath = dir;
    dirpath += '/';
    
    while (struct dirent *entry = readdir (ttfdir)) {
	std::string name = path () + entry->d_name;
	if (this->select (name.c_str ())) {
	    this->push_back (entry->d_name);
	}
    }

    closedir (ttfdir);
}

bool
directory::select (const char *name) const
{
    return true;
}

bool
ttfdirectory::select (const char *name) const
{
    int len;
    struct stat buf;

    /* must be a regular file */
    if (::stat (name, &buf) || !(S_ISREG (buf.st_mode))) {
	return false;
    }
    
    /* we make the decision by the extension of the file name */
    return (((len = strlen (name)) > 4) &&
	    (name[len - 4] == '.') &&
	    (std::toupper(name[len - 3]) == 'T') &&
	    (std::toupper(name[len - 2]) == 'T') &&
	    ((std::toupper(name[len - 1]) == 'F') ||
	     (std::toupper(name[len - 1]) == 'C')));
}
