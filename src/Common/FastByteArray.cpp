#include "FastByteArray.h"

FastByteArray* FastByteArray::T_BYTES = new FastByteArray(1024 * 1024 * 10);

FastByteArray::~FastByteArray()
{
	if (_externalData)
	{
		_data = nullptr;
	}
	else
	{
		delete _data;
		_data = nullptr;
	}
}

void FastByteArray::Uncompress()
{
	//todo
}