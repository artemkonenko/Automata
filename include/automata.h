#ifndef __AUTOMATA__
#define __AUTOMATA__

#include<vector>
#include<map>
#include<unordered_map>
#include<set>
#include<bitset>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<queue>
#include<cmath>

class automata
{
public:
	static const int START = 1;
	static const int cinAL = 2;
	virtual bool run( std::string input ) = 0;
	virtual void print_table() = 0;
	virtual int count_of_occurrences(std::string input) = 0;
};

class DFA: public automata
{
public:
	// таблица состоит из пар обозначение вершины и отображение символов в состояние
	typedef std::vector<std::pair<int, std::map<char, int> > > DFAtable;
protected:
	DFAtable _table;
	int _start;
public:
	DFA( DFAtable table ):_table(table)
	{}

	DFA( std::istream& in )
	{
		int nchars, nstates;
		in >> nchars >> nstates;
		
		std::string str;
		getline(in, str);

		_table.resize(nstates);
		std::vector<char> chars(nchars);
		for ( int c = 0; c < nchars; ++c )
			in >> chars[c];

		for ( int s = 0; s < nstates; ++s )
		{
			for ( int c = 0; c < nchars; ++c )
				in >> _table[s].second[chars[c]];
		}

		int nfstate, fstate;

		in >> _start;
		_table[_start].first |= START;

		in >> nfstate;
		for ( int i=0; i < nfstate; ++i )
		{
			in >> fstate;
			_table[fstate].first |= cinAL;
		}
	}

	void print_table();

	bool run( std::string input );

	/*
		Для автомата, который распознает слово <=> нужное подслова это суффикс данного слова. Считаем число достижений финального состояния,
		т.е. число таких префиксов данного слова, что его суффикс это искомое подслово.
	*/
	int count_of_occurrences(std::string input);
};

class NFA: public automata
{
public:
	// таблица состоит из пар обозначение вершины и отображение символов в состояния
	typedef std::vector<std::pair<int, std::multimap<char, int> > > NFAtable;
protected:
	NFAtable _table;
	int _start;
	std::vector<char> _chars;
public:
	NFA( NFAtable table ):_table(table)
	{}

	NFA()
	{}

	NFA( std::istream& in )
	{
		int nchars, nstates;
		in >> nchars >> nstates;
		
		std::string str;
		getline(in, str);

		_table.resize(nstates);
		_chars.resize(nchars);
		for ( int c = 0; c < nchars; ++c )
			in >> _chars[c];

		for ( int s = 0; s < nstates; ++s )
		{
			for ( int c = 0; c < nchars; ++c )
			{
				int m;
				in >> m;
				for (;m; --m )
				{
					int tostate;
					in >> tostate;
					_table[s].second.insert(std::make_pair(_chars[c], tostate));
				}
			}
		}

		int nfstate, fstate;
		
		in >> _start;
		_table[_start].first |= START;

		in >> nfstate;
		for ( int i=0; i < nfstate; ++i )
		{
			in >> fstate;
			_table[fstate].first |= cinAL;
		}
	}

	void print_table();

	bool run( std::string input );

	int count_of_occurrences( std::string input );
};

class eNFA: public NFA
{
private:
	std::bitset<100> eclosure( int state );

	int is_status_cinal( std::bitset<100> state );

protected:
	std::vector<std::set<int> > _spontaneous;
public:
	eNFA( NFAtable table, std::vector<std::set<int> > spontaneous ):NFA(table),_spontaneous(spontaneous)
	{}

	eNFA( std::istream& in )
	{
		int nchars, nstates;
		in >> nchars >> nstates;

		std::string str;
		getline(in, str);

		_table.resize(nstates);
		_spontaneous.resize(nstates);
		_chars.resize(nchars);
		for ( int c = 0; c < nchars; ++c )
			in >> _chars[c];

		for ( int s = 0; s < nstates; ++s )
		{
			int m;
			for ( int c = 0; c < nchars; ++c )
			{
				in >> m;
				for (;m; --m )
				{
					int tostate;
					in >> tostate;
					_table[s].second.insert(std::make_pair(_chars[c], tostate));
				}
			}
			// Последняя пара это спонтанные переходы
			in >> m;
			_spontaneous[s].insert(s);
			for (;m; --m )
			{
				int tostate;
				in >> tostate;
				_spontaneous[s].insert(tostate);
			}
		}

		int nfstate, fstate;

		in >> _start;
		_table[_start].first |= START;

		in >> nfstate;
		for ( int i=0; i < nfstate; ++i )
		{
			in >> fstate;
			_table[fstate].first |= cinAL;
		}
	}


	void print_table();

	bool run( std::string input );

	int count_of_occurrences( std::string input );

	DFA determine();

};

#endif