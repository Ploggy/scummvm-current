/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */


#ifndef GC_H_
#define GC_H_

#include "common/hashmap.h"
#include "sci/include/vm_types.h"
#include "sci/include/engine.h"


struct reg_t_EqualTo {
	bool operator()(const reg_t& x, const reg_t& y) const {
		return (x.segment == y.segment) && (x.offset == y.offset);
	}
};

struct reg_t_Hash {
	uint operator()(const reg_t& x) const {
		return (x.segment << 3) | x.offset;
	}
};

// The reg_t_hash_map is actually really a hashset
typedef Common::HashMap<reg_t, bool, reg_t_Hash, reg_t_EqualTo> reg_t_hash_map;


reg_t_hash_map *
find_all_used_references(state_t *s);
/* Finds all used references and normalises them to their memory addresses
** Parameters: (state_t *) s: The state to gather all information from
** Returns   : (reg_t_hash_map *) A hash map containing entries for all used references
*/

void
run_gc(state_t *s);
/* Runs garbage collection on the current system state
** Parameters: (state_t *) s: The state in which we should gc
*/

#endif /* !defined(GC_H_) */
