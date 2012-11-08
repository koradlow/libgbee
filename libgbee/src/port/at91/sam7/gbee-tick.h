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

#ifndef GBEE_TICK_H_INCLUDED
#define GBEE_TICK_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

/**
 * Get tick count.
 *
 * \return The elapsed ticks.
 */
uint32_t gbeeTickGet(void);

/**
 * Count a tick.
 * Call this method each millisecond to increment the tick counter.
 */
void gbeeTickCount(void);

/**
 * Calculate timeout with the given period.
 *
 * \param[in] period is the timeout period in milliseconds.
 *
 * \return The timestamp of the timeout.
 */
uint32_t gbeeTickTimeoutCalculate(uint32_t period);

/**
 * Check if timeout expired.
 *
 * \param[in] timeout is the timestamp calculated by gbeeTickTimeoutCalculate()
 *
 * \return true if timeout expired, false if timeout not expired.
 */
bool gbeeTickTimeoutExpired(uint32_t timeout);

#endif /* GBEE_TICK_H_INCLUDED */
