#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdint>
#include <cstring>
#include <vector>
#include <deque>
#include <utility>
#include <unistd.h>

class CLastFour
{
	std::deque<bool> d;
public:
	bool isQuiet()
	{
		for (auto &e : d)
		{
			if(e)
				return false;
		}
		return true;
	}
	void Push(bool b)
	{
		d.push_back(b);
		while (d.size() > 4)
			d.pop_front();
	}
};


void Usage(const std::string &comname, unsigned t)
{
	std::cout << "usage: " << comname << " [-l] [-t threshold] raw-audio-file" << std::endl;
	std::cout << "    The default value for threshold is " << t << std::endl;
}

int main(int argc, char *argv[])
{
	bool dolist = false;
	int opt;
	unsigned thresh = 20u;

	if (argc == 1)
	{
		Usage(argv[0], thresh);
		return EXIT_SUCCESS;
	}

	while (0 <= (opt = getopt(argc, argv, "lt:")))
	{
		switch (opt)
		{
		case 'l':
			dolist = true;
			break;
		case 't':
			thresh = atol(optarg);
			break;
		default: /* '?' */
			Usage(argv[0], thresh);
		    return EXIT_FAILURE;
		}
	}

	if (optind >= argc)
	{
		std::cerr << "Expected filename after options" << std::endl;
		Usage(argv[0], thresh);
		return EXIT_FAILURE;
	}


	std::filesystem::path p(argv[optind]);
	std::ifstream rfile(p, std::ios::binary);
	if (not rfile.is_open())
	{
		std::cerr << "Could not open '" << p.c_str() << "'" << std::endl;
		return EXIT_FAILURE;
	}

	std::ofstream lfile;
	if (dolist)
	{
		p.replace_extension(".list");
		lfile.open(p, std::ios::trunc);
		if (not lfile.is_open())
		{
			std::cerr << "Could Not open '" << p.c_str() << "'" << std::endl;
			return EXIT_FAILURE;
		}
		lfile << std::left << std::setw(6) << "#" << "Max" << std::endl;
	}

	std::vector<unsigned> v; // the array of frame max values
	int16_t aframe[160];     // a 20 ms audio frame (8000 Hz)
	unsigned c = 0;
	do
	{
		memset(aframe, 0, 320);
		rfile.read(reinterpret_cast<char *>(aframe), 320);
		unsigned max = 0;
		for (unsigned i=0; i<160; i++)
		{
			unsigned n = (aframe[i] < 0) ? -aframe[i] : aframe[i];
			if (n > max)
				max = n;
		}
		v.push_back(max);
		if (lfile.is_open())
			lfile << std::setw(6) << std::left << c << max << std::endl;
		c++;
	} while (not rfile.eof());
	rfile.close();

	if (lfile.is_open())
	{
		std::cout << "Created '" << p.c_str() << "' containing the maximum amplitude of " << v.size() << " 20-ms audio frames" << std::endl;
		lfile.close();
	}

	const unsigned l = v.size();

	if (l < 10u)
	{
		std::cout << "There are too few frame to do anything" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "using a threshold of " << thresh << std::endl;

	// initialize
	CLastFour l4; // this is a deque containing the last 4 threshold tests
	std::vector<std::pair<unsigned, unsigned>> words; // an array for the found words
	std::pair<unsigned, unsigned> word;	// start and end indices

	// process each frames
	for (c=0; c<l; c++)
	{
		l4.Push(v[c] >= thresh);

		if (l4.isQuiet())
			continue;	// bypas quiet gaps

		// index the beginning of the new word
		word.first = c;

		// and consume the word
		while (c<l and not l4.isQuiet())
		{
			l4.Push(v[c++] >= thresh);
		}

		// start at c and trim the end
		auto end = c;
		while (v[end] < thresh)
			end--;
		// now save it
		word.second = end;
		words.push_back(word);
	}

	// output the word index
	std::ofstream ifile;
	p.replace_extension(".index");
	ifile.open(p, std::ios::trunc);
	if (ifile.is_open())
		ifile << std::setw(6) << std::left << "#Word" << std::setw(6) << "Start" << std::setw(6) << "Stop" << "Length" << std::endl;
	else
	{
		std::cerr << "Could not open '" << p.c_str() << "'" << std::endl;
		return EXIT_FAILURE;
	}

	for (c=0; c<words.size(); c++)
	{
		auto start = words[c].first;
		auto end = words[c].second;
		ifile << std::left << std::setw(6) << c << std::setw(6) << start << std::setw(6) << end << (end - start) + 1 << std::endl;
	}
	ifile.close();

	std::cout << "Created '" << p .c_str()<<  "' containing " << words.size() << " found word" << ((words.size()>1) ? "s" : "") << std::endl;

	return words.size() ? EXIT_SUCCESS : EXIT_FAILURE;
}
