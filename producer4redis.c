#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis.h>
#include <errno.h>
#include "incident.pb-c.h"

#ifdef _MSC_VER
#include <winsock2.h> /* For struct timeval */
#endif

#define IP_LEN         20
#define MAC_LEN        20
#define PROTOCOL_LEN   15
#define TIME_LEN       20

static int malloc_key_incident_info(KeyIncident *key_incident)
{
    key_incident->sip = (char*)malloc(IP_LEN);
    if (!key_incident->sip)
    {
        goto FAILED;
    }
    key_incident->dip = (char*)malloc(IP_LEN);
    if (!key_incident->dip)
    {
        goto FAILED;
    }

    key_incident->smac = (char*)malloc(MAC_LEN);
    if (!key_incident->smac)
    {
        goto FAILED;
    }
    key_incident->dmac = (char*)malloc(MAC_LEN);
    if (!key_incident->dmac)
    {
        goto FAILED;
    }

    key_incident->proto = (char*)malloc(PROTOCOL_LEN);
    if (!key_incident->proto)
    {
        goto FAILED;
    }
    key_incident->app = (char*)malloc(PROTOCOL_LEN);
    if (!key_incident->app)
    {
        goto FAILED;
    }

    key_incident->flow_time = (char*)malloc(TIME_LEN);
    if (!key_incident->flow_time)
    {
        goto FAILED;
    }
    key_incident->pkt_time = (char*)malloc(TIME_LEN);
    if (!key_incident->pkt_time)
    {
        goto FAILED;
    }
    return 0;

FAILED:
    fprintf(stdout, "malloc error.errno:%u,reason:%s\n", errno, strerror(errno));
    return -1;
}

static void free_key_incident_info(KeyIncident *key_incident)
{
    if (key_incident->sip)
    {
        free(key_incident->sip);
        key_incident->sip = NULL;
    }
    if (key_incident->dip)
    {
        free(key_incident->dip);
        key_incident->dip = NULL;
    }

    if (key_incident->smac)
    {
        free(key_incident->smac);
        key_incident->smac = NULL;
    }
    if (key_incident->dmac)
    {
        free(key_incident->dmac);
        key_incident->dmac = NULL;
    }

    if (key_incident->proto)
    {
        free(key_incident->proto);
        key_incident->proto = NULL;
    }
    if (key_incident->app)
    {
        free(key_incident->app);
        key_incident->app = NULL;
    }

    if (key_incident->flow_time)
    {
        free(key_incident->flow_time);
        key_incident->flow_time = NULL;
    }
    if (key_incident->pkt_time)
    {
        free(key_incident->pkt_time);
        key_incident->pkt_time = NULL;
    }
}

static void set_key_incident_info(KeyIncident *key_incident)
{
    const char *sip = "192.168.1.100";
    const char *dip = "192.168.1.200";
    const char *smac = "02-00-4C-4F-4F-50";
    const char *dmac = "00:0c:29:fa:6c:81";
    const char *proto = "tcp";
    const char *app = "https";
    const char *flow_time = "2022-02-16 10:27:24";
    const char *pkt_time = "2022-01-01 18:27:24";

    strncpy(key_incident->sip, sip, IP_LEN);
    strncpy(key_incident->dip, dip, IP_LEN);
    key_incident->sport = 3306;
    key_incident->dport = 443;
    strncpy(key_incident->smac, smac, MAC_LEN);
    strncpy(key_incident->dmac, dmac, MAC_LEN);
    strncpy(key_incident->proto, proto, PROTOCOL_LEN);
    strncpy(key_incident->app, app, PROTOCOL_LEN);
    strncpy(key_incident->flow_time, flow_time, TIME_LEN);
    strncpy(key_incident->pkt_time, pkt_time, TIME_LEN);
}

int main(int argc, char **argv) {
    unsigned is_unix = 0;
    redisContext *c;
    redisReply *reply;
    const char *hostname = (argc > 1) ? argv[1] : "127.0.0.1";

    KeyIncident key_incident = KEY_INCIDENT__INIT;
    void *buf = NULL;
    unsigned int len;
    unsigned int i = 0;
    KeyIncident *msg = NULL;

    if (argc > 2) {
        if (*argv[2] == 'u' || *argv[2] == 'U') {
            is_unix = 1;
            /* in this case, host is the path to the unix socket */
            printf("Will connect to unix socket @%s\n", hostname);
        }
    }

    int port = (argc > 2) ? atoi(argv[2]) : 6379;

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    if (is_unix) {
        c = redisConnectUnixWithTimeout(hostname, timeout);
    } else {
        c = redisConnectWithTimeout(hostname, port, timeout);
    }
    if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }

    /* PING server */
    reply = redisCommand(c,"PING");
    printf("PING: %s\n", reply->str);
    freeReplyObject(reply);

    /* Switch to DB 8 for testing, now that we know we can chat. */
    reply = redisCommand(c,"SELECT 8");
    printf("SELECT: %s\n", reply->str);
    freeReplyObject(reply);

    /* Set a key
        reply = redisCommand(c,"SET %s %s", "foo", "hello world");
        printf("SET: %s\n", reply->str);
        freeReplyObject(reply);
    */

    /* Set a key using binary safe API
        reply = redisCommand(c,"SET %b %b", "bar", (size_t) 3, "hello", (size_t) 5);
        printf("SET (binary API): %s\n", reply->str);
        freeReplyObject(reply);
    */

    /* Try a GET and two INCR
        reply = redisCommand(c,"GET foo");
        printf("GET foo: %s\n", reply->str);
        freeReplyObject(reply);
    */

    /*
        reply = redisCommand(c,"INCR counter");
        printf("INCR counter: %lld\n", reply->integer);
        freeReplyObject(reply);
    */
    /* again ... */
    /*
        reply = redisCommand(c,"INCR counter");
        printf("INCR counter: %lld\n", reply->integer);
        freeReplyObject(reply);
    */

    /* Create a list of numbers, from 0 to 9 */
//    reply = redisCommand(c,"DEL key_incident_queue");
//    freeReplyObject(reply);

    if (malloc_key_incident_info(&key_incident) == -1) {
        exit(0);
    }

    set_key_incident_info(&key_incident);
    //get key_incident packed size
    len = key_incident__get_packed_size(&key_incident);
    printf("size of key_incident info : %u\n",len);
    buf = malloc(len);
    //put key_incident info pack to buf
    key_incident__pack(&key_incident, buf);
    for (i = 0; i < 200000; i++) {
        reply = redisCommand(c, "RPUSH key_incident_queue %s", buf);
        if (reply->type == REDIS_REPLY_ERROR) {
            printf("Error: <%s>, index: <%d>\n", reply->str, i);
            i--;
            sleep(1);
        } else {
            printf("Success: <%s>, index: <%d>\n", reply->str, i);
        }
        freeReplyObject(reply);
    }
    free(buf);
    buf = NULL;

    /* Let's check what we have inside the list
        reply = redisCommand(c,"LRANGE mylist 0 -1");
        if (reply->type == REDIS_REPLY_ARRAY) {
            for (j = 0; j < reply->elements; j++) {
                printf("%u) %s\n", j, reply->element[j]->str);
            }
        }
        freeReplyObject(reply);
    */

    /* Disconnects and frees the context */
    redisFree(c);
    return 0;
}
