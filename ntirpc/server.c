#include <stdio.h>

#include <rpc/rpc.h>
#include "ntirpc_isolation.h"

static void iso_dispatcher(struct svc_req *req);
static void *calc(void *arg, struct svc_req *req);


int main(int argc, char *argv[])
{
	struct sockaddr_un *addr;
	struct netconfig *nconf;
	void *handlep;
	svc_init_params *init_params;
	SVCXPRT *transp;
	int err, sock_fd;
	enum xprt_stat stat;

	setbuf(stdout, NULL);

	printf("socket()\n");
	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd == -1) {
		perror("socket");
		return -1;
	}

	addr = (struct sockaddr_un*)malloc(sizeof(struct sockaddr_un));
	if (addr == NULL) {
		perror("malloc");
		goto close_n_die;
	}
	addr->sun_family = AF_UNIX;
	strncpy(addr->sun_path, SOCKET_PATH, sizeof(addr->sun_path)-1);

	unlink(SOCKET_PATH);

	printf("bind()\n");
	err = bind(sock_fd, (struct sockaddr*)addr, sizeof(struct sockaddr_un));
	if (err == -1) {
		perror("bind");
		goto close_n_die;
	}

	// printf("listen()\n");
	// err = listen(sock_fd, SOMAXCONN);
	// if (err == -1) {
	// 	perror("listen");
	// 	goto close_n_die;
	// }
	
	printf("setnetconfig()\n");
	handlep = setnetconfig();
	if (handlep == NULL) {
		nc_perror("setnetconfig()");
		goto close_n_die;
	}

	printf("getnetconfig()\n");
	while ((nconf = getnetconfig(handlep))) {
		/* Find a configuration using loopback */
		if (strcmp(nconf->nc_protofmly, NC_LOOPBACK) == 0)
			break;
	}
	if (nconf == NULL) {
		nc_perror("No proper config.");
		goto free_nconf;
	}

	printf("svc_init()\n");
	init_params = (svc_init_params *)malloc(sizeof(*init_params));
	if (init_params == NULL) {
		perror("svc_init_params()");
		goto free_nconf;
	}
	svc_init(init_params);

	printf("svc_tli_ncreate()\n");
	transp = svc_tli_ncreate(sock_fd, nconf, NULL, 0, 0);
	if (transp == NULL) {
		perror("svc_tli_ncreate");
		goto free_nconf;
	}

	printf("svc_reg()\n");
	if (!svc_reg(transp, ISO_PROG, ISO_VERS, iso_dispatcher, NULL)) {
		perror("svc_reg");
		goto free_nconf;
	}
	// transp->xp_dispatch.rendezvous_cb = iso_dispatcher;

	printf("SVC_RECV()");
	stat = SVC_RECV(transp);

	svc_shutdown(SVC_SHUTDOWN_FLAG_NONE);
	printf("here");

free_nconf:
	freenetconfigent(nconf);
close_n_die:
	close(sock_fd);
	return -1;
}

/*
 * The server dispatch function
 */
static void iso_dispatcher(struct svc_req *req)
{
	void *result;
	void *(*func)(void*, struct svc_req*);
	xdrproc_t xdr_arg, xdr_res;
	printf("iso_dispatcher() called");

	switch(req->rq_msg.cb_proc) {
		case ISO_CALC:
			func = (void *(*)(void*, struct svc_req*)) calc;
			break;
		default:
			svcerr_noproc(req);
			perror("iso_dispatcher(): unknown request");
			return;
	}

	result = (*func)(NULL, req);
}

static void *calc(void *arg, struct svc_req *req) {
	printf("calc()");

	return (void *)0;
}