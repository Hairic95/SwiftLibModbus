#include <stdio.h>
#include <modbus.h>
#include "modbus-private.h"
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

int send_data(modbus_t *mb, char *buff, unsigned int buff_s) {
    int rc;

    do {
        rc = send(mb->s, buff, buff_s, 0);
    }while (rc == SOCKET_ERROR);

    if (rc > 0 && rc != buff_s) {
        errno = EMBBADDATA;
        return -1;
    }

    return rc;
}

int receive_data(modbus_t *mb, char * buff, unsigned int buff_s) {
    int rc;
    fd_set rset;
    int msg_length = 0;
    unsigned int length_to_read = buff_s;

    FD_ZERO(&rset);
    FD_SET(mb->s, &rset);

    while (length_to_read != 0) {
        while ((rc = select(mb->s + 1, &rset, NULL, NULL, NULL)) == SOCKET_ERROR) {
            if (errno == EINTR) {
                FD_ZERO(&rset);
                FD_SET(mb->s, &rset);
            } else {
                return -1;
            }
            if (rc != 0) {
                if ((rc = recv(mb->s, buff, buff_s, 0)) == SOCKET_ERROR) {
                    return -1;
                } else if(rc == 0) {
                    return 0;
                } else {
                    msg_length += rc;
                    length_to_read -= rc;
                }
            }
        }
    }
    return msg_length;
}
