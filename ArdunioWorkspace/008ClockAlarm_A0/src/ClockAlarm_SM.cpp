/*$file${AOs::../src::ClockAlarm_SM.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: ClockAlarm.qm
* File:  ${AOs::../src::ClockAlarm_SM.cpp}
*
* This code has been generated by QM 5.2.1 <www.state-machine.com/qm>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This generated code is open source software: you can redistribute it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation.
*
* This code is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* NOTE:
* Alternatively, this generated code may be distributed under the terms
* of Quantum Leaps commercial licenses, which expressly supersede the GNU
* General Public License and are specifically designed for licensees
* interested in retaining the proprietary status of their code.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*/
/*$endhead${AOs::../src::ClockAlarm_SM.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include <Arduino.h>
#include "qpn.h"
#include "lcd.h"
#include "alarm.h"
#include "ClockAlarm_SM.h"
#include "main.h"


//prototypes of helper functions
String get_am_or_pm(uint32_t time24h);
void display_write(String str_, uint8_t r, uint8_t c);
String integertime_to_string(uint32_t time_);
uint32_t convert_12hformat_to_24h(uint32_t time12h, time_format_t ampm);
uint32_t convert_24hformat_to_12h(uint32_t time24h);
void display_cursor_on_blinkon(void);
void display_cursor_off_blinkoff();
void display_set_cursor(uint8_t r, uint8_t c);
void display_clear(void);
bool is_time_set_error(uint32_t time_, time_format_t format);
void display_erase_block(uint8_t row,uint8_t col_start,uint8_t col_stop);

/*$declare${AOs::Clock_Alarm} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Clock_Alarm} ......................................................*/
typedef struct Clock_Alarm {
/* protected: */
    QActive super;

/* private: */
    uint32_t temp_time;
    uint8_t time_mode;
    uint8_t temp_format;
    uint8_t temp_digit;
    uint8_t curr_setting;
    Alarm alarm;
    uint8_t timeout;

/* private state histories */
    QStateHandler hist_Clock;
} Clock_Alarm;

/* public: */
static uint32_t Clock_Alarm_get_curr_time(void);
static void Clock_Alarm_update_curr_time(void);
static void Clock_Alarm_set_curr_time(uint32_t new_curr_time);
static void Clock_Alarm_display_curr_time(Clock_Alarm * const me,
    uint8_t row,
    uint8_t col);
static void Clock_Alarm_display_clock_setting_time(Clock_Alarm * const me,
    uint8_t row,
    uint8_t col);
extern uint32_t Clock_Alarm_curr_time;
extern Clock_Alarm Clock_Alarm_obj;

/* protected: */
static QState Clock_Alarm_initial(Clock_Alarm * const me);
static QState Clock_Alarm_Clock(Clock_Alarm * const me);
static QState Clock_Alarm_Ticking(Clock_Alarm * const me);
static QState Clock_Alarm_Settings(Clock_Alarm * const me);
static QState Clock_Alarm_hd2(Clock_Alarm * const me);
static QState Clock_Alarm_md1(Clock_Alarm * const me);
static QState Clock_Alarm_md2(Clock_Alarm * const me);
static QState Clock_Alarm_sd2(Clock_Alarm * const me);
static QState Clock_Alarm_clock_format(Clock_Alarm * const me);
static QState Clock_Alarm_setting_error(Clock_Alarm * const me);
static QState Clock_Alarm_err_off(Clock_Alarm * const me);
static QState Clock_Alarm_err_on(Clock_Alarm * const me);
static QState Clock_Alarm_hd1(Clock_Alarm * const me);
static QState Clock_Alarm_sd1(Clock_Alarm * const me);
static QState Clock_Alarm_alarm_on_off(Clock_Alarm * const me);
static QState Clock_Alarm_Alarm_Notify(Clock_Alarm * const me);
static QState Clock_Alarm_alarm_msg_on(Clock_Alarm * const me);
static QState Clock_Alarm_alarm_msg_off(Clock_Alarm * const me);
/*$enddecl${AOs::Clock_Alarm} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/* Check for the minimum required QP version */
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpn version 6.9.0 or higher required
#endif
/*$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$define${AOs::AO_ClockAlarm} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::AO_ClockAlarm} ....................................................*/
QActive *const AO_ClockAlarm = &Clock_Alarm_obj.super;
/*$enddef${AOs::AO_ClockAlarm} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$define${AOs::Clock_Alarm_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Clock_Alarm_ctor} .................................................*/
void Clock_Alarm_ctor(void) {
    Alarm_ctor(&Clock_Alarm_obj.alarm);
    QActive_ctor(AO_ClockAlarm,Q_STATE_CAST(&Clock_Alarm_initial));
}
/*$enddef${AOs::Clock_Alarm_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$define${AOs::Clock_Alarm} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Clock_Alarm} ......................................................*/
uint32_t Clock_Alarm_curr_time;
Clock_Alarm Clock_Alarm_obj;

/*${AOs::Clock_Alarm::get_curr_time} .......................................*/
static uint32_t Clock_Alarm_get_curr_time(void) {
    uint8_t saved_sreg = SREG;
    uint32_t temp;
    cli();
    temp = Clock_Alarm_curr_time;
    SREG = saved_sreg;
    return temp;
}

/*${AOs::Clock_Alarm::update_curr_time} ....................................*/
static void Clock_Alarm_update_curr_time(void) {
    if(++Clock_Alarm_curr_time == MAX_TIME){
        Clock_Alarm_curr_time = 0;
    }
}

/*${AOs::Clock_Alarm::set_curr_time} .......................................*/
static void Clock_Alarm_set_curr_time(uint32_t new_curr_time) {
    uint8_t save_sreg;
    save_sreg = SREG;
    cli();
    TCCR1B &= ~(0x7U); //Stop the TIMER1
    TCNT1 = 0U;
    Clock_Alarm_curr_time = new_curr_time;
    TCCR1B = (TCCR1B_CTC_MODE |TCCR1B_PRESCALER_1);
    SREG = save_sreg;
}

/*${AOs::Clock_Alarm::display_curr_time} ...................................*/
static void Clock_Alarm_display_curr_time(Clock_Alarm * const me,
    uint8_t row,
    uint8_t col)
{
    String time_as_string;
    uint32_t time_;

    uint32_t time24h = Clock_Alarm_get_curr_time();
    uint8_t ss = time24h % 10U;       //extract sub-second to append later
    time24h /= 10; //convert to number of seconds

    time_ = (me->time_mode == MODE_24H)?time24h:convert_24hformat_to_12h(time24h);
    time_as_string = integertime_to_string(time_); //hh:mm:ss
    time_as_string.concat('.');
    time_as_string.concat(ss);

    /*if mode is 12H , concatenate  am/pm information */
    if(me->time_mode == MODE_12H){
        time_as_string.concat(' ');
        time_as_string.concat(get_am_or_pm(time24h));
    }

    display_write(time_as_string,row,col);
}

/*${AOs::Clock_Alarm::display_clock_setting_time} ..........................*/
static void Clock_Alarm_display_clock_setting_time(Clock_Alarm * const me,
    uint8_t row,
    uint8_t col)
{
    String time_as_string;

    time_as_string = integertime_to_string(me->temp_time); //hh:mm:ss

    /*concatenate  am/pm information */
    if(me->temp_format != FORMAT_24H){
        time_as_string.concat(' ');
        if(me->temp_format == FORMAT_AM)
            time_as_string.concat("AM");
        else
            time_as_string.concat("PM");
    }

    display_write(time_as_string,row,col);
}

/*${AOs::Clock_Alarm::SM} ..................................................*/
static QState Clock_Alarm_initial(Clock_Alarm * const me) {
    /*${AOs::Clock_Alarm::SM::initial} */
    Alarm_init(&me->alarm);
    Clock_Alarm_set_curr_time(INITIAL_CURR_TIME);
    me->time_mode = MODE_12H;
    /* state history attributes */
    /* state history attributes */
    me->hist_Clock = Q_STATE_CAST(&Clock_Alarm_Ticking);
    return Q_TRAN(&Clock_Alarm_Clock);
}

/*${AOs::Clock_Alarm::SM::Clock} ...........................................*/
static QState Clock_Alarm_Clock(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock} */
        case Q_EXIT_SIG: {
            /* save deep history */
            me->hist_Clock = QHsm_state(me);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::initial} */
        case Q_INIT_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_Ticking);
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::TICK} */
        case TICK_SIG: {
            Q_SIG(&me->alarm) = ALARM_CHECK_SIG;
            Q_PAR(&me->alarm) =  Clock_Alarm_get_curr_time()/10;
            Alarm_dispatch(&me->alarm);
            if(me->curr_setting == NO_SETTING)
                Clock_Alarm_display_curr_time(me,TICKING_CURR_TIME_ROW,TICKING_CURR_TIME_COL);
            else if(me->curr_setting == ALARM_SETTING){
                Clock_Alarm_display_curr_time(me,ALARM_SETTING_CURR_TIME_ROW,ALARM_SETTING_CURR_TIME_COL);
                if( QHsm_state(me) != Q_STATE_CAST(&Clock_Alarm_alarm_on_off))
                    Clock_Alarm_display_clock_setting_time(me,CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_COL);

                if( QHsm_state(me) == Q_STATE_CAST(&Clock_Alarm_hd1))
                    display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_HOUR_D1_COL);
                else if( QHsm_state(me) == Q_STATE_CAST(&Clock_Alarm_hd2))
                    display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_HOUR_D2_COL);
                else if( QHsm_state(me) == Q_STATE_CAST(&Clock_Alarm_md1))
                    display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_MIN_D1_COL);
                else if( QHsm_state(me) == Q_STATE_CAST(&Clock_Alarm_md2))
                    display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_MIN_D2_COL);
                else if( QHsm_state(me) == Q_STATE_CAST(&Clock_Alarm_sd1))
                    display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_SEC_D1_COL);
                else if( QHsm_state(me) == Q_STATE_CAST(&Clock_Alarm_sd2))
                    display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_SEC_D2_COL);
                else if( QHsm_state(me) == Q_STATE_CAST(&Clock_Alarm_clock_format))
                    display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_FMT_COL);
                else if( QHsm_state(me) == Q_STATE_CAST(&Clock_Alarm_alarm_on_off))
                     display_set_cursor(CLOCK_SETTING_TIME_ROW,ALARM_SETTING_STATUS_COL);
            }
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::ALARM} */
        case ALARM_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_Alarm_Notify);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Ticking} ..................................*/
static QState Clock_Alarm_Ticking(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Ticking} */
        case Q_ENTRY_SIG: {
            Clock_Alarm_display_curr_time(me,TICKING_CURR_TIME_ROW,TICKING_CURR_TIME_COL);
            me->curr_setting = NO_SETTING;
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Ticking} */
        case Q_EXIT_SIG: {
            display_clear();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Ticking::SET} */
        case SET_SIG: {
            me->curr_setting =CLOCK_SETTING;
            me->temp_time = Clock_Alarm_get_curr_time()/10;
            if(me->time_mode == MODE_12H){
                if(get_am_or_pm(me->temp_time).equals("AM")){
                    me->temp_format = FORMAT_AM;
                }
                else{
                    me->temp_format = FORMAT_PM;
                }
                me->temp_time = convert_24hformat_to_12h(me->temp_time);
            }
            else
                me->temp_format = FORMAT_24H;
            status_ = Q_TRAN(&Clock_Alarm_Settings);
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Ticking::OK} */
        case OK_SIG: {
            me->curr_setting =ALARM_SETTING;
            me->temp_time = Alarm_get_alarm_time(&me->alarm);
            if(me->time_mode == MODE_12H){
                if(get_am_or_pm(me->temp_time).equals("AM")){
                    me->temp_format = FORMAT_AM;
                }
                else{
                    me->temp_format = FORMAT_PM;
                }
                me->temp_time = convert_24hformat_to_12h(me->temp_time);
            }
            else
                me->temp_format = FORMAT_24H;
            status_ = Q_TRAN(&Clock_Alarm_Settings);
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Clock);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Settings} .................................*/
static QState Clock_Alarm_Settings(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Settings} */
        case Q_ENTRY_SIG: {
            Clock_Alarm_display_clock_setting_time(me,CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_COL);
            display_cursor_on_blinkon();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings} */
        case Q_EXIT_SIG: {
            display_clear();
            display_cursor_off_blinkoff();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::initial} */
        case Q_INIT_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_hd1);
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::ABRT} */
        case ABRT_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_Ticking);
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::OK} */
        case OK_SIG: {
            if(me->curr_setting == CLOCK_SETTING){
                if(me->temp_format != FORMAT_24H){
                    me->temp_time = convert_12hformat_to_24h( me->temp_time,(time_format_t)me->temp_format);
                    me->time_mode = MODE_12H;
                }else{
                    me->time_mode = MODE_24H;
                }
                me->temp_time *= 10UL;
                Clock_Alarm_set_curr_time(me->temp_time);
            }else if(me->curr_setting == ALARM_SETTING){
               //me->alarm_status = me->temp_digit;
                if(me->temp_format != FORMAT_24H){
                    me->temp_time = convert_12hformat_to_24h( me->temp_time,(time_format_t)me->temp_format);
                }
                Alarm_set_alarm_time(&me->alarm,me->temp_time);
                Alarm_set_status(&me->alarm,me->temp_digit);
            }
            status_ = Q_TRAN(&Clock_Alarm_Ticking);
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Clock);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Settings::hd2} ............................*/
static QState Clock_Alarm_hd2(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Settings::hd2} */
        case Q_ENTRY_SIG: {
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_HOUR_D2_COL);
            me->temp_digit =  DIGIT2(GET_HOUR(me->temp_time));
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::hd2::SET} */
        case SET_SIG: {
            ++me->temp_digit;
            me->temp_digit %= 10;
            me->temp_time -=  DIGIT2(GET_HOUR(me->temp_time))  * 3600UL;
            me->temp_time += (me->temp_digit) * 3600UL;
            Clock_Alarm_display_clock_setting_time(me,CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_COL);
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_HOUR_D2_COL);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::hd2::OK} */
        case OK_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_md1);
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Settings);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Settings::md1} ............................*/
static QState Clock_Alarm_md1(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Settings::md1} */
        case Q_ENTRY_SIG: {
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_MIN_D1_COL);
            me->temp_digit =  DIGIT1(GET_MIN(me->temp_time));
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::md1::SET} */
        case SET_SIG: {
            ++me->temp_digit;
            me->temp_digit %= 6;
            me->temp_time -=  DIGIT1(GET_MIN(me->temp_time)) *10UL * 60UL;
            me->temp_time += (me->temp_digit) * 10UL * 60UL;
            Clock_Alarm_display_clock_setting_time(me,CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_COL);
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_MIN_D1_COL);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::md1::OK} */
        case OK_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_md2);
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Settings);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Settings::md2} ............................*/
static QState Clock_Alarm_md2(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Settings::md2} */
        case Q_ENTRY_SIG: {
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_MIN_D2_COL);
            me->temp_digit =  DIGIT2(GET_MIN(me->temp_time));
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::md2::SET} */
        case SET_SIG: {
            ++me->temp_digit;
            me->temp_digit %= 10;
            me->temp_time -=  DIGIT2(GET_MIN(me->temp_time))  * 60UL;
            me->temp_time += (me->temp_digit) * 60UL;
            Clock_Alarm_display_clock_setting_time(me,CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_COL);
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_MIN_D2_COL);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::md2::OK} */
        case OK_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_sd1);
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Settings);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Settings::sd2} ............................*/
static QState Clock_Alarm_sd2(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Settings::sd2} */
        case Q_ENTRY_SIG: {
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_SEC_D2_COL);
            me->temp_digit =  DIGIT2(GET_SEC(me->temp_time));
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::sd2::SET} */
        case SET_SIG: {
            ++me->temp_digit;
            me->temp_digit %= 10;
            me->temp_time -=  DIGIT2(GET_SEC(me->temp_time));
            me->temp_time +=  me->temp_digit;
            Clock_Alarm_display_clock_setting_time(me,CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_COL);
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_SEC_D2_COL);

            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::sd2::OK} */
        case OK_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_clock_format);
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Settings);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Settings::clock_format} ...................*/
static QState Clock_Alarm_clock_format(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Settings::clock_format} */
        case Q_ENTRY_SIG: {
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_FMT_COL);
            String msg[3] = {"24H","AM ","PM "};
            display_write(msg[me->temp_format],CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_FMT_COL);
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_FMT_COL);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::clock_format::SET} */
        case SET_SIG: {
            String msg;
            if(me->temp_format == FORMAT_24H){
                me->temp_format = FORMAT_AM;
                msg = "AM ";
            }
            else if(me->temp_format == FORMAT_AM){
                me->temp_format = FORMAT_PM;
                msg = "PM ";
            }
            else if (me->temp_format == FORMAT_PM){
                me->temp_format = FORMAT_24H;
                msg = "24H";
            }
            display_write(msg,CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_FMT_COL);
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_FMT_COL);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::clock_format::OK} */
        case OK_SIG: {
            /*${AOs::Clock_Alarm::SM::Clock::Settings::clock_format::OK::[is_time_set_error(me->temp_time~} */
            if (is_time_set_error(me->temp_time,(time_format_t)me->temp_format)) {
                status_ = Q_TRAN(&Clock_Alarm_setting_error);
            }
            /*${AOs::Clock_Alarm::SM::Clock::Settings::clock_format::OK::[else]} */
            else {
                /*${AOs::Clock_Alarm::SM::Clock::Settings::clock_format::OK::[else]::[me->curr_setting==ALARM_SETTING~} */
                if (me->curr_setting == ALARM_SETTING) {
                    status_ = Q_TRAN(&Clock_Alarm_alarm_on_off);
                }
                else {
                    status_ = Q_UNHANDLED();
                }
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Settings);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error} ..................*/
static QState Clock_Alarm_setting_error(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error} */
        case Q_ENTRY_SIG: {
            display_cursor_off_blinkoff();
            display_erase_block(CLOCK_SETTING_ERR_MSG_ROW,0,15);
            QActive_armX(AO_ClockAlarm,0,MS_TO_TICKS(500),MS_TO_TICKS(500));
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error} */
        case Q_EXIT_SIG: {
            display_erase_block(CLOCK_SETTING_ERR_MSG_ROW,CLOCK_SETTING_ERR_MSG_COL,CLOCK_SETTING_ERR_MSG_COL_END);
            QActive_disarmX(AO_ClockAlarm,0);

            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error::initial} */
        case Q_INIT_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_err_on);
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error::SET} */
        case SET_SIG: {
            display_cursor_on_blinkon();
            status_ = Q_TRAN(&Clock_Alarm_Settings);
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error::OK} */
        case OK_SIG: {
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error::TICK} */
        case TICK_SIG: {
            Q_SIG(&me->alarm) = ALARM_CHECK_SIG;
            Q_PAR(&me->alarm) =  Clock_Alarm_get_curr_time()/10;
            Alarm_dispatch(&me->alarm);
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Settings);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error::err_off} .........*/
static QState Clock_Alarm_err_off(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error::err_off} */
        case Q_ENTRY_SIG: {
            display_erase_block(CLOCK_SETTING_ERR_MSG_ROW,CLOCK_SETTING_ERR_MSG_COL,CLOCK_SETTING_ERR_MSG_COL_END);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error::err_off::Q_TIMEOUT} */
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_err_on);
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_setting_error);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error::err_on} ..........*/
static QState Clock_Alarm_err_on(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error::err_on} */
        case Q_ENTRY_SIG: {
            display_write("Error!",CLOCK_SETTING_ERR_MSG_ROW,CLOCK_SETTING_ERR_MSG_COL);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::setting_error::err_on::Q_TIMEOUT} */
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_err_off);
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_setting_error);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Settings::hd1} ............................*/
static QState Clock_Alarm_hd1(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Settings::hd1} */
        case Q_ENTRY_SIG: {
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_HOUR_D1_COL);
            me->temp_digit =  DIGIT1(GET_HOUR(me->temp_time));
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::hd1::SET} */
        case SET_SIG: {
            ++me->temp_digit;
            me->temp_digit %= 3;
            me->temp_time -=  DIGIT1(GET_HOUR(me->temp_time)) * 10UL * 3600UL;
            me->temp_time +=  me->temp_digit * 10UL  * 3600UL;
            Clock_Alarm_display_clock_setting_time(me,CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_COL);
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_HOUR_D1_COL);

            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::hd1::OK} */
        case OK_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_hd2);
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Settings);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Settings::sd1} ............................*/
static QState Clock_Alarm_sd1(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Settings::sd1} */
        case Q_ENTRY_SIG: {
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_SEC_D1_COL);
            me->temp_digit =  DIGIT1(GET_SEC(me->temp_time));
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::sd1::SET} */
        case SET_SIG: {
            ++me->temp_digit;
            me->temp_digit %= 6;
            me->temp_time -=  DIGIT1(GET_SEC(me->temp_time)) * 10UL;
            me->temp_time +=  me->temp_digit *  10UL;
            Clock_Alarm_display_clock_setting_time(me,CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_COL);
            display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_SEC_D1_COL);

            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::sd1::OK} */
        case OK_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_sd2);
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Settings);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Clock::Settings::alarm_on_off} ...................*/
static QState Clock_Alarm_alarm_on_off(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Clock::Settings::alarm_on_off} */
        case Q_ENTRY_SIG: {
            display_erase_block(ALARM_SETTING_STATUS_ROW,0,15);
            me->temp_digit = 0U;
            display_write("ALARM OFF",ALARM_SETTING_STATUS_ROW,ALARM_SETTING_STATUS_COL);
            display_set_cursor(ALARM_SETTING_STATUS_ROW,ALARM_SETTING_STATUS_COL);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Clock::Settings::alarm_on_off::SET} */
        case SET_SIG: {
            if(me->temp_digit){
                display_write("ALARM OFF",ALARM_SETTING_STATUS_ROW,ALARM_SETTING_STATUS_COL);
                me->temp_digit = 0;
            }else{
                display_write("ALARM ON ",ALARM_SETTING_STATUS_ROW,ALARM_SETTING_STATUS_COL);
                me->temp_digit = 1;
            }
            display_set_cursor(ALARM_SETTING_STATUS_ROW,ALARM_SETTING_STATUS_COL);
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Settings);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Alarm_Notify} ....................................*/
static QState Clock_Alarm_Alarm_Notify(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Alarm_Notify} */
        case Q_ENTRY_SIG: {
            me->timeout = 20;
            QActive_armX(AO_ClockAlarm,0,MS_TO_TICKS(500),MS_TO_TICKS(500));
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Alarm_Notify} */
        case Q_EXIT_SIG: {
            display_clear();
            QActive_disarmX(AO_ClockAlarm,0);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Alarm_Notify::initial} */
        case Q_INIT_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_alarm_msg_on);
            break;
        }
        /*${AOs::Clock_Alarm::SM::Alarm_Notify::OK} */
        case OK_SIG: {
            status_ = Q_TRAN_HIST(me->hist_Clock);
            break;
        }
        /*${AOs::Clock_Alarm::SM::Alarm_Notify::TICK} */
        case TICK_SIG: {
            Clock_Alarm_display_curr_time(me,ALARM_SETTING_CURR_TIME_ROW,ALARM_SETTING_CURR_TIME_COL);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Alarm_Notify::Q_TIMEOUT} */
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_Clock);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Alarm_Notify::alarm_msg_on} ......................*/
static QState Clock_Alarm_alarm_msg_on(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Alarm_Notify::alarm_msg_on} */
        case Q_ENTRY_SIG: {

            display_write("*ALARM*",ALARM_NOTIFY_MSG_ROW,ALARM_NOTIFY_MSG_COL);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Alarm_Notify::alarm_msg_on::Q_TIMEOUT} */
        case Q_TIMEOUT_SIG: {
            --me->timeout;
            /*${AOs::Clock_Alarm::SM::Alarm_Notify::alarm_msg_on::Q_TIMEOUT::[me->timeout]} */
            if (me->timeout) {
                status_ = Q_TRAN(&Clock_Alarm_alarm_msg_off);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Alarm_Notify);
            break;
        }
    }
    return status_;
}

/*${AOs::Clock_Alarm::SM::Alarm_Notify::alarm_msg_off} .....................*/
static QState Clock_Alarm_alarm_msg_off(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Clock_Alarm::SM::Alarm_Notify::alarm_msg_off} */
        case Q_ENTRY_SIG: {
            display_erase_block(ALARM_NOTIFY_MSG_ROW,ALARM_NOTIFY_MSG_COL,10);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Clock_Alarm::SM::Alarm_Notify::alarm_msg_off::Q_TIMEOUT} */
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_alarm_msg_on);
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Alarm_Notify);
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::Clock_Alarm} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

ISR(TIMER1_COMPA_vect){
     static uint8_t count = 0;
     QF_tickXISR(0);
     if(++count == 100U) {
         count = 0;
         Clock_Alarm_update_curr_time();
        QACTIVE_POST_ISR(AO_ClockAlarm,TICK_SIG,0U);
    }

}
/*
 * Description : Decodes AM/PM information from given time in 24H format
 * param1: Integer time in 24H format
 * return : A string value("AM" or "PM")
 */
String get_am_or_pm(uint32_t time24h){
    String ampm;
    uint8_t h = GET_HOUR(time24h);
    if(h == 0U){
        ampm = "AM";
    }else if( h > 12U){
        ampm = "PM";
    }else if (h == 12U)
        ampm = "PM";
    else
        ampm = "AM";
    return ampm;
}

/*
 * Description: Writes a message to the LCD at given row and column number
 * param1 : Message to write in 'String' format
 * param2 : row number of the LCD
 * param2 : column number of the LCD
 */
void  display_write(String str_, uint8_t r, uint8_t c){
    lcd_set_cursor(c,r);
    lcd_print_string(str_);
}

/*
 * Description: converts an 'integer' time to 'String' time
 * param1 : time represented in terms of number of seconds
 * return : time as 'String' value in the format HH:MM:SS
 */
String integertime_to_string(uint32_t time_){
    uint8_t h,m,s;
    char buf[10]; //00:00:00+null
    h = GET_HOUR(time_); /* Extract how many hours the 'time_' represent */
    m = GET_MIN(time_);  /* Extract how many minutes the 'time_' represent */
    s = GET_SEC(time_);     /* Extract how many seconds the 'time_' represent */
    sprintf(buf,"%02d:%02d:%02d",h,m,s);
    return (String)buf;
}

/*
 * Description: Converts given integer time in 12H format to integer time 24H format
 * param1 : Integer time in 12H format
 * param2 : time format of type time_format_t
 * return : Integer time in 24H format
 */
uint32_t convert_12hformat_to_24h(uint32_t time12h, time_format_t ampm){
    uint8_t hour;
    uint32_t time24h;
    hour = GET_HOUR(time12h);
    if(ampm == FORMAT_AM){
        time24h = (hour == 12)? (time12h-(12UL * 3600UL)) : time12h;
    }else{
        time24h = (hour == 12)? time12h : (time12h +(12UL * 3600UL));
    }
    return time24h;
}

/*
 * Description: Converts given integer time in 24H format to integer time 12H format
 * param1 : Integer time in 24H format
 * return : Integer time in 12H format
 */
uint32_t convert_24hformat_to_12h(uint32_t time24h){
    uint8_t hour;
    uint32_t time12h;
    hour = GET_HOUR(time24h);

    if(hour == 0)
        time12h = time24h + (12UL * 3600UL);
    else{
        if((hour < 12UL) || (hour == 12UL))
            return time24h;
        else
            time12h = time24h - (12UL * 3600UL);
    }
    return time12h;
}

void display_cursor_on_blinkon(void){
    lcd_cursor_show();
    lcd_cursor_blink();
}

void display_set_cursor(uint8_t r, uint8_t c){
     lcd_set_cursor(c,r);
}


void display_cursor_off_blinkoff(){
    lcd_cursor_off();
    lcd_cursor_blinkoff();
}

void display_clear(void){
    lcd_clear();
}

bool is_time_set_error(uint32_t time_,time_format_t format){
    uint8_t h = GET_HOUR(time_);
    return ((h > 23) || ((h > 12 || (h == 0))&& format != FORMAT_24H) );
}


void display_erase_block(uint8_t row,uint8_t col_start,uint8_t col_stop)
{
    uint8_t len = col_stop - col_start;
    do{
        lcd_set_cursor(col_start++,row);
        lcd_print_char(' ');
    }while(len--);
}

