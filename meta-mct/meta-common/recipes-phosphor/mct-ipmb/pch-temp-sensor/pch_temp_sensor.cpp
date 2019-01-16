/* Copyright 2018 Intel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <fcntl.h>
#include <inttypes.h>
#include <poll.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/timerfd.h>
#include <sys/stat.h>
#include <dirent.h>

#include <algorithm>
#include <vector>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/bus/match.hpp>
#include <sdbusplus/message.hpp>
#include <sdbusplus/vtable.hpp>
#include <sdbusplus/server/interface.hpp>
#include <phosphor-logging/log.hpp>
#include <iostream>
#include <getopt.h>

#define BUS_FD 0
#define TIMER_FD 1
#define TOTAL_FDS 2

constexpr const char *DBUS_NAME = "xyz.openbmc_project.Ipmi.Ipmb.Pchtemp";
constexpr const char *OBJ_NAME = "/xyz/openbmc_project/Ipmi/Ipmb/Pchtemp";
constexpr const char *IPMB_DBUS_INTF = "org.openbmc.Ipmb";

constexpr const char *IPMBBridged_BUS = "xyz.openbmc_project.Ipmi.Channel.Ipmb";
constexpr const char *IPMBBridged_OBJ = "/xyz/openbmc_project/Ipmi/Channel/Ipmb";
constexpr size_t ipmbMaxDataSize = 256;

sdbusplus::bus::bus bus = sdbusplus::bus::new_system();

static int CreateFile(const char *path, const char *dataToWrite){
	int fd = open(path, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC);

	if(fd < 0) return -1;

	if(dataToWrite != NULL)
		write(fd, dataToWrite, strlen(dataToWrite));

	close(fd);
}

static int createHWmonFS(){

	uint8_t ToCreateFiles = 0;
	
	DIR *dir = opendir("/run/vsensor");
	if(dir == NULL){ 		
		if(mkdir("/run/vsensor", 0755) == -1) return -1;
	}
	else closedir(dir);

	dir = opendir("/run/vsensor/pch-temp");
	if(dir == NULL){
		if(mkdir("/run/vsensor/pch-temp", 0755) == -1) return -1;
		
		ToCreateFiles = 1;
	}
	else closedir(dir);

	if(ToCreateFiles){
		CreateFile("/run/vsensor/pch-temp/device", NULL);
		CreateFile("/run/vsensor/pch-temp/name", "pch_temp\n");
		CreateFile("/run/vsensor/pch-temp/of_node", NULL);
		CreateFile("/run/vsensor/pch-temp/subsystem", NULL);
		CreateFile("/run/vsensor/pch-temp/temp1_input", "40000\n");
		CreateFile("/run/vsensor/pch-temp/uevent", "OF_NAME=pch_temp\nOF_FULLNAME=/pch-temp\nOF_COMPATIBLE_0=pch-temp\nOF_COMPATIBLE_N=1\n");
	}		
	
	return 0;
}

static int method_process_Response(sd_bus_message *bus_msg, void *userdata,
							   sd_bus_error *ret_error)
{
    uint8_t netfn = 0, lun = 0, seq = 0, cmd = 0, cc = 0;
	static uint8_t firstCall = 1;
    std::vector<uint8_t> dataReceived;
	int input_fd = -1;
	char reading_str[7]={0};

    dataReceived.reserve(ipmbMaxDataSize);
    auto mesg = sdbusplus::message::message(bus_msg);

	mesg.read(seq, netfn, lun, cmd, cc, dataReceived);

#if 0	
	try{
		mesg.read(dataReceived);		
	}
	catch (const std::exception &e){
		sd_journal_print(LOG_INFO, "No any array data from DBUS");
		printf("No any array data from DBUS\n");
	}

    if (dataReceived.size() > ipmbMaxDataSize)
    {
        return 1;
    }
#endif 

#if 0
	{
		char buf[200] = {0};
		int i=0;
		
		sprintf(buf, "%s", "Response Data: ");
		
		for(;i<dataReceived.size();i++){
			if(strlen(buf) >= sizeof(buf) - 1-3) break;

			sprintf(buf, "%s%02x ", buf, dataReceived.data()[i]);
		}

		sd_journal_print(LOG_INFO,"%s" , buf);
		printf("%s\n", buf);
	}
#endif

	if(firstCall){
		firstCall = 0;
		createHWmonFS();
	}

	if(cc == 0x00){//Completion code = 0x00
		snprintf(reading_str, 6, "%d", dataReceived.data()[0]*1000);
		
		input_fd = open("/run/vsensor/pch-temp/temp1_input", O_TRUNC | O_WRONLY);

		if(input_fd < 0) return -1;
		
		write(input_fd, reading_str, strlen(reading_str));
		write(input_fd, "\n", 1);
		
		close(input_fd);
	}
	
	return 0;
}
	
static const sdbusplus::vtable::vtable_t ipmbVtable[] = {
    sdbusplus::vtable::start(),
    sdbusplus::vtable::method("returnResponse", "yyyyyay", "", method_process_Response),
    sdbusplus::vtable::end()};

/**
 * @brief Main
 */
int main(int argc, char *argv[])
{	
	int r = -1, doonce = 1;
	uint8_t netFn = 0x04;
   	uint8_t rqLun = 0x00;
  	uint8_t seq = 0x00;
    	uint8_t cmd = 0x2d;	
	struct pollfd poll_fds[TOTAL_FDS];
	std::vector<uint8_t> data;
	struct itimerspec iTimerpec;

	data.push_back(0x08);

	//Jeannie add for dynamic devicename
	int opt;
	char ipmbDeviceName[20]={0};
        static const struct option long_options[] = {
        {"d", required_argument, NULL, 'd'},
        {0, 0, 0, 0}};
	
	opt = getopt_long(argc, argv, "", long_options, NULL);
	snprintf(ipmbDeviceName, sizeof(ipmbDeviceName), "%s", optarg);
	//sd_journal_print(LOG_ERR,"Jeannie test!!! ipmbDeviceName=%s!!!",ipmbDeviceName);
	
	sdbusplus::server::interface::interface ipmiInterface(
			bus, OBJ_NAME, IPMB_DBUS_INTF, ipmbVtable, nullptr);
	
	bus.request_name(DBUS_NAME);
	poll_fds[BUS_FD].fd = bus.get_fd();
	if (poll_fds[BUS_FD].fd < 0)
    {
        sd_journal_print(LOG_ERR,"Error getting fd from bus object");
        return -1;
    }

	poll_fds[TIMER_FD].fd = timerfd_create(CLOCK_MONOTONIC, 0);

	if (poll_fds[TIMER_FD].fd < 0)
    {
    	sd_journal_print(LOG_ERR,"Error creating Timer");
        return -1;
    }	

	iTimerpec.it_value.tv_sec = 2;
	iTimerpec.it_value.tv_nsec = 0;
	iTimerpec.it_interval.tv_sec = 0;
	iTimerpec.it_interval.tv_nsec = 0;
	
	r = timerfd_settime(poll_fds[TIMER_FD].fd, 0, &iTimerpec, NULL);

	if (r < 0) {
		sd_journal_print(LOG_ERR, "timerfd_settime error, Error:[%d:%s]", errno, strerror(errno));
		close(poll_fds[TIMER_FD].fd);
		return -1;
	}
	
	poll_fds[BUS_FD].events = POLLIN;
	poll_fds[TIMER_FD].events = POLLIN;	

	while(1){
		
		r = poll(poll_fds, TOTAL_FDS, -1);

        if (r < 0)
        {
            continue;
        }

        // received dbus event
        if (poll_fds[BUS_FD].revents & POLLIN)
        {        
            bus.process_discard();
        }

		if (poll_fds[TIMER_FD].revents & POLLIN)
        {
			sdbusplus::message::message mesg =
	        bus.new_signal(OBJ_NAME, IPMB_DBUS_INTF, "sendRequest");
		// mesg.append("spsfw10", seq, netFn, rqLun, cmd, data);
		mesg.append(ipmbDeviceName, seq, netFn, rqLun, cmd, data); //Jeannie Modified to dynamic devicename
	    	mesg.signal_send();

			iTimerpec.it_value.tv_sec = 2;
			iTimerpec.it_value.tv_nsec = 0;
			iTimerpec.it_interval.tv_sec = 0;
			iTimerpec.it_interval.tv_nsec = 0;
			
			r = timerfd_settime(poll_fds[TIMER_FD].fd, 0, &iTimerpec, NULL);

			if (r < 0) {
				sd_journal_print(LOG_ERR, "timerfd_settime error, Error:[%d:%s]", errno, strerror(errno));
				close(poll_fds[TIMER_FD].fd);
				return -1;
			}
		 }
	}
		
    return 0;
}
