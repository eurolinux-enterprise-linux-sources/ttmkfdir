// Last updated on Jan 8, 2002 by Yu Shao <yshao@redhat.com>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdio>

#include "ttmkfdir.h"
#include "ttf.h"

using namespace std;

void
usage (const char *program)
{
    using namespace std;

    cerr << "This Program is (C) Joerg Pommnitz, 2000" << endl;
    cerr << "Usage: " << program << " [OPTION]" << endl;
    cerr << "-e, --encoding\t\tname of the encoding directory file, default is \"/usr/X11R6/lib/X11/fonts/encodings/encodings.dir\"" << endl;
    cerr << "-o, --output\t\tname of the destination file, default is \"fonts.scale\"" << endl;
    cerr << "-d, --font-dir\t\tname of the TrueType font directory, default is \".\"" << endl;
    cerr << "-f, --default-foundry\tname of the default font foundry, default is \"misc\"" << endl;
    cerr << "-m, --max-missing\tmax # of missing characters per encoding, default is 5" << endl;
    cerr << "-a, --max-missing-percentage \tmax percentage # of missing characters per encoding, default is 2" << endl;
    cerr << "-b, --font-backend \tX font backend selection FreeType(1), or XTT(2), default is 1" << endl;
    cerr << "-x, --additional-entries \tgenerate additional TTCaps or FreeType entries, default is 0" << endl;
    cerr << "-c, --completeness\tuse less strict completeness tests for encoding tables" << endl;
    cerr << "-p, --panose\t\tuse panose information" << endl;
    cerr << "-h, --help\t\tshow this help message" << endl;
}

void
ParseCommandline (int argc, char *argv[])
{
    cmdline::instance()->AddOption (new Commandline::Option ("encoding", 'e', "/usr/X11R6/lib/X11/fonts/encodings/encodings.dir"));
    cmdline::instance()->AddOption (new Commandline::Option ("output", 'o', "fonts.scale"));
    cmdline::instance()->AddOption (new Commandline::Option ("font-dir", 'd', "."));
    cmdline::instance()->AddOption (new Commandline::Option ("default-foundry", 'f', "misc"));
    cmdline::instance()->AddOption (new Commandline::Option ("max-missing", 'm', "5"));
    cmdline::instance()->AddOption (new Commandline::Option ("max-missing-percentage", 'a', "2"));
    cmdline::instance()->AddOption (new Commandline::Option ("additional-entries", 'x', "0"));
    cmdline::instance()->AddOption (new Commandline::Option ("font-backend", 'b', "1"));
    cmdline::instance()->AddOption (new Commandline::Option ("completeness", 'c'));
    cmdline::instance()->AddOption (new Commandline::Option ("debug-entries", 'g'));
    cmdline::instance()->AddOption (new Commandline::Option ("help", 'h'));
    cmdline::instance()->AddOption (new Commandline::Option ("panose", 'p'));

    cmdline::instance()->parse (argc, argv);
    // cerr << cmdline::instance();
}

int
main (int argc, char *argv[])
{
    ParseCommandline (argc, argv);
    vector<string> fontdir;

    if (cmdline::instance()->option ("help")) {
	usage (argv[0]);
	return 0;
    }
    
    dir::instance()->scan (cmdline::instance()->option ("font-dir"));
    
    for (directory::iterator i = dir::instance()->begin (); i != dir::instance()->end(); i++) {
	const ttf::Face face (dir::instance()->path() + '/' + *i);
	for (ttf::Face::xlfdcontainer::const_iterator j = face.xlfdnames.begin (); j != face.xlfdnames.end (); j++)
	    fontdir.push_back (*j);
    }

    string filename = cmdline::instance()->option ("output");
    FILE *output = (filename == "-") ? stdout : fopen (filename.c_str (), "w");

    if (!output) {
	cerr << "Could not open output file " << filename << endl;
	return 1;
    }

    fprintf (output, "%d\n", fontdir.size ());

    for (vector<string>::const_iterator i = fontdir.begin (); i != fontdir.end (); i++) {
	fprintf (output, "%s\n", i->c_str ());
    }

    fclose (output);
}
