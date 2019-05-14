#ifndef USB_STREAM_HPP
#define USB_STREAM_HPP

#include <array>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <stdint.h> // uint8_t

#if defined(XENOMAI)
#elif defined(NON_REAL_TIME) || defined(RT_PREEMPT)
  #include <termios.h> // POSIX control def: tcgetattr, cfsetispeed, tcsetattr
#endif

namespace real_time_tools {

/**
 * @brief Simple config class that encapsulate the port parameters for a USB
 * port. This should cover enough paramter to setup the USB port for the
 * imu_3DM_GX3_25, imu_3DM_GX3_45 and the imu_3DM_GX5 in xenomai, rt_preempt
 * and ubuntu (potentially MacOS: non posix).
 */
class PortConfig
{
public:
  bool rts_cts_enabled_;

  bool parity_;
  
  enum StopBits{
    one = 1,
    two = 2
  };
  StopBits stop_bits_;

  bool prepare_size_definition_;

  enum DataBits{
    cs7 = 0,
    cs8 = 1
  };
  DataBits data_bits_;
  
  enum BaudeRate{
    BR_57600 = 0,
    BR_115200,
    // BR_230400,
    // BR_460800,
    // BR_500000,
    // BR_576000,
    BR_921600,
    // BR_1000000,
    // BR_1152000,
    // BR_1500000,
    // BR_2000000,
    // BR_2500000,
    // BR_3000000,
    // BR_3500000,
    // BR_4000000,
  };
  BaudeRate baude_rate_;
};

/**
 * @brief This class has for purpose to interact with devices and files alike
 * as the linux philosophie does. Depending on the current Operating system
 * it uses the available real time APIs.
 */
class UsbStream{

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
   * @param config 
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
   * @brief Read the port or the file.
   * 
   * @param msg is the command sent before this command was executed.
   * @param output Must be of the expected size.
   * @return true 
   * @return false 
   */
  bool read_device(std::vector<uint8_t>& msg, const bool stream_on=true);

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
   * @brief Set the poll mode timeout. The read_device method is blocking until
   * timeout.
   * 
   * @return true success
   * @return false problem occured
   */
  bool set_poll_mode_timeout(double timeout_in_second);

  /**
   * @brief Allow to display a msg.
   * 
   * @param msg 
   * @return std::string the debug string
   */
  static std::string msg_debug_string(const std::vector<uint8_t>& msg);
  
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
   * @brief port configuration from Posix. This class defines some default ways
   * to configure the port. Please inherite from this class to define a
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
};

} // namespace

#endif // HEADER PROTECTION





















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