/*!
  * \file ec.h
  * SDK of Enjoy Clicker device.
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
#define EC_QT_Control 10  //!<Control buzzer beep on, beep off and power off (rf215 only)
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
typedef struct _ec_Event
{
    char data[256];
    int eventType;
    int quizType;
    int keypadId;
    int quizNumber;
    int timeStamp;
} ec_Event;

/*!
 * \brief Port
 */
typedef void* ec_Port;

/*!
 * \brief Device
 */
typedef void* ec_Device;

EC_API void API_FUNC ec_sleep(int ms);

EC_API ec_Port API_FUNC ec_openPort(const char *address, int speed);
EC_API void API_FUNC ec_closePort(ec_Port port);
EC_API int API_FUNC ec_writePort(ec_Port port, int *data, int length, int timeout);
EC_API int API_FUNC ec_readPort(ec_Port port, int *data, int length, int timeout);

EC_API ec_Device API_FUNC ec_createDevice(ec_Port port, int deviceType);
EC_API void API_FUNC ec_destroyDevice(ec_Device device);

EC_API int API_FUNC ec_cmd(ec_Device device, int *cmd, int length, int timeout);

EC_API void API_FUNC ec_setDeviceMode(ec_Device device, int deviceMode);
EC_API int API_FUNC ec_openDevice(ec_Device device, int minId, int maxId);
EC_API void API_FUNC ec_closeDevice(ec_Device device);

EC_API int API_FUNC ec_getEvent(ec_Device device, ec_Event *event);
EC_API void API_FUNC ec_enableEvent(ec_Device device);
EC_API void API_FUNC ec_disableEvent(ec_Device device);

EC_API int API_FUNC ec_startQuiz(ec_Device device, int quizType, int isNew, int p1, int p2);
EC_API void API_FUNC ec_stopQuiz(ec_Device device);

EC_API void API_FUNC ec_setKeypadId(ec_Device device, int id);
EC_API void API_FUNC ec_setKeypadSn(ec_Device device, int sn);

EC_API void API_FUNC ec_startDynamicRegistration(ec_Device device, int address);
EC_API void API_FUNC ec_continueDynamicRegistration(ec_Device device);
EC_API void API_FUNC ec_stopDynamicRegistration(ec_Device device);

EC_API int API_FUNC ec_checkDeviceSn(ec_Device device, const char *sn);

//@}

#ifdef __cplusplus
}
#endif

#endif
