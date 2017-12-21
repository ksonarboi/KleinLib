#include "UcBuffer.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h> // memcpy

using namespace klein;

static const char* const id = "$Id: //TPU-4XXX-Stream/2.13/KleinLib/UcBuffer.cpp#1 $";

//--------------------------------------------------------------------------
// Ctor
//--------------------------------------------------------------------------
UcBuffer::UcBuffer():_buf(NULL), _size(0), _pos(0)
{
}
//--------------------------------------------------------------------------
// Dtor
//--------------------------------------------------------------------------
UcBuffer::~UcBuffer()
{
	_size = 0;
	_pos = 0;
	if (_buf)
		delete [] _buf;
}
//--------------------------------------------------------------------------
// Copy Ctor
//--------------------------------------------------------------------------
UcBuffer::UcBuffer(const UcBuffer& rhs)
{
	if (rhs._size == 0)
	{
		_size = 0;
		_pos = 0;
		_buf = NULL;
	}
	else
	{
		_size = rhs._size;
		_pos = rhs._pos;
		_buf = new unsigned char[_size];
		memcpy(_buf, rhs._buf, _size);
	}
}
//--------------------------------------------------------------------------
// Operator =
//--------------------------------------------------------------------------
UcBuffer& UcBuffer::operator = (const UcBuffer& rhs)
{
	if (this == &rhs)
		return *this;

	if (rhs._size == 0)
	{
		_size = 0;
		_pos = 0;
		_buf = NULL;
	}
	else
	{
		if (_buf) delete [] _buf;
		_size = rhs._size;
		_pos = rhs._pos;
		_buf = new unsigned char[_size];
		memcpy(_buf, rhs._buf, _size);
	}

	return *this;
}
//--------------------------------------------------------------------------
// Move Ctor
//--------------------------------------------------------------------------
UcBuffer::UcBuffer(UcBuffer&& rhs)
{
	if (rhs._size == 0)
	{
		_size = 0;
		_pos = 0;
		_buf = NULL;
	}
	else
	{
		// steal rhs data
		_size = rhs._size;
		_pos = rhs._pos;
		_buf = rhs._buf;

		rhs._size = 0;
		rhs._pos = 0;
		rhs._buf = nullptr;
	}
}
//--------------------------------------------------------------------------
// Move Operator =
//--------------------------------------------------------------------------
UcBuffer& UcBuffer::operator = (UcBuffer&& rhs)
{
	if (this == &rhs)
		return *this;

	if (_buf) delete [] _buf;

	if (rhs._size == 0)
	{
		_size = 0;
		_pos = 0;
		_buf = NULL;
	}
	else
	{
		_size = rhs._size;
		_pos = rhs._pos;
		_buf = rhs._buf;

		rhs._size = 0;
		rhs._pos = 0;
		rhs._buf = nullptr;
	}

	return *this;
}
//--------------------------------------------------------------------------
// Ctor from unsigned char array
//--------------------------------------------------------------------------
UcBuffer::UcBuffer(const unsigned char* uc, const size_t size)
{
	// if (!uc) throw "Attempt to make UcBuffer from NULL";

	if (size == 0)
	{
		_size = 0;
		_pos = 0;
		_buf = NULL;
	}
	else
	{
		_size = size;
		_pos = size;
		_buf = new unsigned char[_size];
		memcpy(_buf, uc, _size);
	}
}
//--------------------------------------------------------------------------
// Ctor from string
//--------------------------------------------------------------------------
UcBuffer::UcBuffer(const std::string& t)
{
	if (t.empty())
	{
		_size = 0;
		_pos = 0;
		_buf = NULL;
	}
	else
	{
		_size = t.size()+1;
		_pos = _size;
		_buf = new unsigned char[_size];
		memcpy(_buf, t.c_str(), _size -1);
		_buf[_size-1] = '\0';
	}
}
//--------------------------------------------------------------------------
// Ctor from char*
//--------------------------------------------------------------------------
UcBuffer::UcBuffer(const char* c)
{
	if (!c)
	{ 
		_size = 0;
		_pos = 0;
		_buf = NULL;
	}
	else
	{
		_size = strlen(c) + 1;	
		_pos = _size;
		_buf = new unsigned char[_size];
		memcpy(_buf, c, _size-1);
		_buf[_size-1] = '\0';
		
	}
}
//--------------------------------------------------------------------------
// Ctor with size
//--------------------------------------------------------------------------
UcBuffer::UcBuffer(const size_t size)
{
	_size = size;
	_pos = 0;
	_buf = new unsigned char[_size];
	memset(_buf, '\0', _size);
}
//--------------------------------------------------------------------------
// UcBuffer substr
//--------------------------------------------------------------------------
UcBuffer UcBuffer::substr(const size_t start, const size_t size) const
{
	if (1 > _size)
	{
		// if rhs is empty
		return UcBuffer();
	}
	else if (start >= _size)
	{
		// start is at or beyond rhs end
		return UcBuffer();
	}
	else if (1 > size)
	{
		// nothing to copy
		return UcBuffer();
	}
	else if (start + size > _size)
	{
		// can only take (start + size) - rhs._size 
		return UcBuffer(_buf + start, (start + size) - _size);
	}
	else
	{
		// normal case
		// return *(new UcBuffer(_buf + start,  size));
		return UcBuffer(_buf + start,  size);
	}
}
//--------------------------------------------------------------------------
// UcBuffer data
//--------------------------------------------------------------------------
unsigned char* UcBuffer::data() const
{
	// if (0 == _size) throw "Attempt to use empty UcBuffer";

	return _buf;
}
//--------------------------------------------------------------------------
// UcBuffer data
//--------------------------------------------------------------------------
unsigned char* UcBuffer::data()
{
	// if (0 == _size) throw "Attempt to use empty UcBuffer";

	return _buf;
}
//--------------------------------------------------------------------------
// UcBuffer print
//--------------------------------------------------------------------------
std::ostream& UcBuffer::print(std::ostream& os) const
{
	// look for a non printable
	bool found = false;
	for (size_t i = 0; i < _pos; ++i)
	{
		const char c = static_cast<const char>(_buf[i]);

		if (!isprint(c))
		{
			switch(c)
			{
				case '\n':
				case '\r':
				case '\t':
				case '\b':
					continue; // try again
					break;
				case '\0':
					// check if null terminator
					if (i + 1 == _pos) continue;
					break;
				default:
					break;
			}
			found = true;
			break; // break for loop
		}
	}

	// if found then use 0x format
	if (found)
	{
		for (size_t i = 0; i < _pos; ++i)
		{
			os << std::hex << std::setw(2) << std::setfill('0') << (int)_buf[i] << " ";
			// os << (int)_buf[i] << " ";
			if ((i+1) % 16 == 0) os << std::endl;
		}
		os << std::dec;
	}
	else
	{
		// else make string
		os << (std::string)(*this);
		// buf is not null terminated - fails
		// const char* c = reinterpret_cast<const char*>(_buf);
		// os << c;
	}
	return os;
}
//--------------------------------------------------------------------------
// UcBuffer reserve
//--------------------------------------------------------------------------
void UcBuffer::reserve(size_t size)
{
	// if we have one
	if (0 < _size && _buf)
	{
		// want a bigger one
		if (size > _size)
		{
			// copy old into new
			UcBuffer tmp(*this);
			delete [] _buf;
			_size = size;
			_buf = new unsigned char[_size];
			memcpy(_buf, tmp.uc_str(), tmp.size());
			memset(_buf + tmp.size(), '\0', _size - tmp.size());
		}
		else
		{
			// existing one is bigger
			// don't truncate it
		}
	}
	else if (size > 0)
	{
		// didn't have one
		_size = size;
		_buf = new unsigned char[_size];
		memset(_buf, '\0', _size);
	}
	else
	{
		// didn't have one and didn't ask for one
	}
}
//--------------------------------------------------------------------------
// UcBuffer en64String
//--------------------------------------------------------------------------
std::string UcBuffer::en64String() const
{
	// make a base64 encoded string

	std::string result;
	// allocate room to stuff the bytes from the passphase into
	// add 1 for null, 1 for the truncation of the / and 2 for evenness
	// size_t s = sizeof(_buf)*8/6 + 4;
	size_t s = _size*8/6 + 4;
	char* p = new char[s];

	// en64(p, (const char*)_buf, (long)_size);

	result = std::string(p);

	delete [] p;

	return result;
}
//--------------------------------------------------------------------------
// UcBuffer de64String
//--------------------------------------------------------------------------
UcBuffer& UcBuffer::de64String(const std::string& data) 
{
	// load our buf with data decoded from base64

	if (data.empty())
	{
		_size = 0;
		_buf = NULL;
		return *this;
	}

	if (_size && _buf)
	{
		_size = 0;
		delete [] _buf;
	}

	_size = data.size() + 1;

	unsigned char* p = new unsigned char[_size];
	memcpy(p, data.c_str(), data.size());
	p[_size-1] = '\0';

//	_size = (size_t)un64((char*)p);

	if (_size > 0)
	{
		_buf = new unsigned char[_size];
		memcpy(_buf, p, _size);
	}

	delete [] p;

	return *this;
}
//--------------------------------------------------------------------------
// UcBuffer nonZero
//--------------------------------------------------------------------------
bool UcBuffer::nonZero() const
{
	for (size_t i = 0; i < _size; ++i)
		if (_buf[i] != (unsigned char) '\0') return true;
	return false;
}
//--------------------------------------------------------------------------
// UcBuffer Operator +=
//--------------------------------------------------------------------------
UcBuffer& UcBuffer::operator += (const UcBuffer& rhs)
{
	if (rhs._size == 0)
	{
		return *this;
	}

	if (_size == 0 && _buf)
	{
		// nothing there yet
		_size = rhs._size;
		_pos = 0;
		_buf = new unsigned char[_size];
		memcpy(_buf, rhs._buf, _size);
	}
	else if (_size - _pos >= rhs._size)
	{
		// if it will fit in our existing buffer
		memcpy(_buf + _pos, rhs._buf, rhs._size);
		_pos += rhs._size;
	}
	else
	{
		// save in tmp, while we realloc
		unsigned char* tmp = new unsigned char[_size];
		memcpy(tmp, _buf, _size);

		delete [] _buf;
		_buf = new unsigned char[_size + rhs._size];
		memcpy(_buf, tmp, _size);

		// adding self to self	
		if (this == &rhs)
			memcpy(_buf + _pos, tmp, rhs._size);
		else
			memcpy(_buf + _pos, rhs._buf, rhs._size);

		_size = _size + rhs._size;
		_pos = _pos + rhs._pos;

		delete [] tmp;
	}
	return *this;
}
//--------------------------------------------------------------------------
// UcBuffer push_back
//--------------------------------------------------------------------------
void UcBuffer::push_back(const unsigned char uc)
{
	if (_pos < _size)
	{
		// if it will fit in our existing buffer
		_buf[_pos] = uc;
		_pos++;
	}
	else
	{
		// realloc
		unsigned char* tmp = _buf;
		_buf = new unsigned char[_size + 1];
		memcpy(_buf, tmp, _size);
		_buf[_size] = uc;
		_size++;
		_pos++;

		delete [] tmp;
	}
}
//--------------------------------------------------------------------------
// UcBuffer find
//--------------------------------------------------------------------------
size_t UcBuffer::find(const unsigned char uc, const size_t pos)
{
	size_t ret = npos;
	for (size_t i = pos; i < _size; ++i)
		if (uc == _buf[i])  // valgrind complaint
			return i;
	return ret;
}
//--------------------------------------------------------------------------
// Operator () std::string
//--------------------------------------------------------------------------
UcBuffer::operator std::string() const
{
	// build up a std::string until a null is hit
	std::string s;
	if (0 == _size) return s;

	size_t i = 0;
	do
	{
		s.push_back(_buf[i]);
	}
	while (_buf[i] != '\0' && i++ < _size-1);

	return s;
}
//--------------------------------------------------------------------------
// Operator == UcBuffer
//--------------------------------------------------------------------------
bool klein::operator == (const UcBuffer& lhs, const UcBuffer& rhs)
{
	bool ret = false;
	if (lhs._size != rhs._size) return ret;
	if (lhs._pos != rhs._pos) return ret;
	const unsigned char* l = lhs._buf;
	const unsigned char* r = rhs._buf;
	while (*l++ == *r++)
		if (l == &(lhs._buf[lhs._size])) return true;
	return ret;
}
//--------------------------------------------------------------------------
// Operator +
//--------------------------------------------------------------------------
UcBuffer klein::operator + (UcBuffer& lhs, const UcBuffer& rhs)
{
	lhs += rhs;
	return lhs;
}
//--------------------------------------------------------------------------
// Operator <<
//--------------------------------------------------------------------------
namespace klein
{
	std::ostream& operator << (std::ostream& out, const UcBuffer& o)
	{
		out << "UcBuffer: " 
			<< "Size: " << std::dec << o._size << " "
			<< "Position: " << std::dec << o._pos << std::endl;

		if (o._size > 0) out << "\t";

		for (size_t i = 0; i < o._size; ++i)
		{
			out << std::hex << std::setw(2) << std::setfill('0') << (int)o[i] << " ";
			if ((i+1) % 16 == 0) 
				out << std::endl << "\t";
			// out << "i + 1: " << i+1 << " (i + 1) %
		}
		return out;
	}
}

// #define _UCBUFFER_UNIT_TEST_
#ifdef _UCBUFFER_UNIT_TEST_

#include <vector>

int main(const int ac, const char* av[])
{
	try
	{
		// empty UcBuffer
		UcBuffer o; std::cout << o << std::endl;

		std::cout << "o is nonZero? " << std::boolalpha << o.nonZero() << std::endl;

		// UcBuffer ctor char*
		UcBuffer p("Here is some stuff"); std::cout << p << std::endl;

		std::cout << "p is nonZero? " << std::boolalpha << p.nonZero() << std::endl;

		std::string s("Here is some more stuff"); 

		// UcBuffer ctor string
		UcBuffer q(s); std::cout << "q: " << q << std::endl;

		// UcBuffer ctor UcBuffer
		UcBuffer r(q); std::cout << "r: " <<r << std::endl;

		{
			const unsigned char w[16] = { '0', '1', '2', '3', 
				'4', '5', '6', '7', 
				'8', '9', 'A', 'B', 
				'C', 'D', 'E', 'F'};

			UcBuffer k(w, 16);
			std::cout << k << std::endl;
			// std::string k2 = k.en64String();
			// std::cout << k2 << std::endl;
			
			k.clear();
			if (k.empty()) std::cout << "k is empty" << std::endl;
			else std::cout << k << std::endl;

			UcBuffer k3(32);
			std::cout << k3 << std::endl;
			UcBuffer k4(w, 16);
			k3 = k4;
			std::cout << k3 << std::endl;

			k3.clear();

#if 0
			// can't catch SIGSEGV without signal
			try
			{
				memcpy(k3.data(), "bad stuff", sizeof("bad stuff"));
			}
			catch(const char*& e)
			{
				std::cout << e << std::endl;
			}
			catch(...)
			{
				std::cout << "caught expected unhandled exception, continuing" << std::endl;
			}
#endif

			k4.reserve(32);
			std::cout << k4 << std::endl;
			memcpy(k4.data(), "bad stuff", sizeof("bad stuff"));
			std::cout << k4 << std::endl;
			k4.reserve(2);
			std::cout << k4 << std::endl;
			k4.clear();
			std::cout << k4 << std::endl;
			k4.reserve(2);
			std::cout << k4 << std::endl;
			UcBuffer k5(w, 16);
			UcBuffer k6 = k5.substr(3, 4);
			std::cout << k6 << std::endl;
			UcBuffer k7 = k5.substr();
			std::cout << k7 << std::endl;
			k7 = k5.substr(8, 16);
			std::cout << k7 << std::endl;
			UcBuffer k8(k5);
			if (k8 == k5) std::cout << "equal" << std::endl;
			else std::cout << "not equal" << std::endl;
			k8.push_back('f');
			if (k8 == k5) std::cout << "equal" << std::endl;
			else std::cout << "not equal" << std::endl;
			std::cout << "pos of f: " << k8.find('f') << std::endl;
		}

		// try some containers
		{
			std::vector<UcBuffer> v;

			v.push_back("Here");
			v.push_back("Is");
			v.push_back("Some");
			v.push_back("Data");

			for (auto it = v.begin(); it != v.end(); ++it)
			{
				it->print(std::cout); 
				std::cout << " ";
			}
			std::cout << std::endl;

			v.push_back(v.front() + " Now");
			v.erase(v.begin());

			for (auto it = v.begin(); it != v.end(); ++it)
			{
				it->print(std::cout); 
				std::cout << " ";
			}
			std::cout << std::endl;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (std::string& e)
	{
		std::cerr << e << std::endl;
	}
	catch (char*& e)
	{
		std::cerr << e << std::endl;
	}
	catch (...)
	{
		std::cerr << "unknown exception" << std::endl;
	}
	return 0;
}
#endif // _UCBUFFER_UNIT_TEST_
