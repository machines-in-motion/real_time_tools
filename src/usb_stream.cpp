#include <exception>
#include "real_time_tools/iostream.hpp"
#include "real_time_tools/usb_stream.hpp"

#if defined(XENOMAI)
#elif defined(NON_REAL_TIME) || defined(RT_PREEMPT)
  #include <unistd.h> // UNIX standard function definitions: write, read
  #include <fcntl.h> // File control definitions: open
  #include <errno.h> // Error number definitions
  #include <string.h> // parse errno message
#endif

namespace real_time_tools {

UsbStream::UsbStream()
{
  // important initialization
  timeout_set_ = false;

  // some default value
  file_name_ = "";
  file_id_ = 0;
  return_value_ = 0;
  timeout_ = 0.0;

#if defined(XENOMAI)
#elif defined(RT_PREEMPT) || defined(NON_REAL_TIME)
  // config_ : nothing to be done. Initilized upon port openning.
  timeout_posix_.tv_sec = 0;
  timeout_posix_.tv_nsec = 0;
  FD_ZERO(&file_id_set_);
#endif
}

UsbStream::~UsbStream()
{
  close_device();
}

bool UsbStream::open_device(const std::string& file_name)
{
  file_name_ = file_name;
#if defined(XENOMAI)
  // fd_ = rt_dev_open(port_, O_RDWR);
  // if (fd_ < 0)
  // {
  //   rt_printf("ERROR >> Failed to open real-time USB port %s. "
  //             "Are you sure you've loaded the correct drivers?\n", port_);
  //   return false;
  // }
#elif defined(RT_PREEMPT) || defined(NON_REAL_TIME)
  // http://man7.org/linux/man-pages/man2/open.2.html
  // blocking mode by default, unless O_NONBLOCK is passed
  // We open the device in read and write mode: O_RDWR.
  // Whenever we write in the device, the function will be blockant until the
  // device received the message.
  file_id_ = open(file_name_.c_str(), O_RDWR | O_SYNC);
  if (file_id_ < 0)
  {
    // here errno is a POSIX global variable containing the errors of the last
    // POSIX function call.
    int errsv = errno;
    printf("ERROR >> Failed to open device port %s with error:\n \t%s\n",
      file_name_.c_str(), strerror(errsv));
    return false;
  }
#endif
  return true;
}

bool UsbStream::set_port_config(const PortConfig& user_config)
{
#if defined(XENOMAI)
#elif defined(RT_PREEMPT) || defined(NON_REAL_TIME)
  /**
   * Update the port setting using teh POSIX api.
   * https://linux.die.net/man/3/tcgetattr
   */
  // Get the current port settings
  tcgetattr(file_id_, &config_);

  /**
   * set port control modes:
   * CREAD: Enable receiver.
   * CLOCAL: Ignore modem control lines. 
   */
  if(user_config.rts_cts_enabled_)
  {
    config_.c_cflag = CRTSCTS | CREAD;
  }else{
    config_.c_cflag = CLOCAL | CREAD;
  }
  

  // set to 8N1 (eight data bits, no parity bit, one stop bit):
  // here "~XXX" means "no XXXX"
  if(user_config.parity_)
  {
    config_.c_cflag &= PARENB;
  }else{
    config_.c_cflag &= ~PARENB;
  }
  
  if(user_config.stop_bits_ == PortConfig::StopBits::one)
  {
    config_.c_cflag &= ~CSTOPB; // 1 stop bit
  }else{
    config_.c_cflag &= CSTOPB; // 2 stop bit
  }

  if(user_config.prepare_size_definition_)
  {
    config_.c_cflag &= CSIZE;
  }else{
    config_.c_cflag &= ~CSIZE;
  }

  switch (user_config.data_bits_)
  {
  case PortConfig::DataBits::cs7 :
    config_.c_cflag |= CS7;     // define the size 7N1, or 7 data bits
    break;
  case PortConfig::DataBits::cs8 :
    config_.c_cflag |= CS8;     // define the size 8N1, or 8 data bits  
    break;
  
  default:
    config_.c_cflag |= CS8;
    break;
  }
  
  // set to baudrate 115200.
  // https://www.setra.com/blog/what-is-baud-rate-and-what-cable-length-is-required-1
  switch (user_config.baude_rate_)
  {
  case PortConfig::BR_57600 :
    cfsetispeed(&config_, B57600);
    cfsetospeed(&config_, B57600);  
    break;
  case PortConfig::BR_115200 :
    cfsetispeed(&config_, B115200);
    cfsetospeed(&config_, B115200);  
    break;
  case PortConfig::BR_230400 :
    cfsetispeed(&config_, B230400);
    cfsetospeed(&config_, B230400);  
    break;
  case PortConfig::BR_460800 :
    cfsetispeed(&config_, B460800);
    cfsetospeed(&config_, B460800);  
    break;  
  case PortConfig::BR_921600 :
    cfsetispeed(&config_, B921600);
    cfsetospeed(&config_, B921600);  
    break;
  default:
    throw std::runtime_error("UsbStream::open_device : Baude rate not yet "
                             "supported, fix the code or correct baude rate");
    break;
  }
  
  // set port properties after flushing buffer
  if (tcsetattr(file_id_, TCSAFLUSH, &config_) < 0)
  {
    printf("ERROR >> Failed to configure port.\n");
    return false;
  }
#endif
}

bool UsbStream::close_device()
{
#if defined(XENOMAI)
  return_value_ = rt_dev_close(file_id_);
  if (return_value_ != 0)
  {
    printString("ERROR >> Failed to close port.\n");
    return false;
  }
#elif defined(RT_PREEMPT) || defined(NON_REAL_TIME)
  return_value_ = close(file_id_);
  if (return_value_ != 0)
  {
    int errsv = errno;
    printf("ERROR >> Failed to close port %s with error:\n"
           "\t%s\n", file_name_.c_str(), strerror(errsv));
    return false;
  }
#endif
  return true;
}

bool UsbStream::read_device(std::vector<uint8_t>& msg, const bool stream_on)
{
#if defined(XENOMAI)
  return_value_ = rt_dev_read(file_id_, msg.reply_.data(), msg.reply_size());
#elif defined(RT_PREEMPT) || defined(NON_REAL_TIME)
  /**
   * Poll Mode
   */
  if (!stream_on)
  {
    if(!timeout_set_)
    {
      throw std::runtime_error("UsbStream::read_device : Poll mode requested "
                              "but no timeout set. Please use "
                              "UsbStream::set_poll_mode_timeout");
    }
    // here we acquire the port access.
    return_value_ = pselect(file_id_ + 1, &file_id_set_,
                            nullptr, nullptr, &timeout_posix_, nullptr);

    // an error occured during the ressource access
    if (return_value_ == -1)
    {
      int errsv = errno;
      rt_printf("UsbStream::read_device: "
                "Failed to access port %s with command %s and error\n\t%s\n",
                file_name_.c_str(), msg_debug_string(msg).c_str(),
                strerror(errsv));
      return false;
    }
    // the timeout has expired
    else if (return_value_ == 0)
    {
      int errsv = errno;
      rt_printf("UsbStream::read_device: "
                "Failed to access port %s befor timeout with command %s and "
                "error\n\t%s\n",
                file_name_.c_str(), msg_debug_string(msg).c_str(),
                strerror(errsv));
      return false;
    }
    // Nothing wrong happened: access the data.
    else
    {
      return_value_ = read(file_id_, msg.data(), msg.size());
    }
  }
  /**
   * Stream Mode
   */
  else
  {
    return_value_ = read(file_id_, msg.data(), msg.size());
  }
#endif
  /**
   * Check the potential error:
   * 
   * - First we check if the port could be read at all.
   * - Then we check if the port was read before the timeout
   * - Then we check the validity of the message
   */

  // Port reading failure
  if (return_value_ < 0)
  {
    int errsv = errno;
    rt_printf("UsbStream::read_device: "
              "Failed to read port %s with command %s and error\n\t%s\n",
              file_name_.c_str(), msg_debug_string(msg).c_str(),
              strerror(errsv));
    return false;
  }
  // Timeout failure
  else if (return_value_ != msg.size())
  {
    rt_printf("UsbStream::read_device: "
              "Failed to read port %s. Requested %ld bytes and "
              "received %ld bytes: %s\n",
              file_name_.c_str(), msg.size(), return_value_,
              msg_debug_string(msg).c_str());
    return false;
  }
  return true;
}

bool UsbStream::write_device(const std::vector<uint8_t>& msg)
{
#if defined(XENOMAI)
  return_value_ = rt_dev_write(file_id_, msg.data(), msg.size());
#elif defined(RT_PREEMPT) || defined(NON_REAL_TIME)
  return_value_ = write(file_id_, msg.data(), msg.size());
#endif

  if (return_value_ < 0)
  {
    int errsv = errno;
    rt_printf("ERROR >> Failed to read port %s with command %s "
              "and error\n\t%s\n", file_name_.c_str(),
              msg_debug_string(msg).c_str(), strerror(errsv));
    return false;
  }
  else if (return_value_ != msg.size())
  {
    rt_printf("ERROR >> Failed writing requested amount of %ld bytes\n",
              msg.size());
    return false;
  }

  return true;
}

bool UsbStream::set_poll_mode_timeout(double timeout_in_second)
{
#ifdef __XENO__
  // Set read timeout
  rt_config_.config_mask = RTSER_SET_TIMEOUT_RX | RTSER_SET_BAUD;
  rt_config_.rx_timeout = (nanosecs_rel_t)(timeout * 1000000000); // rx_timeout in ns
  rt_config_.baud_rate = 921600;
  res_ = rt_dev_ioctl(fd_, RTSER_RTIOC_SET_CONFIG, &rt_config_);
  if (res_ != 0)
  {
    rt_printf("ERROR >> Failed to set read timeout.\n");
    return false;
  }
#else
  FD_ZERO(&file_id_set_);
  FD_SET(file_id_, &file_id_set_);
  long int tv_sec = timeout_in_second;
  long int tv_nsec = (timeout_in_second - tv_sec) * 1000000000 /* 1e9 */ ;
  timeout_posix_.tv_sec = tv_sec;
  timeout_posix_.tv_nsec = tv_nsec;
#endif
  timeout_ = timeout_in_second;
  timeout_set_ = true;
  return true;
}

std::string UsbStream::msg_debug_string(const std::vector<uint8_t>& msg)
{
  std::ostringstream cmd_debug_string;
  cmd_debug_string << "[ ";
  for (unsigned i=0 ; i<msg.size() ; ++i)
  {
    cmd_debug_string << std::hex << std::setfill('0') << std::setw(2)
                    << std::uppercase << (msg[i] & 0xFF) << " ";
  }
  cmd_debug_string << "]";
  return cmd_debug_string.str();
}

bool UsbStream::flush()
{
#ifdef __XENO__
#else
  // sleep(2); //required to make flush work, for some reason
  return_value_ = tcflush(file_id_, TCIOFLUSH);
  if (return_value_ < 0)
  {
    int errsv = errno;
    rt_printf("ERROR >> Failed to flush port %s"
              "and error\n\t%s\n", file_name_.c_str(), strerror(errsv));
    return false;
  }
#endif
}

} // namespace
