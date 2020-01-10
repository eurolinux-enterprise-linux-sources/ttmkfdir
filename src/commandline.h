/* -*- C++ -*-  */
#ifndef COMMANDLINE_H__
#define COMMANDLINE_H__

#include <string>
#include <map>
#include <iostream>
#include "util.h"
using namespace std;

class Commandline {
    friend ostream &operator<< (ostream &, const Commandline &);
    friend ostream &operator<< (ostream &, const Commandline *);
public:
    class Option {
	friend class Commandline;
    public:
	class InvalidArg {
	public:
	    InvalidArg (const char *arg) : option (arg) {};
	    ~InvalidArg (void) {};
	    std::string option;
	};

	Option (const char *n, char sop, const char *defval = 0);
	~Option (void);
	operator bool ();
	operator int ();
	operator double ();
	operator std::string ();
    protected:
	std::string name;
	char shortop;
	bool argument;
	std::string value;
    };
    
    virtual ~Commandline (void);
    Option &option (const std::string &n);
    void AddOption (Option *);
    void parse (int argc, char *argv[]);
protected:
    std::map<std::string, Option *> OptionsByName;
    std::map<int,         Option *> OptionsBySop;
    void dump (std::ostream &dst) const;
};

#endif /* COMMANDLINE_H__ */
