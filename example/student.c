#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.pb-c.h"

int main(void)
{
    Student pack_stu = {0};
    uint8_t buffer[200] = {0};
    Student *unpack_stu = NULL;
    size_t len = 0;

    student__init(&pack_stu);

    /* 组包 */
    pack_stu.sip = "192.168.1.100";
    pack_stu.dip = "192.168.1.200";
    pack_stu.sport = 80;
    pack_stu.dport = 5637;
    pack_stu.smac = "02-00-4C-4F-4F-50";
    pack_stu.dmac = "00:0c:29:fa:6c:81";
    pack_stu.proto = "tcp";
    pack_stu.app = "https";
    pack_stu.flow_time = "2022-02-16 10:27:24";
    pack_stu.pkt_time = "2022-01-01 18:27:24";
    len = student__pack(&pack_stu, buffer);
    printf("len = %ld\n",len);

    /* 解包 */
    unpack_stu = student__unpack(NULL, len, buffer);
    printf("unpack_stu.sip = %s\n", unpack_stu->sip);
    printf("unpack_stu.dip = %s\n", unpack_stu->dip);
    printf("unpack_stu.sport = %d\n", unpack_stu->sport);
    printf("unpack_stu.dport = %d\n", unpack_stu->dport);
    printf("unpack_stu.smac = %s\n", unpack_stu->smac);
    printf("unpack_stu.dmac = %s\n", unpack_stu->dmac);
    printf("unpack_stu.proto = %s\n", unpack_stu->proto);
    printf("unpack_stu.app = %s\n", unpack_stu->app);
    printf("unpack_stu.flow_time = %s\n", unpack_stu->flow_time);
    printf("unpack_stu.pkt_time = %s\n", unpack_stu->pkt_time);

    student__free_unpacked(unpack_stu, NULL);
    return 0;
}