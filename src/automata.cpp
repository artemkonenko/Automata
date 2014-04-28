#include <iostream>
#include <bitset>
#include <automata.h>

using namespace std;

void DFA::print_table()
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

bool DFA::run(std::string input)
{
	int state = _start;

	for ( std::string::const_iterator ci = input.begin(); ci != input.end(); ++ci )
		state = _table[state].second[*ci];

	return _table[state].first & cinAL;
}


int DFA::count_of_occurrences(std::string input)
{
	int cnt = 0;
	int state = _start;

	for ( std::string::const_iterator ci = input.begin(); ci != input.end(); ++ci )
	{
		state = _table[state].second[*ci];
		if ( _table[state].first & cinAL )
			++cnt;
	}

	return cnt;
}

std::bitset <100> eNFA::eclosure(int state)
{
	bitset<100> s;

	s.set(state);

	for ( auto it = _spontaneous[state].cbegin(); it != _spontaneous[state].cend(); ++it )
	{
		if ( *it == state )
			continue;

		s.set(*it);
		s |= eclosure(*it);
	}

	return s;
}

int eNFA::is_status_cinal(std::bitset<100> state)
{
	int status = 0;

	for ( int s=0; s < 100; ++s )
	{
		if ( state.test(s) )
			status |= _table[s].first;
	}

	return status;
}

void NFA::print_table()
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


bool NFA::run(std::string input)
{
	std::queue<std::pair<int, int> > clones;
	clones.push( std::make_pair(_start, 0) );

	while ( !clones.empty() )
	{
		std::pair<int,int> clone = clones.front();
		clones.pop();

		if ( clone.second == input.size() && _table[clone.first].first & cinAL) // Если дочитали слово до конца, самое время проверить состояние и допустить
			return true;

		for ( std::multimap<char, int>::const_iterator si = _table[clone.first].second.lower_bound(input[clone.second]);
		      si != _table[clone.first].second.upper_bound(input[clone.second]); ++si )
			clones.push( std::make_pair( si->second, clone.second + 1 ) );
	}
	// Если все клоны мертвы, то мы не допустились.
	return false;
}

int NFA::count_of_occurrences(std::string input)
{
	int cnt = 0;

	std::queue<std::pair<int, int> > clones;
	clones.push( std::make_pair(_start, 0) );

	while ( !clones.empty() )
	{
		std::pair<int,int> clone = clones.front();
		clones.pop();

		if ( clone.second == input.size() && _table[clone.first].first & cinAL) // Если дочитали слово до конца, самое время проверить состояние и допустить
			++cnt;

		for ( std::multimap<char, int>::const_iterator si = _table[clone.first].second.lower_bound(input[clone.second]);
		      si != _table[clone.first].second.upper_bound(input[clone.second]); ++si )
			clones.push( std::make_pair( si->second, clone.second + 1 ) );
	}

	return cnt;
}

void eNFA::print_table()
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

		// И спонтанные переходы
		std::cout << " | {";
		for ( std::set<int>::const_iterator si = _spontaneous[i].begin(); si != _spontaneous[i].end(); ++si )
			std::cout << (si != _spontaneous[i].begin() ? ", " : "") << *si;
		std::cout << "} ";

		std::cout << std::endl;
	}

	std::cout << "----------" << std::endl;
}

bool eNFA::run(std::string input)
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

		if ( clone.second == input.size() && _table[clone.first].first & cinAL) // Если дочитали слово до конца, самое время проверить состояние и допустить
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
	// Если все клоны мертвы, то мы не допустились.
	return false;
}


int eNFA::count_of_occurrences(std::string input)
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

		if ( clone.second == input.size() && _table[clone.first].first & cinAL) // Если дочитали слово до конца, самое время проверить состояние и допустить
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


DFA eNFA::determine()
{
	using namespace std;
	// Ну да, не более чем сотня состояний.
	// Ну да, хардкод это плохо.
	// Ну а щито поделать?
	map< int, pair<int, map<char, int > > > newtable;
	queue< int > q;
	unordered_map< bitset<100>, int> ns;
	unordered_map< int, bitset<100> > sn;
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

		newtable.insert( make_pair( nstate, make_pair( is_status_cinal( sn[nstate] ), map<char, int>() ) ) );
		// Отдельная обработка дьявольского состояния
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

		// По всем состояниям этого состояния
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