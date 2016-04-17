/*!
 * \file ec.h
 * SDK of Enjoy Clicker.
 * \mainpage EnjoyARS librf21x SDK API Reference
 *
 * \section intro_sec Introduction
 * EnjoyARS librf21x SDK is a cross platform SDK written in C/C++.
 */

#ifndef __ENJOY_CLICKER_H__
#define __ENJOY_CLICKER_H__

/*!
 * \def EC_API
 * Internal usage.
 */

/*!
 * \def API_FUNC
 * Internal usage.
 */

// Windows and SWIG
#ifdef SWIG
    #define API_FUNC
    #define EC_API
#else
    #ifdef WIN32
        #define API_FUNC __stdcall
        #ifdef EC_EXPORTS
            #define EC_API __declspec(dllexport)
        #else
            #define EC_API __declspec(dllimport)
        #endif
    #else
        #define API_FUNC
        #define EC_API
    #endif
#endif

/*!
 * \name Definition of Device Type
 */
//@{
#define EC_DT_Unknown 0 //!< Device Type: Unknown
#define EC_DT_RF215 1 //!< Device Type: RF215
#define EC_DT_RF217 2 //!< Device Type: RF217
#define EC_DT_RF218 3 //!< Device Type: RF218
#define EC_DT_RF219 4 //!< Device Type: RF219
#define EC_DT_RF317 5 //!< Device Type: RF317
//@}


/*!
 * \name Definition of Device Mode
 */
//@{
#define EC_DM_Static 0 //!< Device Mode: Static
#define EC_DM_Dynamic 1 //!< Device Mode: Dynamic
//@}


/*!
 * \name Definition of Event Type
 */
//@{
#define EC_ET_Unknown 0 //!< Message Type: Unknown
#define EC_ET_Teacher 1 //!< Message Type: Teacher
#define EC_ET_Student 2 //!< Message Type: Student
#define EC_ET_SetId 3 //!< Message Type: SetId
#define EC_ET_DynamicId 4 //!< Message Type: DynamicId
//@}


/*!
 * \name Definition of Quiz Type
 */
//@{
#define EC_QT_Homework -1 //!< Quiz Type: Homework
#define EC_QT_Unknown 0 //!< Quiz Type: Unknown
#define EC_QT_Rush 1 //!< Quiz Type: Rush
#define EC_QT_Single 2 //!< Quiz Type: Single
#define EC_QT_Multiple 3 //!< Quiz Type: Multiple
#define EC_QT_Number 4 //!< Quiz Type: Number
#define EC_QT_Text 5 //!< Quiz Type: Text (rf219 only)
#define EC_QT_Classify 6 //!< Quiz Type: Classify (rf219 only)
#define EC_QT_Sort 7 //!< Quiz Type: Sort (rf219 only)
#define EC_QT_JudgeOrVote 8 //!< Quiz Type: Judge Or Vote (rf219 only)
#define EC_QT_SelectId 9 //!< Select a buzzer Id (rf215 only)
#define EC_QT_Control 10  //!< Control buzzer beep on, beep off and power off (rf215 only)
#define EC_QT_SelfPaced 11 // only for 219
#define EC_QT_Notification = 12 // only for sending auth info to 228
//@}


#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \name EC
 */
//@{

/*!
 * \brief Event
 */
struct ec_Event
{
    char data[1024]; // plain text
    int eventType; // EC_ET_xxx
    int quizType; // EC_QT_xxx
    int keypadId; // Id of the keypad
    int quizNumber; // number of the quiz
    int timeStamp; // time stamp
};

/*!
 * \brief Port
 */
typedef void* ec_Port;

/*!
 * \brief Device
 */
typedef void* ec_Device;

/*!
 * \brief Sleep for a while
 */
EC_API void API_FUNC ec_sleep(int ms);

/*!
 * \brief Enumerate all USB HID ports and storge their path in data, each path takes 10 bytes
 */
EC_API const char* API_FUNC ec_enumerateAllHidPorts();

/*!
 * \brief Open a general serial port with speed baud, return the port handle
 */
EC_API ec_Port API_FUNC ec_openPort(const char *path, int baud);

/*!
 * \brief Close a general serial port
 */
EC_API void API_FUNC ec_closePort(ec_Port port);

/*!
 * \brief Read from port, return the number of bytes received
 */
EC_API int API_FUNC ec_readPort(ec_Port port, unsigned char *data, int length, int timeout = 0);

/*!
 * \brief Write to port, return the number of bytes sent
 */
EC_API int API_FUNC ec_writePort(ec_Port port, unsigned char *data, int length, int timeout = 0);

/*!
 * \brief Flush the buffer
 */
EC_API void API_FUNC ec_flushPort(ec_Port port);

/*!
 * \brief Create a device hanle based on port
 */
EC_API ec_Device API_FUNC ec_createDevice(ec_Port port, int deviceType);

/*!
 * \brief Destroy the device handle
 */
EC_API void API_FUNC ec_destroyDevice(ec_Device device);

/*!
 * \brief Set device mode before connection. Should be EC_DM_Static or EC_DM_Dynamic
 */
EC_API void API_FUNC ec_setDeviceMode(ec_Device device, int deviceMode);

/*!
 * \brief Connect to device. Teacher keypad is active after the connection
 * \return Success(1) or fail(0).
 */
EC_API int API_FUNC ec_connectDevice(ec_Device device, int minId, int maxId);

/*!
 * \brief Disconnect from device.
 */
EC_API void API_FUNC ec_disconnectDevice(ec_Device device);

/*!
 * \brief Get keypads' events from device.
 * \return Success(1) or fail(0).
 */
EC_API int API_FUNC ec_getEvent(ec_Device device, ec_Event *event);

/*!
 * \brief Parameters of different quiz type and device type.
 *
 * Quiz Type Support:
 * - rf217: Rush, Single.
 * - rf218: Rush, Single, Multiple, Number, Homework.
 * - rf219: Rush, Single, Multiple, Number, Text, Classify, Sort, Judge or Vote,SelfPaced, Homework.
 * - rf215: Rush, Single, SelectId, Control.
 * - rf228: Single, Multiple, Number, Notification.
 *
 * Parameters Support:
 * - rf217: Do not support any parameter.
 * - rf218:
 *   - Single, Multiple, Number:
 *     - param1:
 *       - 0 = one-by-one mode;
 *       - 1 = self-pace mode.
 *   - Homework:
 *     - param1: number of questions, 1~100
 *     - usage of rf218:
 *       - Only support single type, limited by number of 100, result returned by question groups, each group has 8 questions.
 *       - Fn+9: Enter homework mode.
 *       - Fn+0: Exit homework mode.
 *       - '<' & '>': Switch question.
 *       - Fn+>: Goto question.
 *       - Fn+CE: Clear all.
 *       - Fn+OK: Send all.
 * - rf219: Support different quiz type.
 *   - Single:
 *     - param1: Number of total options, 1~10.
 *   - Multiple:
 *     - param1: Number of total options, 1~8.
 *     - param2: Number of right options, 1~4 for 1~4 options, 5~8 for any options.
 *   - Text:
 *     - param1: Max length of message.
 *   - Classify:
 *     - param1: Number of total options, 1~10.
 *     - param2: Number of total classes, 2~6.
 *   - Sort:
 *     - param1: Number of total options, 1~9.
 *   - Judge or Vote:
 *     - param1: Number of total options, 1~3.
 *     - param2: Display type of judge or vote quiz:
 *       - 10 = Right / Wrong / None (Image)
 *       - 11 = 赞成 / 反对 / 弃权 (Chinese)
 *       - 12 = True / False / Don't know
 *       - 13 = Yes / No / Unsuer
 *       - 14 = votes for / against / abstention
 *       - 15 = Positive / negative / neutral
 *   - SelfPaced:
 *     - param1: number of questions 1~100, only for rf219.
 *   - Homework:
 *     - param1: number of questions, 1~100.
 *     - param2: subject, from 1~7, rf219 only.
 *     - usage of rf219:
 *       - Only support text type, limited by number of 100.
 *       - Fn+4: Enter homework mode.
 *       - Fn+4: Exit homework mode.
 *       - '<' & '>': Switch question.
 *       - Fn+1: Goto question.
 *       - Fn+CE: Clear all.
 *       - Fn+OK: Send all.
 * - rf215:
 *   - Rush: (The fastest one will flash and beep)
 *     - param1:
 *       - 1 = Start new rush quiz;
 *       - 2 = Continue last rush quiz;
 *	   - param2:
 *       - 1 = Only Rush button;
 *       - 2 = Rush button with option A,B,C and D select;
 *       - 3 = Only option A,B,C and D select.
 *   - Single:Do not support any parameter
 *   - Setlect Id : (The select buzzer will flash and beep)
 *     - param1: The buzzer Id value, 1~100;
 *     - param2:
 *       - 1 = The buzzer only flash, no need reply;
 *       - 2 = The buzzer need to select one option from A,B,C or D,
 *   - Control
 *     - param1:
 *       - 1 = Beep ON;
 *       - 2 = Beep OFF;
 *       - 3 = Power OFF buzzer.
 * - rf228:
 *   - Single, Multiple, Number:
 *     - param1:
 *       - 0 = one-by-one mode;
 *       - 1~100 = self-pace mode with sepecific number of questions.
 *   - Notification:
 *     - param1: SN of keypad.
 *     - param2:
 *       - 1 = Success;
 *       - 0 = Fail.
 *     - usage of auth：
 *       1. Press "Fn+2" to enter auth mode, enter user id and pwd to auth;
 *       2. The auth request will be returned in a message of type Auth.
 *       3. The message consists of user id in quizNumber, pwd in data, and SN in keypadID.
 *
 * \param device Device handle
 * \param type Quiz type.
 * \param param1 Parameter 1.
 * \param param2 Parameter 2.
 * \param newQuiz Only for RF219.
 *        newQuiz=0 start same quiz, the input content will be kept;
 *        newQuiz=1 start a new quiz, the input content will be cleared.
 * \return Success(1) or fail(0).
 */
EC_API int API_FUNC ec_startQuiz(ec_Device device, int quizType, int param1 = -1, int param2 = -1, int isNewQuiz = 1);

/*!
 * \brief Stop current quiz
 */
EC_API void API_FUNC ec_stopQuiz(ec_Device device);

/*!
 * \brief Set keypad ID, which need cooperation with keypad.
 * \param device Device handle.
 * \param id keypad ID.
 * \return Success(1) or fail(0).
 */
EC_API void API_FUNC ec_setKeypadId(ec_Device device, int id);

//EC_API void API_FUNC ec_setKeypadSn(`ec_Device device, int sn);

/*!
 * \brief Start dynamic registration with an input address, the actual addree might be changed
 */
EC_API int API_FUNC ec_startDynamicRegistration(ec_Device device, int address);

/*!
 * \brief Continue dynamic registration previous address, the actual addree will be put into the address parameter
 */
EC_API int API_FUNC ec_continueDynamicRegistration(ec_Device device, int address);

/*!
 * \brief Stop dynamic registration
 */
EC_API void API_FUNC ec_stopDynamicRegistration(ec_Device device);

/*!
 * \brief Check a 8-bytes length serial number from device. Worked for all devices.
 * \param device Device handle.
 * \param port Same as open function.
 * \param sn A 8-bytes serial number, each byte from 0 to 9. So a serial is like "01234567";
 * \return Success(1) or fail(0).
 */
EC_API int API_FUNC ec_checkDeviceSn(ec_Device device, const char *sn);

//@}

#ifdef __cplusplus
}
#endif

#endif
