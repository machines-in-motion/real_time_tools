/**
 * @file demo_usb_stream_imu_3DM_GX3_25.cpp
 * @author Vincent Berenz (vincent.brenz@tuebingen.mpg.de)
 * @brief Testing imu connection directly via the drivers. See test_interface in
 *        the same package for an example of the API.
 * @version 0.1
 * @date 2019-05-09
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "real_time_tools/timer.hpp"
#include "real_time_tools/usb_stream.hpp"

/**
 * @brief Send the message that set the imu into stream mode or not.
 *
 * @param usb_stream is the usb interface.
 * @param stream_mode start or stop the stream mode.
 */
void continuous_mode_on(real_time_tools::UsbStream& usb_stream,
                        bool stream_mode)
{
    std::vector<uint8_t> reply;
    std::vector<uint8_t> command;

    /**
     * Here we set the IMU into a constant broadcasting mode. The broadcasted
     * data are composed with the accelerometer and the gyroscope.
     * https://atlas.is.localnet/confluence/display/AMDW/Microstrain+3DM+IMUs?preview=/8979810/17761244/3DM-GX3-Data-Communications-Protocol.pdf
     */
    command.resize(4);
    command[0] = 0xc4;  // set continuous mode on
    command[1] = 0xc1;  // user confirmation 1
    command[2] = 0x29;  // user confirmation 2
    command[3] =
        0xc2;  // Acceleration and angular rate continuously broadcasted
    reply.resize(8, 0);  // answer in 8 bits.

    rt_printf("The IMU will blink fast\n");
    while (!(reply[0] == 0xC4 && reply[1] == 0xc2))
    {
        usb_stream.write_device(command);
        usb_stream.read_device(reply, stream_mode);
    }
    rt_printf("Device answer is: %s\n",
              real_time_tools::UsbStream::msg_debug_string(reply).c_str());
    rt_printf("The IMU should blink fast\n");
}

/**
 * @brief Check the mode of the imu.
 *
 * @param usb_stream usb communication interface.
 * @param stream_mode read the socket in stream mode or not.
 * @return true imu is in stream mode
 * @return false imu is in idle mode
 */
bool is_continuous_mode_on(real_time_tools::UsbStream& usb_stream,
                           bool stream_mode)
{
    std::vector<uint8_t> reply;
    std::vector<uint8_t> command;

    /**
     * Ask current mode
     * https://atlas.is.localnet/confluence/display/AMDW/Microstrain+3DM+IMUs?preview=/8979810/17761244/3DM-GX3-Data-Communications-Protocol.pdf
     */
    command.resize(4);
    command[0] = 0xd4;   // set continuous mode on
    command[1] = 0xa3;   // user confirmation 1
    command[2] = 0x47;   // user confirmation 2
    command[3] = 0;      // request continuous mode
    reply.resize(4, 0);  // answer in 8 bits.

    bool success = usb_stream.write_device(command);
    success = success && usb_stream.read_device(reply, stream_mode);
    rt_printf("is continuous mode reply: %s\n",
              real_time_tools::UsbStream::msg_debug_string(reply).c_str());
    return success && (reply[1] > 0);
}

/**
 * @brief Set the imu into idle mode
 *
 * @param usb_stream
 * @param stream_mode
 */
void continuous_mode_off(real_time_tools::UsbStream& usb_stream,
                         bool stream_mode)
{
    std::vector<uint8_t> reply;
    std::vector<uint8_t> command;

    /**
     * Here we set the IMU into a constant broadcasting mode. The broadcasted
     * data are composed with the accelerometer and the gyroscope.
     * https://atlas.is.localnet/confluence/display/AMDW/Microstrain+3DM+IMUs?preview=/8979810/17761244/3DM-GX3-Data-Communications-Protocol.pdf
     */
    command.resize(4);
    command[0] = 0xc4;  // set continuous mode on
    command[1] = 0xc1;  // user confirmation 1
    command[2] = 0x29;  // user confirmation 2
    command[3] =
        0x00;  // Acceleration and angular rate continuously broadcasted
    reply.resize(8, 0xFF);  // answer in 8 bits.

    rt_printf("The IMU will blink slowly\n");
    while (!(reply[0] == 0xC4 && reply[1] == 0x00))
    {
        usb_stream.write_device(command);
        usb_stream.read_device(reply, stream_mode);
    }
    rt_printf("Device answer is: %s\n",
              real_time_tools::UsbStream::msg_debug_string(reply).c_str());
    rt_printf("The IMU should blink slowly\n");
}

/**
 * @brief Reset the imu.
 *
 * @param usb_stream
 * @param stream_mode
 */
void reset(real_time_tools::UsbStream& usb_stream, bool stream_mode)
{
    std::vector<uint8_t> reply;
    std::vector<uint8_t> command;

    /**
     * Device reset
     */
    command.resize(3);
    command[0] = 0xfe;   // reset device
    command[1] = 0x9e;   // user confirmation 1
    command[2] = 0x3a;   // user confirmation 2
    reply.resize(0, 0);  // answer in 8 bits.

    rt_printf("The IMU is resetting\n");
    usb_stream.write_device(command);
    usb_stream.read_device(reply, stream_mode);
    rt_printf("Device answer is: %s\n",
              real_time_tools::UsbStream::msg_debug_string(reply).c_str());
    rt_printf("The IMU is reset\n");
    real_time_tools::Timer::sleep_sec(10);
}

/**
 * @brief Example on how to use the usb interface using an imu.
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char** argv)
{
    /**
     * The software input is the path to the port: /dev/tty0
     */
    if (argc != 2)
    {
        printf("usage: demo_device_stream <device>\ne.g. %s /dev/tty0\n",
               argv[0]);
        return -1;
    }

    /**
     * Initialization, the IMU should blink slowly
     */

    // Let us acquire the device path from the application arguments
    std::string device = std::string(argv[1]);

    /** We create a UsbStream object that will allow us to interact with the dev
     * port.
     */
    real_time_tools::UsbStream usb_stream;

    /** If you receive some permission denied, please add yourself in the
     * "dialout" group: sudo usermod -a -G dialout [YOUR_USER_NAME] Ask an admin
     * to do it for you if you do not have the sudo rights.
     */
    if (!usb_stream.open_device(device))
    {
        return -1;
    }

    /** We need to create some port configuration. These configuration are valid
     * for the IMU 3DM-GX3-25 from micro-strain initialization.
     */
    real_time_tools::PortConfig port_config;
    port_config.rts_cts_enabled_ = false;
    port_config.parity_ = false;
    port_config.stop_bits_ = real_time_tools::PortConfig::StopBits::one;
    port_config.prepare_size_definition_ = false;
    port_config.data_bits_ = real_time_tools::PortConfig::cs8;
    port_config.baude_rate_ = 115200;
    usb_stream.set_port_config(port_config);
    usb_stream.set_poll_mode_timeout(0.1);

    // stream mode of the usb port
    bool stream_mode = false;

    real_time_tools::Timer::sleep_sec(1);

    /**
     * send some messages
     */
    continuous_mode_on(usb_stream, stream_mode);
    real_time_tools::Timer::sleep_sec(5);

    usb_stream.flush();
    continuous_mode_off(usb_stream, stream_mode);
    usb_stream.flush();

    rt_printf("Close port\n");
    usb_stream.close_device();

    rt_printf("Stop program\n");
    return 0;
}

/**
 * \example demo_usb_stream_imu_3DM_GX3_25.cpp
 *
 * In order to use this Demo one must have an IMU 3DM-GX3-25 from micro-strain
 * plug in one of the usb port of the computer.
 * https://atlas.is.localnet/confluence/display/AMDW/Microstrain+3DM+IMUs?preview=/8979810/17761244/3DM-GX3-Data-Communications-Protocol.pdf
 *
 * This demos present the use of the usb socket use using the real_time_tools
 * API.
 *
 * One need to create a real_time_tools::UsbStream. This class allows you to
 * open a device, which means that the class connects this process to a
 * usb communication socket.One can initialize the socket parameters through
 * the real_time_tools::PortConfig structure. Once open one can simply use the
 * communication protocole of the hardware to send and receive messages.
 *
 */