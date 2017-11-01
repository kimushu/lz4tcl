#include <tcl.h>
#include <lz4hc.h>

static int lz4_compress_entry(ClientData clientData, Tcl_Interp *interp, int objc, struct Tcl_Obj *const *objv)
{
    const unsigned char *sourceData;
    int sourceLen;
    struct Tcl_Obj *destObj;
    unsigned char *destData;
    int destCapacity;
    int destLen;
    int compressionLevel = 0;

    Tcl_ResetResult(interp);
    if ((objc != 2) && (objc != 3)) {
        Tcl_WrongNumArgs(interp, 1, objv, "source ?compressionLevel");
        return TCL_ERROR;
    }
    if (objc == 3) {
        if (Tcl_GetInt(interp, Tcl_GetString(objv[2]), &compressionLevel) == TCL_ERROR) {
            return TCL_ERROR;
        }
    }
    if (compressionLevel < LZ4HC_CLEVEL_MIN) {
        compressionLevel = LZ4HC_CLEVEL_DEFAULT;
    } else if (compressionLevel > LZ4HC_CLEVEL_MAX) {
        compressionLevel = LZ4HC_CLEVEL_MAX;
    }

    sourceData = Tcl_GetByteArrayFromObj(objv[1], &sourceLen);
    destObj = Tcl_NewByteArrayObj(NULL, sourceLen + (sourceLen >> 2) + 16); // 25% + 16bytes
    Tcl_IncrRefCount(destObj);
    destData = Tcl_GetByteArrayFromObj(destObj, &destCapacity);

    destLen = LZ4_compress_HC(sourceData, destData, sourceLen, destCapacity, compressionLevel);
    if (destLen <= 0) {
        Tcl_SetResult(interp, (char *)"LZ4 compression failure", TCL_STATIC);
        Tcl_DecrRefCount(destObj);
        return TCL_ERROR;
    }
    Tcl_SetByteArrayLength(destObj, destLen);
    Tcl_SetObjResult(interp, destObj);
    Tcl_DecrRefCount(destObj);
    return TCL_OK;
}

#ifdef _WIN32
__declspec(dllexport) __stdcall
#endif
int Lz_Init(Tcl_Interp *interp)
{
    Tcl_CreateObjCommand(interp, "LZ4_compress", lz4_compress_entry, 0, NULL);
    return TCL_OK;
}
