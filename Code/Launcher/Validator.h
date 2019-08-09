/**
 * @file
 * @brief Validator.
 */

#pragma once

#include <stddef.h>

// CryEngine headers
#include "IValidator.h"

class Validator : public IValidator
{
public:

	// --- IValidator ---
	void Report( SValidatorRecord & record ) override;
};

