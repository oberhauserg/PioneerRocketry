#include "SDCard.h"

bool SDCard::initializeSDCard()
{
	return SD.begin(CS_PIN);
}

