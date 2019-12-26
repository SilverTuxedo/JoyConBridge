#pragma once
#include "JoyCon.h"


namespace joy_con_bridge::connect
{
/**
	@brief Gets the left JoyCon if it is available.

	@return JoyCon (L)

	@throw HidOpenError if the left JoyCon can't be opened.
*/
JoyCon getLeftJoyCon();


/**
	@brief Gets the right JoyCon if it is available.

	@return JoyCon (R)

	@throw HidOpenError if the right JoyCon can't be opened.
*/
JoyCon getRightJoyCon();
}
