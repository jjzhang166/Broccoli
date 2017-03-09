/*
 The MIT License (MIT)

 Copyright (c) 2017 badlyby

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "nrf.h"
#include "nrf_radio.h"

void RTC_config(void)
{
    NRF_CLOCK->LFCLKSRC             = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED  = 0;
    NRF_CLOCK->TASKS_LFCLKSTART     = 1;
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {}
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NVIC_EnableIRQ(RTC0_IRQn);
    NRF_RTC0->PRESCALER     = 7;
    NRF_RTC0->EVTENSET      = RTC_EVTENSET_COMPARE0_Msk;
    NRF_RTC0->INTENSET      = RTC_INTENSET_COMPARE0_Msk;
}

void RTCSleep(uint32_t counter)
{
    NRF_RTC0->TASKS_CLEAR = 1;
    NRF_RTC0->CC[0]         = counter;
    NRF_RTC0->TASKS_START = 1;
    __SEV();
    __WFE();
    __WFE();
}

void RTC0_IRQHandler()
{
    if ((NRF_RTC0->EVENTS_TICK != 0) && 
        ((NRF_RTC0->INTENSET & RTC_INTENSET_TICK_Msk) != 0))
    {
        NRF_RTC0->EVENTS_TICK = 0;
    }
    
    if ((NRF_RTC0->EVENTS_COMPARE[0] != 0) && 
        ((NRF_RTC0->INTENSET & RTC_INTENSET_COMPARE0_Msk) != 0))
    {
        NRF_RTC0->EVENTS_COMPARE[0] = 0;
        Radio_Break();
    }
}
