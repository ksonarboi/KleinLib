#ifndef _KLEIN__UCBUFFER_H_
#define _KLEIN__UCBUFFER_H_

// $Id: //TPU-4XXX-Stream/2.13/KleinLib/UcBuffer.h#1 $

#include <ostream>
#include <string>
#include <cstring>

// class to mimic string class only using unsigned chars instead of chars
namespace klein
{
	class UcBuffer
	{
	public:
		// constructors
		UcBuffer();
		~UcBuffer();
		UcBuffer(const UcBuffer& rhs);
		UcBuffer(UcBuffer&& rhs);

		UcBuffer(const unsigned char* uc, const size_t size);
		UcBuffer(const std::string& s);
		UcBuffer(const char* c);
		UcBuffer(const size_t size);

		// assignment operators
		UcBuffer& operator = (const UcBuffer& rhs);
		UcBuffer& operator = (UcBuffer&& rhs);

		UcBuffer substr(const size_t pos = 0, const size_t n = 0) const;

		// acessors
		inline const unsigned char* uc_str() const { return _buf; }
		inline const unsigned char* uc_str() { return _buf; }

		unsigned char* data();
		unsigned char* data() const;

		inline const size_t size() const { return _size; }
		inline const size_t size() { return _size; }

		// operations
		inline const bool empty() const { return 0 == _pos; }
		inline void clear() { _size = 0; _pos = 0; if (_buf) delete [] _buf; _buf = NULL; }
		inline const size_t  length() const { return _pos; }
		inline const size_t  capacity() const { return _size; }

		inline void resize(const size_t n) 
		{
			if (n > _size)
			{
				unsigned char* k = _buf;
				_buf = new unsigned char[n];
				memset(_buf, 0, n);
				memcpy(_buf, k, _pos);
				_size = n;
				delete [] k;
			}
			if (n < _pos)
			{
				_pos = n;
			}
		}

		std::ostream& print(std::ostream& os) const;

		void reserve(size_t size);

		bool nonZero() const;

		std::string en64String() const;
		UcBuffer& de64String(const std::string& data);

		operator std::string() const;

		// compound assignment
		UcBuffer& operator += (const UcBuffer& rhs);

		void push_back(const unsigned char uc);
		size_t find(const unsigned char uc, const size_t pos = 0);

		inline const unsigned char& operator [] (const size_t pos) const { return _buf[pos]; } 
		inline unsigned char& operator [] (const size_t pos) { return _buf[pos]; } 

		constexpr static size_t npos = -1;

	private:
		unsigned char* _buf;
		// how big is the buffer
		size_t _size;
		// next place to put
		size_t _pos;

	friend std::ostream& operator << (std::ostream& out, const UcBuffer&);
	friend bool operator == (const UcBuffer& lhs, const UcBuffer& rhs);
	};
	
	UcBuffer operator + (UcBuffer& lhs, const UcBuffer& rhs);
	bool operator == (const UcBuffer& lhs, const UcBuffer& rhs);
} // namespace Klein

#endif // _UCBUFFER_H_

