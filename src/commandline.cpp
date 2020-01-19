#include <cstdlib>

#define _GNU_SOURCE
#include <getopt.h>

#include "commandline.h"

Commandline::~Commandline (void)
{
    std::map<std::string, Option *>::iterator i;

    for (i = OptionsByName.begin (); i != OptionsByName.end (); i++)
	delete i->second;
}

Commandline::Option &
Commandline::option (const std::string &n)
{
    return *OptionsByName[n];
}

void
Commandline::AddOption (Option *op)
{
    OptionsByName[op->name] = op;
    OptionsBySop [op->shortop] = op;
}

void
Commandline::parse (int argc, char *argv[])
{
    struct option *lopts = new struct option[OptionsByName.size ()];
    std::string    sopts;
    std::map<std::string, Option *>::iterator i;
    int c, j, option_index;

    for (i = OptionsByName.begin (), j = 0; i != OptionsByName.end (); i++, j++) {
	lopts[j].name    = i->second->name.c_str ();
	lopts[j].has_arg = i->second->argument;
	lopts[j].flag    = 0;
	lopts[j].val     = i->second->shortop;

	if (i->second->shortop) {
	    sopts += i->second->shortop;
	    if (i->second->argument)
		sopts += ':';
	}
    }

    while ((c = getopt_long (argc, argv, sopts.c_str (), lopts, &option_index)) != -1) {
	if (c != '?') {
	    Option *o = c ? OptionsBySop[c] : OptionsByName[lopts[option_index].name];
	    o->value = o->argument ? optarg : "true";
	}
    }

    delete [] lopts;
}

void
Commandline::dump (std::ostream &dst) const
{
    using namespace std;
    map<string, Option *>::const_iterator i;
    for (i = OptionsByName.begin (); i != OptionsByName.end (); i++) {
	dst << "option \"" << i->second->name << "\" has value \"" << i->second->value << "\"" << endl;
    }
}

std::ostream &
operator<< (std::ostream &dst, const Commandline &cmdline)
{
    cmdline.dump (dst);
    return dst;
}

std::ostream &
operator<< (std::ostream &dst, const Commandline *cmdline)
{
    cmdline->dump (dst);
    return dst;
}

Commandline::Option::Option (const char *n, char sop, const char *defval):
    name (n),
    shortop (sop),
    argument (defval != 0),
    value (argument ? defval : "false")
{
}

Commandline::Option::~Option (void)
{
}

Commandline::Option::operator bool ()
{
    return ((value == "true") || (value == "1"));
}

Commandline::Option::operator int ()
{
    const char *startptr = value.c_str ();
    char *endptr;

    long int result = std::strtol (startptr, &endptr, 0);

    if (startptr == endptr)
	throw InvalidArg (startptr);

    return result;
}

Commandline::Option::operator double ()
{
    const char *startptr = value.c_str ();
    char *endptr;

    double result = std::strtod (startptr, &endptr);

    if (startptr == endptr)
	throw InvalidArg (startptr);

    return result;
}

Commandline::Option::operator std::string ()
{
    return value;
}
