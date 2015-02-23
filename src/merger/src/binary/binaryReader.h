#ifndef BINARYREADER_H
#define BINARYREADER_H

#include "../utils/stream.h"
#include "binaryStructures.h"
#include "binaryOperation.h"
#include <string>
#include <vector>

namespace binary {
    /*
     * \class BinaryReader
     * \brief Reads primitive data types from a given stream.
     */
    class BinaryReader : public BinaryOperation {
    private:
        template <typename T>
        T read_number(int bytesCount) {
            T n = 0;
            for (int i = 0; i < bytesCount; i++) {
                uint8_t b = this->read_byte();
                n |= b << (8 * i);
            }
            return n;
        }

    public:
        /*
         * \brief Constructs \c BinaryReader for a given stream.
         * \param stream The stream from which data will be read
         * \param pointer_size The size of \c MetaFileOffset in bytes
         * \param array_count_size The size of \c MetaArrayCount in bytes
         */
        BinaryReader(std::shared_ptr<utils::Stream> stream, int pointer_size, int array_count_size)
            : BinaryOperation(stream, pointer_size, array_count_size) { }

        BinaryReader(std::shared_ptr<utils::Stream> stream) : BinaryReader(stream, 4, 4) { }

        /*
         * \brief Reads a nil terminated string.
         */
        const std::string read_string();

        /*
         * \brief Reads a pointer.
         */
        MetaFileOffset read_pointer();

        /*
         * \brief Reads an array count.
         */
        MetaArrayCount read_arrayCount();

        /*
         * \brief Reads a binary array
         * A binary array is a collection of offsets
         */
        std::vector<binary::MetaFileOffset> read_binaryArray();

        /*
         * \brief Reads a 4 byte integer.
         */
        int32_t read_int();

        /*
         * \brief Reads a 2 byte short.
         */
        int16_t read_short();

        /*
         * \brief Reads a single byte.
         */
        uint8_t read_byte();
    };
}

#endif