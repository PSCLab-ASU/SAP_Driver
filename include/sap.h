#include "client_structs.h"


void SAP_Init(int, char*[] );

void SAP_GetNDevices( uint * );

void SAP_GetDeviceIDs( uint, uint[], uint* );

void SAP_GetDeviceDesc( uint, enum DEV_DESC_ATTR, uint *);


void SAP_Finalize();

