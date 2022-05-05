/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	data-pool-service.c
 * @brief	data service provider
 */

#include "data-pool-static-configurator.h"
#include "ipc_protocol.h"
#include "data-pool-client.h"
#include "data-pool-storage.h"

#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

/** data pool client data */
struct s_data_pool_client {
	struct S_AGLCLUSTER_SERVICE_DATA_V1 data; /**< Data pool data. */
	ssize_t packet_size; /**< Cluster ipc packet receive buffer size. */
};
typedef struct s_data_pool_client *data_pool_client_handle;

static bool data_pool_packet_check_v1(struct S_AGLCLUSTER_SERVICE_PACKETHEADER_V1 *header)
{
	bool ret = false;
	
	if ((header->magic == AGLCLUSTER_SERVICE_PACKETHEADER_MAGIC)
		&& (header->version == AGLCLUSTER_SERVICE_PACKET_VERSION_V1))
		ret = true;
	
	return ret;
}

/**
 * Event handler for receive packet
 *
 * @param [in]	event		Socket event source object
 * @param [in]	fd			File discriptor for socket session
 * @param [in]	revents		Active event (epooll)
 * @param [in]	userdata	Pointer to data_pool_service_handle
 * @return int	 0 success
 *				-1 internal error
 */
int data_pool_receive(int fd)
{
	AGLCLUSTER_SERVICE_PACKET packet;
	int ret = -1;
	ssize_t sret = -1;

	// Receive
	sret = read(fd, &packet,sizeof(packet));
	if (sret == sizeof(packet)) {
		if (data_pool_packet_check_v1(&packet.header) == true) {
			ret = data_pool_set_v1(&packet.data);
		}
	} else {
		ret = -1;
	}

	fprintf(stderr,"rcv size = %ld (req: %ld) seqnum=%lx\n", sret, sizeof(packet), packet.header.seqnum);

	return ret;
}