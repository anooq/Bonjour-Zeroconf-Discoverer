//============================================================================
// Name        : device_finder
// Author      : Muzaffar Sheikh
// Description : application to detect devices using Bonjour/Zeroconf
//============================================================================
#include "device_finder.h"

/* printtimestamp - Print the time on the stdout when events occur  */

static void printtimestamp(void)
	{
	struct tm tm;
	int ms;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	localtime_r((time_t*)&tv.tv_sec, &tm);
	ms = tv.tv_usec/1000;
	printf("%2d:%02d:%02d.%03d  ", tm.tm_hour, tm.tm_min, tm.tm_sec, ms);
	}

/*  browse_reply - Call back function called when a service is found/added/removed.
 *  prints the meta data of the service or the error code on the stdout */

static void DNSSD_API browse_reply(DNSServiceRef sdref, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
	const char *replyName, const char *replyType, const char *replyDomain, void *context)
	{
	char *op = (flags & kDNSServiceFlagsAdd) ? "Add" : "Rmv";
	EXIT_IF_LIBDISPATCH_FATAL_ERROR(errorCode);

	if (num_printed++ == 0) printf("Timestamp     A/R Flags if %-25s %-25s %s\n", "Domain", "Service Type", "Instance Name");
	printtimestamp();
	if (errorCode) printf("Error code %d\n", errorCode);
	else printf("%s%6X%3d %-25s %-25s %s\n", op, flags, ifIndex, replyDomain, replyType, replyName);
	if (!(flags & kDNSServiceFlagsMoreComing)) fflush(stdout);

	}

/* HandleEvents() - main scheduler for a non-blocking approach using select() */

static void HandleEvents(void)
	{
	int dns_sd_fd  = client    ? DNSServiceRefSockFD(client   ) : -1;
	int nfds = dns_sd_fd + 1;
	fd_set readfds;
	struct timeval tv;
	int result;
	while (!stopNow)
		{
		FD_ZERO(&readfds);
		if (client   ) FD_SET(dns_sd_fd , &readfds);

		tv.tv_sec  = timeOut; // defines the timeout in seconds before it needs to stop the loop
		tv.tv_usec = 0;

		result = select(nfds, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);

		if (result > 0)
			{
			DNSServiceErrorType err = kDNSServiceErr_NoError;
			if (client    && FD_ISSET(dns_sd_fd , &readfds))
				err = DNSServiceProcessResult(client   );
			if (err)
				{
				fprintf(stderr, "DNSServiceProcessResult returned %d\n", err);
				stopNow = 1;
				}
			}

		else if (result == 0)
			{
			fprintf(stderr, "DNSService add/update/remove failed %ld\n", (long int) kDNSServiceErr_Unknown);
			stopNow = 1;
			}

		else
			{
			printf("select() returned %d errno %d %s\n", result, errno, strerror(errno));
			if (errno != EINTR)
				stopNow = 1;
			}
		}
	}

int main()
	{
	DNSServiceErrorType err;
	DNSServiceFlags flags = 0;

	// typ - variable to indicate which type of services should be searched - assigend value searches for all advertised services.
	char *typ = "_services._dns-sd._udp";

	printf("Browsing . for %s\n", typ);

	/* DNSServiceBrowse - Discovery of devices and their offered services on the local domain using advertised packets.
	 * Devices are only listed not resolved. Services with NonBrowsable flag set will not be discovered. We search on the
	 * default local domain so the domain is NULL and on all interfaces so opinterface = 0.  On success
	 * kDNSServiceErr_NoError is returned, otherwise the error code is returned. When a service is found browse_reply
	 * prints the meta data*/

	err = DNSServiceBrowse(&client, flags, opinterface, typ, NULL, browse_reply, NULL);

	/* if an error code is returned by the DNSServiceBrowse terminate the program */
	if (!client || err != kDNSServiceErr_NoError) {
		fprintf(stderr, "DNSService call failed %ld\n", (long int)err);
		return (-1);
	}

	HandleEvents();		// Schedule the events using a non-blocking callback approach

	// Free up the memory and termiate the connection to the Daemon
	if (client) DNSServiceRefDeallocate(client);
	return 0;

	return 0;
	}
