#pragma once
#include <string>
#include <iostream>

using uint = unsigned int;
using uchar = unsigned char;
using ushort = unsigned short;
using Number = double;
using String = std::string;

using UnAlignInt = int;
using UnAlignShort = short;
using UnAlignFloat = float;
using UnAlignDouble = double;

	//固定长度，暂不支持中文读写
	class FastByteArray
	{
	public:
		FastByteArray(uchar *data, uint dataLen) :_data(data), _dataLen(dataLen) , _position(0)
		{
			_externalData = true; 
		}

		FastByteArray(uint dataLen):_dataLen(dataLen), _position(0)
		{
			_data = new uchar[dataLen];
			_externalData = false;
		}

		~FastByteArray();
		
		void Uncompress();

		uchar* GetData() const { return _data; };

		uint GetDataLen() const { return _dataLen; }

		uint GetPosition() const { return _position; }

		uchar* GetCurrentPtr() const { return &_data[_position]; }

		void SetPosition(uint value) 
		{
			#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
				CheckOutOfRange(value);
			#endif
			_position = value;
		}

		void Skip(int len)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + len);
#endif
			_position += len;
		}

		uint GetBytesAvailable() const
		{
			return _dataLen - _position;
		}

		//----------------------------------------read---------------------------------------
		bool ReadBoolean()
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 1);
#endif
			return (bool)*(&_data[_position - 1]);
		}

		int ReadByte()
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 1);
#endif
			_position += 1;
			return *((char*)(&_data[_position - 1]));
		}

		int ReadUnsignedByte()
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 1);
#endif
			_position += 1;
			return *(&_data[_position - 1]);
		}

		int ReadShort()
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 2);
#endif
			_position += 2;
			return *((UnAlignShort*)(&_data[_position - 2]));
		}

		int ReadUnsignedShort()
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 2);
#endif
			_position += 2;
			return (ushort)*((UnAlignShort*)(&_data[_position - 2]));
		}

		int ReadInt()
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 4);
#endif
			_position += 4;
			return *((UnAlignInt*)(&_data[_position - 4]));
		}

		int ReadUnsignedInt()
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 4);
#endif
			_position += 4;
			return (uint)*((UnAlignInt*)(&_data[_position - 4]));
		}

		Number ReadFloat()
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 4);
#endif
			_position += 4;
			return *((UnAlignFloat*)(&_data[_position - 4]));
		}
		
		double ReadDouble()
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 8);
#endif
			_position += 8;
			return *((UnAlignDouble*)(&_data[_position - 8]));
		}
		
		String ReadUTFBytes(uint length)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + length);
#endif
			char *cp = (char*)(&_data[_position]);
			String st(cp, length);
			_position += length;
			return st;
		}

		String ReadUTF()
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 2);
#endif
			uint length = ReadUnsignedShort();
			return ReadUTFBytes(length);
		}
		
		void ReadBytes(FastByteArray &bytes, uint offset = 0, uint length = 0)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + length);
#endif
			memcpy(bytes.GetData() + offset, &_data[_position], length);
			_position += length;
		}

		void ReadBytes(uchar *bytes, uint length = 0)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + length);
#endif
			memcpy(bytes, &_data[_position], length);
			_position += length;
		}

		//-------------------------------------------------write-----------------------------------
		void WriteBoolean(bool value)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 1);
#endif
			_data[_position++] = value ? 1 : 0;			
		}

		void WriteByte(int value)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 1);
#endif
			_data[_position++] = value;
		}

		void WriteShort(int value)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 2);
#endif
			*(UnAlignShort*)(&_data[_position]) = value;
			_position += 2;
		}

		void WriteInt(int value)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 4);
#endif
			*(UnAlignInt*)(&_data[_position]) = value;
			_position += 4;
		}

		void WriteUnsignedInt(uint value)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 4);
#endif
			*(UnAlignInt*)(&_data[_position]) = value;
			_position += 4;
		}

		void WriteFloat(float value)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 4);
#endif
			*(UnAlignFloat*)(&_data[_position]) = value;
			_position += 4;
		}

		void WriteDouble( double value)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + 8);
#endif
			*(UnAlignDouble*)(&_data[_position]) = value;
			_position += 8;
		}

		void WriteUTF(String value)
		{
			size_t len = value.size();
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + len + 2);
#endif			
			WriteShort(len);
			const char *cp = value.c_str();
			memcpy(&_data[_position],cp,len);
			_position += len;
		}

		void WriteBytes(FastByteArray &bytes,uint offset = 0, uint length = 0)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(_position + length);
#endif
			memcpy(&_data[_position], bytes.GetData() + offset, length);
			_position += length;
		}


		void CheckOutOfRange(int pos)
		{
			if (pos > _dataLen || pos < 0)
				std::cout << "Out Of Range!" << std::endl;
		}


		//-----------------------------------------premium l and s--------------------------------------
		int li8(int ptr)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(ptr + 1);
#endif
			return *((char*)(&_data[ptr]));
		}

		int li16(int ptr)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(ptr + 2);
#endif
			return *((UnAlignShort*)(&_data[ptr]));
		}

		int li32(int ptr)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(ptr + 4);
#endif
			return *((UnAlignInt*)(&_data[ptr]));
		}

		Number lf32(int ptr)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(ptr + 4);
#endif
			return *((UnAlignFloat*)(&_data[ptr]));
		}

		double lf64(int ptr)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(ptr + 8);
#endif
			return *((UnAlignDouble*)(&_data[ptr]));
		}

		void si8(int value,int ptr)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(ptr + 1);
#endif
			_data[ptr] = value;
		}

		void si16(int value, int ptr)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(ptr + 2);
#endif
			*(UnAlignShort*)(&_data[ptr]) = value;
		}

		void si32(int value, int ptr)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(ptr + 4);
#endif
			*(UnAlignInt*)(&_data[ptr]) = value;
		}

		void sf32(float value, int ptr)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(ptr + 4);
#endif
			*(UnAlignFloat*)(&_data[ptr]) = value;
		}

		void sf64(double value, int ptr)
		{
#ifdef CHECK_BYTE_ARRAY_POSITION_RANGE
			CheckOutOfRange(ptr + 8);
#endif
			*(UnAlignDouble*)(&_data[ptr]) = value;
		}

	private:
		uchar* _data;
		uint _dataLen;
		uint _position;
		bool _externalData;

	public:
		static FastByteArray* T_BYTES;
	};
