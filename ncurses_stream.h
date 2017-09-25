#ifndef NCURSES_STREAM_H
#define NCURSES_STREAM_H

#include <ncurses.h>

class ncursesbuf: public std::streambuf {
public:
	virtual int overflow(int c) {
		printw("%c", c);
		return 1;
	}

public:
	ncursesbuf() = default;
};

class ncurses_stream: public std::ostream {
protected:
	ncursesbuf _tbuf;
	std::ostream& _src;
	std::streambuf* const _srcbuf;

public:
	ncurses_stream& operator=(const ncurses_stream&) = delete;
	ncurses_stream(const ncurses_stream& obj) = delete;
	
	ncurses_stream(std::ostream &o):
		 std::ostream(&_tbuf), _tbuf(), _src(o), _srcbuf(o.rdbuf())
	{
		o.rdbuf(rdbuf());
	}

	~ncurses_stream() {
		_src.rdbuf(_srcbuf);
	}
};

#endif // NCURSES_STREAM_H
