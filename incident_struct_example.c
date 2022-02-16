#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "incident.pb-c.h"

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

void print_key_incident_info(KeyIncident *key_incident)
{
    printf("sip: %s\n", key_incident->sip);
    printf("dip: %s\n", key_incident->dip);
    printf("sport: %d\n", key_incident->sport);
    printf("dport: %d\n", key_incident->dport);
    printf("smac: %s\n", key_incident->smac);
    printf("dmac: %s\n", key_incident->dmac);
    printf("proto: %s\n", key_incident->proto);
    printf("app: %s\n", key_incident->app);
    printf("flow_time: %s\n", key_incident->flow_time);
    printf("pkt_time: %s\n", key_incident->pkt_time);
}

int main()
{
    KeyIncident key_incident = KEY_INCIDENT__INIT;
    void *buf = NULL;
    unsigned int len ;
    KeyIncident *msg = NULL;

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

    //unpack key_incident info from buf
    msg = key_incident__unpack(NULL, len, buf);
    print_key_incident_info(msg);
    //free msg
    key_incident__free_unpacked(msg, NULL);

    free(buf);
    free_key_incident_info(&key_incident);

    return 0;
}