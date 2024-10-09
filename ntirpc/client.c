#include <stdio.h>

#include <rpc/rpc.h>
#include <rpc/types.h>

#include "ntirpc_isolation.h"
 
#define TOTAL (30)

static struct svc_req *
alloc_request(SVCXPRT *xprt, XDR *xdrs)
{
	struct svc_req *req = calloc(1, sizeof(*req));

	SVC_REF(xprt, SVC_REF_FLAG_NONE);
	req->rq_xprt = xprt;
	req->rq_xdrs = xdrs;
	req->rq_refcnt = 1;

	return req;
}

static void
free_request(struct svc_req *req, enum xprt_stat stat)
{
	free(req);
}

int main(int argc, char *argv[])
{
	CLIENT *clnt;
	AUTH *auth;
	struct clnt_req *cc;
	struct sockaddr_un addr;
	struct sockaddr_storage ss;
	struct netbuf nbuf;
	svc_init_params *init_params;
	int sock;

	if ((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

	// if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
	// 	perror("connect");
	// 	close(sock);
	// 	exit(1);
	// }

	nbuf.buf = &addr;
	nbuf.len = sizeof(addr);

	init_params = (svc_init_params *)malloc(sizeof(*init_params));
	if (init_params == NULL) {
		perror("svc_init_params()");
		close(sock);
		exit(1);
	}
	memset(init_params, 0, sizeof(*init_params));
	init_params->alloc_cb = alloc_request;
	init_params->free_cb  = free_request;
	svc_init(init_params);

	// clnt = clnt_vc_ncreatef(sock, &nbuf, ISO_PROG, ISO_VERS, 0, 0, CLNT_CREATE_FLAG_NONE);
	clnt = clnt_dg_ncreate(sock, &nbuf, ISO_PROG, ISO_VERS, 0, 0);
	if (CLNT_FAILURE(clnt)) {
		rpc_perror(&clnt->cl_error, "Error creating RPC client");
		close(sock);
		exit(1);
	}

	auth = authnone_ncreate();
	if (AUTH_FAILURE(auth)) {
		rpc_perror(&auth->ah_error, "authnone_ncreate()");
		exit(1);
	}

	cc = (struct clnt_req *)malloc(sizeof(*cc));
	clnt_req_fill(cc, clnt, auth, ISO_PROG,
				  xdr_void, NULL,
				  xdr_void, NULL);

	clnt->cl_ops->cl_call(cc);
	
	AUTH_DESTROY(auth);

	return 0;
}