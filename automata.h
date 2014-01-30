#ifndef __AUTOMATA__
#define __AUTOMATA__

#include<vector>
#include<map>
#include<unordered_map>
#include<set>
#include<bitset>
#include<iostream>
#include <iomanip>
#include<fstream>
#include<string>
#include<queue>

class automata
{
public:
	static const int START = 1;
	static const int FINAL = 2;
	virtual bool run( std::string input ) = 0;
	virtual void print_table() = 0;
	virtual int count_of_occurrences(std::string input) = 0;
};

class DFA: public automata
{
public:
	// таблица состоит из пар обозначение вершины и отображение символов в состо€ние
	typedef std::vector<std::pair<int, std::map<char, int> > > DFAtable;
protected:
	DFAtable _table;
	int _start;
public:
	DFA( DFAtable table ):_table(table)
	{}

	DFA( std::string filename)
	{
		std::ifstream fin(filename);

		int nchars, nstates;
		fin >> nchars >> nstates;
		
		std::string str;
		getline(fin, str);

		_table.resize(nstates);
		std::vector<char> chars(nchars);
		for ( int c = 0; c < nchars; ++c )
			fin >> chars[c];

		for ( int s = 0; s < nstates; ++s )
		{
			for ( int c = 0; c < nchars; ++c )
				fin >> _table[s].second[chars[c]];
		}

		int nfstate, fstate;
		
		fin >> _start;
		_table[_start].first |= START;

		fin >> nfstate;
		for ( int i=0; i < nfstate; ++i )
		{
			fin >> fstate;
			_table[fstate].first |= FINAL;
		}

		fin.close();
	}

	void print_table()
	{
		std::cout << "----------" << std::endl;
		std::cout << "      " << std::setw( (int)(log10(_table.size())+1) ) << " " ;
		for ( std::map<char, int>::const_iterator it = _table[0].second.begin(); it != _table[0].second.end(); ++it )
				std::cout << " | " << it->first;
		std::cout << std::endl;

		for ( int i=0; i < _table.size(); ++i )
		{
			std::cout << (_table[i].first & 1 ? "->" : "  ");
			std::cout << (_table[i].first & 2 ? "(" : " ");

			std::cout << "q_" << std::setw( (int)(log10(_table.size())+1) ) << i;

			std::cout << (_table[i].first & 2 ? ")" : " ");

			for ( std::map<char, int>::const_iterator it = _table[i].second.begin(); it != _table[i].second.end(); ++it )
				std::cout << " | " << it->second;

			std::cout << std::endl;
		}

		std::cout << "----------" << std::endl;
	}

	bool run( std::string input )
	{
		int state = _start;

		for ( std::string::const_iterator ci = input.begin(); ci != input.end(); ++ci )
			state = _table[state].second[*ci];

		return _table[state].first & FINAL;
	}

	/*
		ƒл€ автомата, который распознает слово <=> нужное подслова это суффикс данного слова. —читаем число достижений финального состо€ни€,
		т.е. число таких префиксов данного слова, что его суффикс это искомое подслово.
	*/
	int count_of_occurrences(std::string input)
	{
		int cnt = 0;
		int state = _start;

		for ( std::string::const_iterator ci = input.begin(); ci != input.end(); ++ci )
		{
			state = _table[state].second[*ci];
			if ( _table[state].first & FINAL )
				++cnt;
		}

		return cnt;
	}
};

class NFA: public automata
{
public:
	// таблица состоит из пар обозначение вершины и отображение символов в состо€ни€
	typedef std::vector<std::pair<int, std::multimap<char, int> > > NFAtable;
protected:
	NFAtable _table;
	int _start;
	std::vector<char> _chars;
public:
	NFA()
	{}

	NFA( NFAtable table ):_table(table)
	{}

	NFA( std::string filename )
	{
		std::ifstream fin(filename);

		int nchars, nstates;
		fin >> nchars >> nstates;
		
		std::string str;
		getline(fin, str);

		_table.resize(nstates);
		_chars.resize(nchars);
		for ( int c = 0; c < nchars; ++c )
			fin >> _chars[c];

		for ( int s = 0; s < nstates; ++s )
		{
			for ( int c = 0; c < nchars; ++c )
			{
				int m;
				fin >> m;
				for (;m; --m )
				{
					int tostate;
					fin >> tostate;
					_table[s].second.insert(std::make_pair(_chars[c], tostate));
				}
			}
		}

		int nfstate, fstate;
		
		fin >> _start;
		_table[_start].first |= START;

		fin >> nfstate;
		for ( int i=0; i < nfstate; ++i )
		{
			fin >> fstate;
			_table[fstate].first |= FINAL;
		}

		fin.close();
	}

	void print_table()
	{
		std::cout << "----------" << std::endl;
		std::cout << "      " << std::setw( (int)(log10(_table.size())+1) ) << " " ;
		for ( int i=0; i < _chars.size(); ++i )
			std::cout << " | " << _chars[i];
		std::cout << std::endl;

		for ( int i=0; i < _table.size(); ++i )
		{
			std::cout << (_table[i].first & 1 ? "->" : "  ");
			std::cout << (_table[i].first & 2 ? "(" : " ");

			std::cout << "q_" << std::setw( (int)(log10(_table.size())+1) ) << i;

			std::cout << (_table[i].first & 2 ? ")" : " ");

			for ( int c=0; c < _chars.size(); ++c )
			{
				std::cout << " | {";
				
				for ( std::multimap<char, int>::const_iterator si = _table[i].second.lower_bound(_chars[c]); si != _table[i].second.upper_bound(_chars[c]); ++si )
					std::cout << (si != _table[i].second.lower_bound(_chars[c]) ? ", " : "") << si->second;
				std::cout << "} ";
			}

			std::cout << std::endl;
		}

		std::cout << "----------" << std::endl;
	}

	bool run( std::string input )
	{
		std::queue<std::pair<int, int> > clones;
		clones.push( std::make_pair(_start, 0) );

		while ( !clones.empty() )
		{
			std::pair<int,int> clone = clones.front();
			clones.pop();

			if ( clone.second == input.size() && _table[clone.first].first & FINAL) // ≈сли дочитали слово до конца, самое врем€ проверить состо€ние и допустить
				return true;

			for ( std::multimap<char, int>::const_iterator si = _table[clone.first].second.lower_bound(input[clone.second]);
													si != _table[clone.first].second.upper_bound(input[clone.second]); ++si )
				clones.push( std::make_pair( si->second, clone.second + 1 ) );
		}
		// ≈сли все клоны мертвы, то мы не допустились.
		return false;
	}

	int count_of_occurrences( std::string input )
	{
		int cnt = 0;

		std::queue<std::pair<int, int> > clones;
		clones.push( std::make_pair(_start, 0) );

		while ( !clones.empty() )
		{
			std::pair<int,int> clone = clones.front();
			clones.pop();

			if ( clone.second == input.size() && _table[clone.first].first & FINAL) // ≈сли дочитали слово до конца, самое врем€ проверить состо€ние и допустить
				++cnt;

			for ( std::multimap<char, int>::const_iterator si = _table[clone.first].second.lower_bound(input[clone.second]);
													si != _table[clone.first].second.upper_bound(input[clone.second]); ++si )
				clones.push( std::make_pair( si->second, clone.second + 1 ) );
		}
		
		return cnt;
	}
};

class eNFA: public NFA
{
private:
	std::bitset<100> eclosure( int state )
	{
		std::bitset<100> s;

		s.set(state);

		for ( std::set<int>::const_iterator it = _spontaneous[state].cbegin(); it != _spontaneous[state].cend(); ++it )
		{
			if ( *it == state )
				continue;

			s.set(*it);
			s |= eclosure(*it);
		}

		return s;
	}

	int is_status_final( std::bitset<100> state )
	{
		int status = 0;

		for ( int s=0; s < 100; ++s )
		{
			if ( state.test(s) )
				status |= _table[s].first;
		}

		return status;
	}

protected:
	std::vector<std::set<int> > _spontaneous;
public:
	eNFA( NFAtable table, std::vector<std::set<int> > spontaneous ):NFA(table),_spontaneous(spontaneous)
	{}

	eNFA( std::string filename )
	{
		std::ifstream fin(filename);

		int nchars, nstates;
		fin >> nchars >> nstates;
		
		std::string str;
		getline(fin, str);

		_table.resize(nstates);
		_spontaneous.resize(nstates);
		_chars.resize(nchars);
		for ( int c = 0; c < nchars; ++c )
			fin >> _chars[c];

		for ( int s = 0; s < nstates; ++s )
		{
			int m;
			for ( int c = 0; c < nchars; ++c )
			{
				fin >> m;
				for (;m; --m )
				{
					int tostate;
					fin >> tostate;
					_table[s].second.insert(std::make_pair(_chars[c], tostate));
				}
			}
			// ѕоследн€€ пара это спонтанные переходы
			fin >> m;
			_spontaneous[s].insert(s);
			for (;m; --m )
			{
				int tostate;
				fin >> tostate;
				_spontaneous[s].insert(tostate);
			}
		}

		int nfstate, fstate;
		
		fin >> _start;
		_table[_start].first |= START;

		fin >> nfstate;
		for ( int i=0; i < nfstate; ++i )
		{
			fin >> fstate;
			_table[fstate].first |= FINAL;
		}

		fin.close();
	}

	void print_table()
	{
		std::cout << "----------" << std::endl;
		std::cout << "      " << std::setw( (int)(log10(_table.size())+1) ) << " " ;
		for ( int i=0; i < _chars.size(); ++i )
			std::cout << " | " << _chars[i];
		std::cout << " | " << "e" << std::endl;

		for ( int i=0; i < _table.size(); ++i )
		{
			std::cout << (_table[i].first & 1 ? "->" : "  ");
			std::cout << (_table[i].first & 2 ? "(" : " ");

			std::cout << "q_" << std::setw( (int)(log10(_table.size())+1) ) << i;

			std::cout << (_table[i].first & 2 ? ")" : " ");

			for ( int c=0; c < _chars.size(); ++c )
			{
				std::cout << " | {";
				
				for ( std::multimap<char, int>::const_iterator si = _table[i].second.lower_bound(_chars[c]); si != _table[i].second.upper_bound(_chars[c]); ++si )
					std::cout << (si != _table[i].second.lower_bound(_chars[c]) ? ", " : "") << si->second;
				std::cout << "} ";
			}

			// » спонтанные переходы
			std::cout << " | {";	
			for ( std::set<int>::const_iterator si = _spontaneous[i].begin(); si != _spontaneous[i].end(); ++si )
				std::cout << (si != _spontaneous[i].begin() ? ", " : "") << *si;
			std::cout << "} ";

			std::cout << std::endl;
		}

		std::cout << "----------" << std::endl;
	}

	bool run( std::string input )
	{
		std::queue<std::pair<int, int> > clones;
		clones.push( std::make_pair(_start, 0) );
		for ( std::set<int>::const_iterator it = _spontaneous[_start].cbegin(); it != _spontaneous[_start].cend(); ++it )
		{
			clones.push( std::make_pair( *it, 0 ) );
		}

		while ( !clones.empty() )
		{
			std::pair<int,int> clone = clones.front();
			clones.pop();

			if ( clone.second == input.size() && _table[clone.first].first & FINAL) // ≈сли дочитали слово до конца, самое врем€ проверить состо€ние и допустить
				return true;

			for ( std::multimap<char, int>::const_iterator si = _table[clone.first].second.lower_bound(input[clone.second]);
													si != _table[clone.first].second.upper_bound(input[clone.second]); ++si )
			{
				clones.push( std::make_pair( si->second, clone.second + 1 ) );
				for ( std::set<int>::const_iterator it = _spontaneous[si->second].cbegin(); it != _spontaneous[si->second].cend(); ++it )
				{
					clones.push( std::make_pair( *it, clone.second + 1 ) );
				}
			}
		}
		// ≈сли все клоны мертвы, то мы не допустились.
		return false;
	}

	int count_of_occurrences( std::string input )
	{
		int cnt = 0;

		std::queue<std::pair<int, int> > clones;
		clones.push( std::make_pair(_start, 0) );
		for ( std::set<int>::const_iterator it = _spontaneous[_start].cbegin(); it != _spontaneous[_start].cend(); ++it )
		{
			clones.push( std::make_pair( *it, 0 ) );
		}

		while ( !clones.empty() )
		{
			std::pair<int,int> clone = clones.front();
			clones.pop();

			if ( clone.second == input.size() && _table[clone.first].first & FINAL) // ≈сли дочитали слово до конца, самое врем€ проверить состо€ние и допустить
				++cnt;

			for ( std::multimap<char, int>::const_iterator si = _table[clone.first].second.lower_bound(input[clone.second]);
													si != _table[clone.first].second.upper_bound(input[clone.second]); ++si )
			{
				clones.push( std::make_pair( si->second, clone.second + 1 ) );
				for ( std::set<int>::const_iterator it = _spontaneous[si->second].cbegin(); it != _spontaneous[si->second].cend(); ++it )
				{
					clones.push( std::make_pair( *it, clone.second + 1 ) );
				}
			}
		}
		
		return cnt;
	}

	DFA determine()
	{
		using namespace std;
		// Ќу да, не более чем сотн€ состо€ний.
		// Ќу да, хардкод это плохо.
		// Ќу а щито поделать?
		map< int, pair<int, map<char, int > > > newtable;
		queue< int > q;
		unordered_map< bitset<100>, int> ns;
		unordered_map< int, bitset<100>> sn;
		int nstates = 0;

		bitset<100> state = eclosure(_start);
		ns[state] = nstates;
		sn[nstates] = state;
		q.push(nstates);
		++nstates;

		while ( !q.empty() )
		{
			int nstate = q.front();
			q.pop();

			if ( newtable.find( nstate ) != newtable.end() )
				continue;
			
			newtable.insert( make_pair( nstate, make_pair( is_status_final( sn[nstate] ), map<char, int>() ) ) );
			// ќтдельна€ обработка дь€вольского состо€ни€
			if ( sn[nstate].none() )
			{
				for ( int c=0; c < _chars.size(); ++c )
				{
					newtable[nstate].second.insert( make_pair( _chars[c], nstate ) );
				}
			}

			map<char, bitset<100> > bsets;
			for ( int c=0; c < _chars.size(); ++c )
			{
				bsets[_chars[c]] = bitset<100>();
			}

			// ѕо всем состо€ни€м этого состо€ни€
			for ( int s=0; s < 100; ++s )
			{
				if ( sn[nstate].test(s) )
				{
					for ( int c=0; c < _chars.size(); ++c )
					{
						for ( std::multimap<char, int>::const_iterator si = _table[s].second.lower_bound(_chars[c]);
															si != _table[s].second.upper_bound(_chars[c]);
															++si )
						{
							bsets[_chars[c]] |= eclosure( si->second );
						}
					}
				}
			}

			for ( int c=0; c < _chars.size(); ++c )
			{
				if ( ns.find(bsets[_chars[c]]) == ns.end() )
				{
					ns[bsets[_chars[c]]] = nstates;
					sn[nstates] = bsets[_chars[c]];
					q.push(nstates);
					++nstates;
				}
					
				newtable[nstate].second.insert( make_pair( _chars[c], ns[bsets[_chars[c]]] ) );
			}
		}



		DFA::DFAtable table(newtable.size());

		for ( auto it = newtable.begin(); it != newtable.end(); ++it )
		{
			table[it->first] = it->second;
		}

		std::cout << "----------" << std::endl;
		std::cout << "      " << std::setw( (int)(log10(table.size())+1) ) << " " ;
		for ( std::map<char, int>::const_iterator it = table[0].second.begin(); it != table[0].second.end(); ++it )
				std::cout << " | " << it->first;
		std::cout << std::endl;

		for ( int i=0; i < table.size(); ++i )
		{
			std::cout << (table[i].first & 1 ? "->" : "  ");
			std::cout << (table[i].first & 2 ? "(" : " ");

			std::cout << "q_" << std::setw( (int)(log10(table.size())+1) ) << i;

			std::cout << (table[i].first & 2 ? ")" : " ");

			cout << " | {";
			for ( int s=0; s < 100; ++s )
				if ( sn[i].test(s) )
					cout << "q_" << s << ", ";
			cout << " } | ";

			for ( std::map<char, int>::const_iterator it = table[i].second.begin(); it != table[i].second.end(); ++it )
				std::cout << " | " << it->second;

			std::cout << std::endl;
		}

		std::cout << "----------" << std::endl;

		return DFA(table);
	}

};

#endif