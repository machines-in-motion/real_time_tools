#ifndef USB_STREAM_HPP
#define USB_STREAM_HPP

#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <stdint.h>  // uint8_t

#if defined(XENOMAI)
#elif defined(NON_REAL_TIME) || defined(RT_PREEMPT)
#include <fcntl.h>
#include <termios.h>  // POSIX control def: tcgetattr, cfsetispeed, tcsetattr
#endif

namespace real_time_tools
{
/**
 * @brief Simple config class that encapsulate the port parameters for a USB
 * port. This should cover enough paramter to setup the USB port for the
 * imu_3DM_GX3_25, imu_3DM_GX3_45 and the imu_3DM_GX5 in xenomai, rt_preempt
 * and ubuntu (potentially MacOS: non posix).
 */
class PortConfig
{
public:
    /**
     * @brief This is if one wants 1 or 2 stop bits
     */
    enum StopBits
    {
        one = 1,
        two = 2
    };

    /**
     * @brief This correspond to the number of data bits echanged
     */
    enum DataBits
    {
        cs7 = 0,
        cs8 = 1
    };

    /**
     * @brief Get the _bauderate object
     *
     * @return int
     */
    int get_bauderate();

public:
    /**
     * @brief Enabling/Disabling rts cts. TODO: look for what is rts cts
     */
    bool rts_cts_enabled_;
    /**
     * @brief Use or not a parity bit
     */
    bool parity_;
    /**
     * @brief Defines the choice of the stop bits. (see enum StopBits)
     */
    StopBits stop_bits_;
    /**
     * @brief Defines if the port should prepare the size definition.
     */
    bool prepare_size_definition_;
    /**
     * @brief Defines the number of bits echanged. (see enum DataBits)
     */
    DataBits data_bits_;
    /**
     * @brief Defines the BaudeRate to be used. (see enum BaudeRate)
     */
    int baude_rate_;
};

/**
 * @brief This class has for purpose to interact with devices and files alike
 * as the linux philosophie does. Depending on the current Operating system
 * it uses the available real time APIs.
 */
class UsbStream
{
public:
    /**
     * @brief Construct a new fstream object
     */
    UsbStream();

    /**
     * @brief Destroy the fstream object
     */
    ~UsbStream();

    /**
     * @brief This method allows you to open a port or a file.
     *
     * @param file_name
     */
    bool open_device(const std::string& file_name);

    /**
     * @brief Set the _port_config object parametrize the port configuration
     *
     * @param user_config is the configuration of the port. (see struct
     * PortConfig)
     * @return true
     * @return false
     */
    bool set_port_config(const PortConfig& user_config);

    /**
     * @brief Stop the device communication
     *
     * @return true success
     * @return false problem occured
     */
    bool close_device();

    /**
     * @brief Read the port or the file. Reports error if read bytes is not equal to expected bytes.
     *
     * @param msg is the command sent before this command was executed.
     * @param stream_on define if we just read on the fly or we wait until we
     * get the correct amount of data.
     * @return true
     * @return false
     */
    bool read_device(std::vector<uint8_t>& msg, const bool stream_on = true);

    /**
     * @brief Read the port or the file. Does not check if read bytes is equal to expected bytes.
     *
     * @param msg is the command sent before this command was executed.
     * @param stream_on define if we just read on the fly or we wait until we
     * get the correct amount of data.
     * @param start_location is the index in msg that we want to start reading to. 
     * This is for when you want to keep a portion of the already existing data:
     * For example, when you are have read a partial message and need to get the rest
     * while keeping the first part.
     * @return Number of bytes read or -1 if failure.
     */
    ssize_t read_device_raw(std::vector<uint8_t>& msg, const bool stream_on = true, const size_t start_location = 0);

    /**
     * @brief Write msg in the port or the file.
     *
     * @return true success
     * @return false problem occured
     */
    bool write_device(const std::vector<uint8_t>& msg);

    /**
     * @brief Activate the stream mode. The read method is not blocking.
     *
     * @return true success
     * @return false problem occured
     */
    bool activate_stream_mode();

    /**
     * @brief Set the poll mode timeout. The read_device method is blocking
     * until timeout.
     *
     * @return true success
     * @return false problem occured
     */
    bool set_poll_mode_timeout(double timeout_in_second);

    /**
     * @brief Flush the current port
     *
     * @return true
     * @return false
     */
    bool flush(int duration_ms = 150);

    /**
     * @brief Display the uint8_t message in hexadecimal format.
     *
     * @param msg is the message to be displayed
     * @param until is a bound on the number of displayed bytes. "-1" means
     * display all.
     * @return std::string the debug string
     */
    static std::string msg_debug_string(const std::vector<uint8_t>& msg,
                                        long int until = -1);

    /**
     * @brief Test if two message are the same or not
     *
     * @param msg1
     * @param msg2
     * @return true
     * @return false
     */
    static bool test_msg_equal(const std::vector<uint8_t>& msg1,
                               const std::vector<uint8_t>& msg2);

    /**
     * Private methods
     */
private:
    /**
     * Attributes
     */
private:
    /**
     * @brief This is the path tot the device file
     */
    std::string file_name_;
    /**
     * @brief This is the port id.
     */
    int file_id_;
    /**
     * @brief This is the return value of the different POSIX/Xenomai methods.
     */
    ssize_t return_value_;
    /**
     * @brief Verify that the timeout value has been set.
     */
    bool timeout_set_;
    /**
     * @brief The timeout for the poll mode in seconds.
     */
    double timeout_;

#if defined(XENOMAI)
#elif defined(RT_PREEMPT) || defined(NON_REAL_TIME)
    /**
     * @brief port configuration from Posix. This class defines some default
     * ways to configure the port. Please inherite from this class to define a
     * different behavior.
     */
    struct termios config_;
    /**
     * @brief The timeout for the poll mode.
     */
    struct timespec timeout_posix_;
    /**
     * @brief An fd_set is a fixed size buffer. Executing FD_CLR() or FD_SET()
     * with a value of fd that is negative or is equal to or larger than
     * FD_SETSIZE will result in undefined behavior. Moreover, POSIX requires fd
     * to be a valid file descriptor.
     */
    fd_set file_id_set_;
#endif

    /**
     * @brief Internal buffer that is supposed to be much bigger than the
     * message sent or received to avoid memory problems
     */
    std::vector<uint8_t> buffer_;
};

}  // namespace real_time_tools

#endif  // HEADER PROTECTION

// /**
//  * @brief Verify if a previous command has been taken into account. This
//  * suppose that the protocole of communication expect a command in and a
//  * command out. send a get data code: "id" and receive "id XXXXX".
//  * Typically IMU from MicrosStrain follow this procedure.
//  *
//  * @param rep is the output buffer of read.
//  * @param rep_len is the expected message length.
//  * @return true if the message contains the expected length.
//  * @return false if the checksum is incorrect.
//  */
// virtual bool is_checksum_correct(const ImuMsg& msg);

// /**
//  * @brief
//  *
//  * @param command
//  * @param reply
//  * @return true
//  * @return false
//  */
// bool read_misaligned_msg_from_device(ImuMsg& msg);

// /**
//  * @brief byte swap of uint16_t
//  *
//  * @param x the uint16_t to byte swap
//  * @return swapped uint16_t
//  */
// static uint16_t bswap_16(uint16_t x)
// {return (x >> 8) | (x << 8);}

// /**
//  * @brief byte swap of uint32_t
//  *
//  * @param x the uint32_t to byte swap
//  * @return swapped uint32_t
//  */
// static uint32_t bswap_32(uint32_t x)
// {return (bswap_16(x & 0xffff) << 16) | (bswap_16(x >> 16));}
