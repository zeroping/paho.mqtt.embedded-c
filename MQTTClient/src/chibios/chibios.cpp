/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "chibios.h"

#include "ch.h"
#include "hal.h"

#include "lwip/ip_addr.h"
#include "lwip/netdb.h"


#include <stdio.h>
#include <string.h>


//we can define something better here if we need to debug
#define MQTT_PRINTF(...) 


    IPStack::IPStack()
    {

    }
    
	int IPStack::Socket_error(const char* aString)
	{
		if (errno != EINTR && errno != EAGAIN && errno != EINPROGRESS && errno != EWOULDBLOCK)
		{
			if (strcmp(aString, "shutdown") != 0 || (errno != ENOTCONN && errno != ECONNRESET))
			{
				printf("Socket error %s in %s for socket %d\n", strerror(errno), aString, mysock);
			}
		}
		return errno;
	}

    int IPStack::connect(const char* hostname, int port)
    {
		int type = SOCK_STREAM;
		struct sockaddr_in address;
		int rc = -1;
		sa_family_t family = AF_INET;
		struct addrinfo *result = NULL;
		struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

		if ((rc = lwip_getaddrinfo(hostname, NULL, &hints, &result)) == 0)
		{
			struct addrinfo* res = result;

			/* prefer ip4 addresses */
			while (res)
			{
				if (res->ai_family == AF_INET)
				{
					result = res;
					break;
				}
				res = res->ai_next;
			}

			if (result->ai_family == AF_INET)
			{
				address.sin_port = htons(port);
				address.sin_family = family = AF_INET;
				address.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
			}
			else
				rc = -1;

			lwip_freeaddrinfo(result);
		}

		if (rc == 0)
		{
			mysock = lwip_socket(family, type, 0);
			if (mysock != -1)
			{	
				rc = lwip_connect(mysock, (struct sockaddr*)&address, sizeof(address));
			}
		}

        return rc;
    }

    int IPStack::read(unsigned char* buffer, int len, int timeout_ms)
    {

		int ret ;
		
		int opt = timeout_ms;
		ret = lwip_setsockopt(mysock, SOL_SOCKET, SO_RCVTIMEO, &opt, sizeof(int));
		if(ret != 0)
		{
		  MQTT_PRINTF("setsocketopt  ret %d\r\n", ret);
		}

		int optget;
		socklen_t optlen = sizeof(optget);
		ret = lwip_getsockopt(mysock, SOL_SOCKET, SO_RCVTIMEO, &optget, &optlen);
		
		int bytes = 0;
		while (bytes < len)
		{
			int rc = lwip_recv(mysock, &buffer[bytes], (size_t)(len - bytes), 0);
			if (rc <= 0)
			{
				if (Socket_error("read") != 0 || (errno == EAGAIN) || 1)
				{
					bytes = -1;
					break;
				}
			}
			else
				bytes += rc;
		}
		return bytes;
    }
    
    int IPStack::write(unsigned char* buffer, int len, int timeout)
    {
	
	int ret ;
	int opt = timeout;
	
	ret = lwip_setsockopt(mysock, SOL_SOCKET, SO_RCVTIMEO, &opt, sizeof(int));
	if(ret != 0)
	{
	  MQTT_PRINTF( "setsocketopt  ret %d\r\n", ret);
	}
	
	
	int	rc = lwip_write(mysock, buffer, len);
	return rc;
    }

	int IPStack::disconnect()
	{
		return lwip_close(mysock);
	}

Countdown::Countdown()
    { 
	
    }

    Countdown::Countdown(int ms)
    { 
		countdown_ms(ms);
    }
    

    /**
     * @brief Checks if the timer is expired
     * 
     * @return true if expired
     */
    bool Countdown::expired()
    {
      
      int diff = end_time - chVTGetSystemTime();
      return (diff <= 0);

    }
    

    /**
     * @brief Sets counter to now plus the given time
     * 
     * @param ms time in MS
     * @return void
     */
    void Countdown::countdown_ms(int ms)  
    {
		end_time = chVTGetSystemTime() + OSAL_MS2ST(ms);
    }

    
    /**
     * @brief Sets counter to now plus the given time
     * 
     * @param seconds time in seconds
     * @return void
     */
    void Countdown::countdown(int seconds)
    {
	end_time = chVTGetSystemTime() + OSAL_S2ST(seconds);
    }

    
    /**
     * @brief Returns the remining time, or zero if expired
     * 
     * @return int time remaining in milliseconds
     */
    int Countdown::left_ms()
    {
	systime_t diff = end_time - chVTGetSystemTime();
	
	if (diff>0)
	{ 
	  return (diff * 1000 / OSAL_ST_FREQUENCY);
	} else {
	  return 0;
	}
    }
    
