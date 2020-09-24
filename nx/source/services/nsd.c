#define NX_SERVICE_ASSUME_NON_DOMAIN
#include "service_guard.h"
#include "services/nsd.h"

static Service g_nsdSrv;

NX_GENERATE_SERVICE_GUARD(nsd);

Result _nsdInitialize(void) {
    Result rc = smGetService(&g_nsdSrv, "nsd:a");
    
    if (R_FAILED(rc)) rc = smGetService(&g_nsdSrv, "nsd:u");
    
    return rc;
}

void _nsdCleanup(void) {
    serviceClose(&g_nsdSrv);
}

Service* nsdGetServiceSession(void) {
    return &g_nsdSrv;
}

Result nsdGetEnvionmentIdentifier(char *out, size_t out_size) {
    return serviceDispatch(&g_nsdSrv, 11,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { out, out_size } },
    );
}
