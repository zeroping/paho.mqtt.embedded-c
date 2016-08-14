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

#include "ch.h"
#include "hal.h"


#define sa_family_t u8_t

class IPStack 
{
public:    
    IPStack();
    
	int Socket_error(const char* aString);

    int connect(const char* hostname, int port);

    int read(unsigned char* buffer, int len, int timeout_ms);
    int write(unsigned char* buffer, int len, int timeout);

	int disconnect();
private:

    int mysock; 
    
};


/**
 * @brief Implements a timer
 * 
 */
class Countdown
{
public:
    Countdown();

    Countdown(int ms);
    

    bool expired();
    

    void countdown_ms(int ms);

    
    void countdown(int seconds);

    
    int left_ms();
    
private:
	systime_t end_time;
};

