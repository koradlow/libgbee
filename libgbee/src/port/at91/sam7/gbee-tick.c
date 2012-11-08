/**
 * \file
 * \author  d264
 * \version $Rev$
 *
 * \section LICENSE
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * \section DESCRIPTION
 *
 * Simple tick counter implementation.
 */

#include "gbee-tick.h"

/** This is our tick counter. */
static uint32_t ticks = 0;

/******************************************************************************/

uint32_t gbeeTickGet(void)
{
	return ticks;
}

/******************************************************************************/

void gbeeTickCount(void)
{
	ticks++;
}

/******************************************************************************/

uint32_t gbeeTickTimeoutCalculate(uint32_t period)
{
	return ticks + period;
}

/******************************************************************************/

bool gbeeTickTimeoutExpired(uint32_t timeout)
{
	if (ticks >= timeout)
	{
		return true;
	}
	else
	{
		return false;
	}
}
