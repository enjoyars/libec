/*!
  * \file gsp.h
  * API of general serial port.
  */

#ifndef __GENERAL_SERIAL_PORT_H__
#define __GENERAL_SERIAL_PORT_H__

#include <string>

/*!
  * \brief General Serial port class.
  */
class Gsp
{
protected:
    /*!
     * \brief Constructor function.
     */
    explicit Gsp() {}

public:
    /*!
     * \brief Destructor function.
     */
    virtual ~Gsp() {}

    /*!
     * \brief Open serial port.
     */
    virtual bool open(const std::string path, int baud) = 0;

    /*!
     * \brief Close serial port.
     */
    virtual bool close() = 0;

    /*!
     * \brief Check is port opened.
     */
    virtual bool isOpened() = 0;

    /*!
     * \brief Read data from port, return bytes received.
     */
    virtual int read(unsigned char *data, int length, int timeout = 0) = 0;

    /*!
     * \brief Read data from port, return bytes written.
     */
    virtual int write(const unsigned char *data, int length, int timeout = 0) = 0;

    /*!
     * \brief Flush the serial port.
     */
    virtual void flush() = 0;
};

/*!
 * \brief Create a serial port class.
 */
Gsp* createGsp(const std::string path, int baud);
void destroyGsp(Gsp *gsp);

#endif
