#pragma once
#include "FreeTypeAmalgam.h"

FT_EXPORT( FT_Error )
FT_Done_Library( FT_Library  library );


FT_EXPORT( void )
FT_Add_Default_Modules( FT_Library  library );

FT_EXPORT( FT_Error )
FT_New_Library( FT_Memory memory, FT_Library  *alibrary );
