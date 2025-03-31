#include <cstdarg>
#include <cstdio>

#include "prosdk.h"

#include "logs_service.h"
#include "ui_models/logs_model.h"


namespace Connector {

    namespace {

        Event::EventBus *bus = nullptr;

    } // namespace


    void LogsService::Log(const char *format, ...) {
        // Создаем буфер для форматированной строки
        const size_t bufferSize = 1000; // Размер буфера
        char buffer[bufferSize];

        // Инициализируем список аргументов
        va_list args;
        va_start(args, format);

        // Форматируем строку с помощью vsnprintf
        vsnprintf(buffer, bufferSize, format, args);

        // Завершаем работу с аргументами
        va_end(args);

        bus->PostEvent<Events::AddLogEvent>({.log{buffer}});
    }


    void LogsService::SetUp() {
        bus = evBus;
        //evBus->PostEvent<EventModels::AddLogEvent>(
        //        {
        //                .log{ "03/22 08:51:01 INFO   :.main: *************** RSVP Agent started ***************\n"
        //                      "03/22 08:51:01 INFO   :...locate_configFile: Specified configuration file: /u/user10/rsvpd1.conf\n"
        //                      "03/22 08:51:01 INFO   :.main: Using log level 511\n"
        //                      "03/22 08:51:01 INFO   :..settcpimage: Get TCP images rc - EDC8112I Operation not supported on socket.\n"
        //                      "03/22 08:51:01 INFO   :..settcpimage: Associate with TCP/IP image name = TCPCS\n"
        //                      "03/22 08:51:02 INFO   :..reg_process: registering process with the system\n"
        //                      "03/22 08:51:02 INFO   :..reg_process: attempt OS/390 registration\n"
        //                      "03/22 08:51:02 INFO   :..reg_process: return from registration rc=0\n"
        //                      "03/22 08:51:06 TRACE  :...read_physical_netif: Home list entries returned = 7\n"
        //                      "03/22 08:51:06 INFO   :...read_physical_netif: index #0, interface VLINK1 has address 129.1.1.1, ifidx 0\n"
        //                      "03/22 08:51:06 INFO   :...read_physical_netif: index #1, interface TR1 has address 9.37.65.139, ifidx 1\n"
        //                      "03/22 08:51:06 INFO   :...read_physical_netif: index #2, interface LINK11 has address 9.67.100.1, ifidx 2\n"
        //                      "03/22 08:51:06 INFO   :...read_physical_netif: index #3, interface LINK12 has address 9.67.101.1, ifidx 3\n"
        //                      "03/22 08:51:06 INFO   :...read_physical_netif: index #4, interface CTCD0 has address 9.67.116.98, ifidx 4\n"
        //                      "03/22 08:51:06 INFO   :...read_physical_netif: index #5, interface CTCD2 has address 9.67.117.98, ifidx 5\n"
        //                      "03/22 08:51:06 INFO   :...read_physical_netif: index #6, interface LOOPBACK has address 127.0.0.1, ifidx 0\n"
        //                      "03/22 08:51:06 INFO   :....mailslot_create: creating mailslot for timer\n"
        //                      "03/22 08:51:06 INFO   :...mailbox_register: mailbox allocated for timer\n"
        //                      "03/22 08:51:06 INFO   :.....mailslot_create: creating mailslot for RSVP\n"
        //                      "03/22 08:51:06 INFO   :....mailbox_register: mailbox allocated for rsvp\n"
        //                      "03/22 08:51:06 INFO   :.....mailslot_create: creating mailslot for RSVP via UDP\n"
        //                      "03/22 08:51:06 WARNING:.....mailslot_create: setsockopt(MCAST_ADD) failed - EDC8116I Address not available.\n"
        //                      "03/22 08:51:06 INFO   :....mailbox_register: mailbox allocated for rsvp-udp\n"
        //                      "03/22 08:51:06 TRACE  :..entity_initialize: interface 129.1.1.1, entity for rsvp allocated and initialized\n"
        //                      "03/22 08:51:06 INFO   :.....mailslot_create: creating mailslot for RSVP\n"
        //                      "03/22 08:51:06 INFO   :....mailbox_register: mailbox allocated for rsvp\n"
        //                      "03/22 08:51:06 INFO   :.....mailslot_create: creating mailslot for RSVP via UDP\n"
        //                      "03/22 08:51:06 WARNING:.....mailslot_create: setsockopt(MCAST_ADD) failed - EDC8116I Address not available."
        //                }
        //        });
    }


} // Connector
