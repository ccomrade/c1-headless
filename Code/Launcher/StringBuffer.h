/**
 * @file
 * @brief String buffer.
 */

#pragma once

#include <cstring>
#include <new>

// Library headers
#include "printf/printf.h"

// va_copy is available only in VS2013 and later
#if defined(_MSC_VER) && _MSC_VER < 1800
#define va_copy(dest,src) ((dest) = (src))  // dirty haxs
#endif

using std::size_t;

template<size_t DefaultSize>
class StringBuffer
{
	char m_stackBuffer[DefaultSize];
	char *m_buffer;
	size_t m_bufferSize;
	size_t m_pos;

	// disable implicit copy constructor and copy assignment operator
	StringBuffer( const StringBuffer & );
	StringBuffer & operator=( const StringBuffer & );

public:
	StringBuffer()
	{
		m_stackBuffer[0] = '\0';
		m_buffer = m_stackBuffer;
		m_bufferSize = DefaultSize;
		m_pos = 0;
	}

	~StringBuffer()
	{
		if ( isHeap() )
		{
			delete [] m_buffer;
		}
	}

	bool isHeap() const
	{
		return m_buffer != m_stackBuffer;
	}

	bool isEmpty() const
	{
		return m_pos == 0;
	}

	const char *get() const
	{
		return m_buffer;
	}

	size_t getLength() const
	{
		return m_pos;
	}

	size_t getAvailableLength() const
	{
		return m_bufferSize - m_pos - 1;
	}

	size_t getCapacity() const
	{
		return m_bufferSize;
	}

	char operator[]( size_t index ) const
	{
		return m_buffer[index];
	}

	char getLast() const
	{
		return (m_pos > 0) ? m_buffer[m_pos-1] : m_buffer[0];
	}

	void pop( size_t length = 1 )
	{
		m_pos -= (length < m_pos) ? length : m_pos;
		m_buffer[m_pos] = '\0';
	}

	StringBuffer<DefaultSize> & operator+=( char c )
	{
		append( c );
		return *this;
	}

	StringBuffer<DefaultSize> & operator+=( const char *string )
	{
		append( string );
		return *this;
	}

	void append( char c )
	{
		makeSpaceFor( 1 );

		m_buffer[m_pos] = c;
		m_pos += 1;
		m_buffer[m_pos] = '\0';
	}

	void append( const char *string )
	{
		if ( ! string )
		{
			return;
		}

		append( string, std::strlen( string ) );
	}

	void append( const char *string, size_t length )
	{
		if ( ! string || length == 0 )
		{
			return;
		}

		makeSpaceFor( length );

		std::memcpy( m_buffer + m_pos, string, length );
		m_pos += length;
		m_buffer[m_pos] = '\0';
	}

	template<size_t U>
	void append( const StringBuffer<U> & other )
	{
		append( other.get(), other.getLength() );
	}

	int append_f( const char *format, ... )
	{
		if ( ! format )
		{
			return 0;
		}

		va_list args;
		va_start( args, format );

		va_list argsCopy;
		va_copy( argsCopy, args );

		int status = vsnprintf_( m_buffer + m_pos, m_bufferSize - m_pos, format, args );

		va_end( args );

		if ( status > 0 )
		{
			size_t length = status;
			// check if the resulting string was truncated
			if ( length > getAvailableLength() )
			{
				makeSpaceFor( length );

				m_pos += vsnprintf_( m_buffer + m_pos, m_bufferSize - m_pos, format, argsCopy );
			}
			else
			{
				m_pos += length;
			}
		}

		va_end( argsCopy );

		return status;
	}

	int append_vf( const char *format, va_list args )
	{
		if ( ! format )
		{
			return 0;
		}

		va_list argsCopy;
		va_copy( argsCopy, args );

		int status = vsnprintf_( m_buffer + m_pos, m_bufferSize - m_pos, format, args );

		if ( status > 0 )
		{
			size_t length = status;
			// check if the resulting string was truncated
			if ( length > getAvailableLength() )
			{
				makeSpaceFor( length );

				m_pos += vsnprintf_( m_buffer + m_pos, m_bufferSize - m_pos, format, argsCopy );
			}
			else
			{
				m_pos += length;
			}
		}

		va_end( argsCopy );

		return status;
	}

	void makeSpaceFor( size_t length )
	{
		if ( length > getAvailableLength() )
		{
			size_t requiredSize = getLength() + length + 1;
			size_t minimalSize = getCapacity() * 2;
			size_t newSize = (requiredSize > minimalSize) ? requiredSize : minimalSize;

			resize( newSize );
		}
	}

	void resize( size_t size )
	{
		if ( size == 0 || size == m_bufferSize )
		{
			return;
		}

		char *oldBuffer = m_buffer;
		char *newBuffer = (size > DefaultSize) ? new char[size] : m_stackBuffer;

		if ( newBuffer == oldBuffer )
		{
			return;
		}

		if ( m_pos >= size )
		{
			m_pos = size - 1;
			oldBuffer[m_pos] = '\0';
		}

		std::memcpy( newBuffer, oldBuffer, m_pos + 1 );

		if ( isHeap() )
		{
			delete [] oldBuffer;
		}

		m_buffer = newBuffer;
		m_bufferSize = size;
	}
};

