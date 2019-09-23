#include <exception>
#include <algorithm>
#include "real_time_tools/iostream.hpp"
#include "real_time_tools/timer.hpp"
#include "real_time_tools/usb_stream.hpp"

#if defined(XENOMAI)
#elif defined(NON_REAL_TIME) || defined(RT_PREEMPT)
  #include <unistd.h> // UNIX standard function definitions: write, read
  #include <termios.h> // terminal io (serial port) interface
  #include <fcntl.h> // File control definitions: open
  #include <errno.h> // Error number definitions
  #include <string.h> // parse errno message
#endif

namespace real_time_tools {

UsbStream::UsbStream()
{
  // important initialization
  timeout_set_ = false;
  buffer_.resize(100);

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
  file_id_ = open(file_name_.c_str(), O_RDWR | O_NOCTTY);
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
  flush();
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
    config_.c_cflag |= (CRTSCTS | CREAD);
  }else{
    config_.c_cflag |= (CLOCAL | CREAD);
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
  //Convert specified baud to hardware specific value
  int hardware_bit_baud = 0;
  switch (user_config.baude_rate_)
  {
    case 0:
      hardware_bit_baud = B0;
      break;
    case 50:
      hardware_bit_baud = B50;
      break;
    case 75:
      hardware_bit_baud = B75;
      break;
    case 110:
      hardware_bit_baud = B110;
      break;
    case 134:
      hardware_bit_baud = B134;
      break;
    case 150:
      hardware_bit_baud = B150;
      break;
    case 200:
      hardware_bit_baud = B200;
      break;
    case 300:
      hardware_bit_baud = B300;
      break;
    case 600:
      hardware_bit_baud = B600;
      break;
    case 1200:
      hardware_bit_baud = B1200;
      break;
    case 1800:
      hardware_bit_baud = B1800;
      break;
    case 2400:
      hardware_bit_baud = B2400;
      break;
    case 4800:
      hardware_bit_baud = B4800;
      break;
    case 9600:
      hardware_bit_baud = B9600;
      break;
    case 19200:
      hardware_bit_baud = B19200;
      break;
    case 38400:
      hardware_bit_baud = B38400;
      break;
# ifdef B7200
    case 7200:
      hardware_bit_baud = B7200;
      break;
# endif
# ifdef B14400
    case 14400:
      hardware_bit_baud = B14400;
      break;
# endif
# ifdef B57600
    case 57600:
      hardware_bit_baud = B57600;
      break;
# endif
# ifdef B115200
    case 115200:
      hardware_bit_baud = B115200;
      break;
# endif
# ifdef B230400
    case 230400:
      hardware_bit_baud = B230400;
      break;
# endif
# ifdef B460800
    case 460800:
      hardware_bit_baud = B460800;
      break;
# endif
# ifdef B500000
    case 500000:
      hardware_bit_baud = B500000;
      break;
# endif
# ifdef B576000
    case 576000:
      hardware_bit_baud = B576000;
      break;
# endif
# ifdef B921600
    case 921600:
      hardware_bit_baud = B921600;
      break;
# endif
# ifdef B1000000
    case 1000000:
      hardware_bit_baud = B1000000;
      break;
# endif
# ifdef B1152000
    case 1152000:
      hardware_bit_baud = B1152000;
      break;
# endif
# ifdef B2000000
    case 2000000:
      hardware_bit_baud = B2000000;
      break;
# endif
# ifdef B3000000
    case 3000000:
      hardware_bit_baud = B3000000;
      break;
# endif
# ifdef B3500000
    case 3500000:
      hardware_bit_baud = B3500000;
      break;
# endif
# ifdef B4000000
    case 4000000:
      hardware_bit_baud = B4000000;
      break;
  # endif
  //Unsupported baud specified
  default:
    throw std::runtime_error("UsbStream::open_device : Baude rate not yet "
                             "supported, fix the code or correct baude rate");
    break;
  }

  //set the baud rate
  cfsetospeed(&config_, hardware_bit_baud);
  cfsetispeed(&config_, hardware_bit_baud);

  // from the imu drivers...
  //set for non-canonical (raw processing, no echo, etc.)
  config_.c_iflag = IGNPAR; // ignore parity check close_port(int
  config_.c_oflag = 0; // raw output
  config_.c_lflag = 0; // raw input
  //Time-Outs -- won't work with NDELAY option in the call to open
  config_.c_cc[VMIN] = 0;  // block reading until RX x characers. If x = 0, 
  // it is non-blocking.
  config_.c_cc[VTIME] = 1;  // Inter-Character Timer -- i.e. timeout= x*.1 s
  
  if(!flush())
  {
    rt_printf("UsbStream::open_device : Flushing old serial buffer data failed\n");
    return false;
  }

  // set port properties after flushing buffer
  if (tcsetattr(file_id_, TCSANOW, &config_) < 0)
  {
    rt_printf("UsbStream::open_device : Failed to configure port.\n");
    return false;
  }

  if(!flush())
  {
    rt_printf("UsbStream::open_device : Flushing old serial buffer data failed\n");
    return false;
  }
#endif
  return true;
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
  if(file_id_ != 0)
  {
    return_value_ = close(file_id_);
    if (return_value_ != 0)
    {
      int errsv = errno;
      printf("ERROR >> Failed to close port %s with error:\n"
            "\t%s\n", file_name_.c_str(), strerror(errsv));
      return false;
    }
    file_id_ = 0;
  }
#endif
  real_time_tools::Timer::sleep_sec(5);
  return true;
}

bool UsbStream::read_device(std::vector<uint8_t>& msg, const bool stream_on)
{
  // We make sure that the internal buffer is big enough, while avoiding too
  // many resize. Theoretically the default size is good enough.
  if (msg.size() > buffer_.size())
  {
    rt_printf("UsbStream::read_device: Warning internal buffer needs resizing,"
              "This operation is not real-time safe");
    buffer_.resize(10*msg.size());
  }
  // inefficient but safer
  std::fill(buffer_.begin(), buffer_.end(), 0);

#if defined(XENOMAI)
  return_value_ = rt_dev_read(file_id_, buffer_.data(), msg.size());
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
    return_value_ = pselect(file_id_ + 1,
                            &file_id_set_, // writefds
                            nullptr, // readfds
                            nullptr, // exceptfds
                            &timeout_posix_, // timeout
                            nullptr); // sigmask

    // an error occured during the ressource access
    if (return_value_ == -1)
    {
      int errsv = errno;
      rt_printf("UsbStream::read_device: "
                "Failed to access port %s with error\n\t%s\n",
                file_name_.c_str(), strerror(errsv));
      return false;
    }
    // the timeout has expired
    else if (return_value_ == 0)
    {
      int errsv = errno;
      rt_printf("UsbStream::read_device: "
                "Failed to access port %s before timeout with "
                "error\n\t%s\n", file_name_.c_str(), strerror(errsv));
      return false;
    }
    // Nothing wrong happened: access the data.
    else
    {
      return_value_ = read(file_id_, buffer_.data(), msg.size());
    }
  }
  /**
   * Stream Mode
   */
  else
  {
    return_value_ = read(file_id_, buffer_.data(), msg.size());
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
              "Failed to read port %s with error\n\t%s\n",
              file_name_.c_str(), strerror(errsv));
    return false;
  }
  // Timeout failure
  else if (return_value_ != static_cast<ssize_t>(msg.size()))
  {
    rt_printf("UsbStream::read_device: "
              "Failed to read port %s. Requested %ld bytes and "
              "received %ld bytes: %s\n",
              file_name_.c_str(), msg.size(), return_value_,
              msg_debug_string(buffer_, return_value_).c_str());
    return false;
  }
  // Here we copy the message inside the buffer in order to use a bigger memory
  // buffer than the message itself
  std::copy_n(buffer_.begin(), msg.size(), msg.begin());
  return true;
}

bool UsbStream::write_device(const std::vector<uint8_t>& msg)
{
  if (msg.size() > buffer_.size())
  {
    rt_printf("UsbStream::write_device: Warning internal buffer needs resizing,"
              "This operation is not real-time safe");
    buffer_.resize(10*msg.size());
  }
  // inefficient but safer
  std::fill(buffer_.begin(), buffer_.end(), 0);

  // Here we copy the message inside the buffer in order to use a bigger memory
  // buffer than the message itself
  std::copy(msg.begin(), msg.end(), buffer_.begin());

#if defined(XENOMAI)
  return_value_ = rt_dev_write(file_id_, buffer_.data(), msg.size());
#elif defined(RT_PREEMPT) || defined(NON_REAL_TIME)
  return_value_ = write(file_id_, buffer_.data(), msg.size());
#endif

  if (return_value_ < 0)
  {
    int errsv = errno;
    rt_printf("UsbStream::write_device: Failed to write in port %s with "
              "command %s and error\n\t%s\n", file_name_.c_str(),
              msg_debug_string(msg).c_str(), strerror(errsv));
    return false;
  }
  else if (return_value_ != static_cast<ssize_t>(msg.size()))
  {
    rt_printf("UsbStream::write_device: Failed to write in port %s, the "
              "requested amount of bytes is %ld, could only write %ld bytes\n",
              file_name_.c_str(), msg.size(), return_value_);
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

std::string UsbStream::msg_debug_string(const std::vector<uint8_t>& msg,
                                        long int until)
{
  long int msg_size = static_cast<long int>(msg.size());
  if(until < 0){until = msg_size;}
  std::ostringstream cmd_debug_string;
  cmd_debug_string << "[ ";
  for (unsigned i = 0 ; i < std::min(msg_size, until) ; ++i)
  {
    cmd_debug_string << std::hex << std::setfill('0') << std::setw(2)
                    << std::uppercase << (msg[i] & 0xFF) << " ";
  }
  cmd_debug_string << "]";
  return cmd_debug_string.str();
}

bool UsbStream::test_msg_equal(const std::vector<uint8_t>& msg1,
                               const std::vector<uint8_t>& msg2)
{
  if(msg1.size() != msg2.size())
  {
    return false;
  }
  bool test = true;
  for(unsigned i = 0 ; i < msg1.size() ; ++i)
  {
    test = test && (msg1[i] == msg2[i]);
  }
  return test;
}

bool UsbStream::flush(int)
{
#ifdef __XENO__
#else
  // fcntl(file_id_, F_SETFL, 0);
  // return_value_ = fcntl(file_id_, F_SETFL, (O_RDWR | O_NONBLOCK));
  
  // int i = duration_ms;
  // while (--i > 0) {
	//   real_time_tools::Timer::sleep_ms(1.0);
	//   while ((return_value_ = read(file_id_, buffer_.data(), buffer_.size())) > 0)
  //   { // flush buffer and make sure it's cleared for while.
	//     i = 100;
  //   }
  // }
  // fcntl(file_id_, F_SETFL, 0);
  // return_value_ = fcntl(file_id_, F_SETFL, O_RDWR);
  // real_time_tools::Timer::sleep_ms(500);
  
  if(tcflush(file_id_, TCIOFLUSH) == -1)
  {
    printf("flush failed\n");
    return false;
  }
  return true;
#endif
}

} // namespace
