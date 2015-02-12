/*!
  * \file gsp.h
  * API of general serial port.
  */

#ifndef __GENERAL_SERIAL_PORT_H__
#define __GENERAL_SERIAL_PORT_H__

#define GSP_MAX_DATA_SIZE 1000

#include <string>

#include "../3rdparty/tthread/tinythread.h"

class AbstractGsp;
class Gsp;

/*!
  * \brief Data receiving event class of gsp.
  */
class GspRecvEvent
{
public:
    /*!
     * \brief Data receiving event function of gsp.
     */
    virtual void dataRecvEvent(AbstractGsp *gsp, unsigned char *data, int length) = 0;
};


/*!
  * \brief Abstract class of gsp.
  */
class AbstractGsp
{
    friend class Gsp;
public:
    explicit AbstractGsp();
    virtual ~AbstractGsp();
    virtual void setRecvEvent(GspRecvEvent *recvEvent);
    virtual void enableRecvEvent();
    virtual void disableRecvEvent();

    virtual bool open(const std::string path, int baud) = 0;
    virtual bool close() = 0;
    virtual bool isOpened() = 0;
    virtual int readData(void *data, int length) = 0;
    virtual int writeData(const void *data, int length) = 0;
    virtual void flush() = 0;
    virtual void setTimeout(int ms) = 0;

protected:
    virtual void _dataRecvEvent(unsigned char *data, int length);

    tthread::mutex _mutexEvent;
    GspRecvEvent *_recvEvent;

private:
    bool _isEventEnable;
};


/*!
  * \brief General Serial port class.
  */
class Gsp
{
public:
    /*!
     * \brief Constructor function.
     */
    explicit Gsp();

    /*!
     * \brief Destructor function.
     */
    virtual ~Gsp();

    /*!
     * \brief Set data receiving event.
     */
    virtual void setRecvEvent(GspRecvEvent *recvEvent);

    /*!
     * \brief Enable data receiving event.
     */
    virtual void enableRecvEvent();

    /*!
     * \brief Disable data receiving event.
     */
    virtual void disableRecvEvent();

    /*!
     * \brief Open serial port.
     */
    virtual bool open(const std::string path, const int baud);

    /*!
     * \brief Close serial port.
     */
    virtual bool close();

    /*!
     * \brief Check is port opened.
     */
    virtual bool isOpened();

    /*!
     * \brief Read data from port, return bytes received.
     */
    virtual int readData(void *data, int length);

    /*!
     * \brief Read data from port, return bytes written.
     */
    virtual int writeData(const void *data, int length);

    /*!
     * \brief Flush port buffer.
     */
    virtual void flush();

    /*!
     * \brief Set reading and writing timeout.
     */
    virtual void setTimeout(int ms);

    /*!
     * \brief Get HID device serial number.
     *  User can use serial number as address which like "hid://0007800D" to select a hid device to be opened.
     * \param index Index number of device, start from 0.
     * \param str Serial number got from device.
     * \return success or fail.
     */
    static bool getHidSerialNumber(int index, std::string &str);

private:
    AbstractGsp *_sp;
};

#endif
