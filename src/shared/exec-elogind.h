#pragma once
#ifndef ELOGIND_SRC_BASIC_EXEC_ELOGIND_H_INCLUDED
#define ELOGIND_SRC_BASIC_EXEC_ELOGIND_H_INCLUDED

/***
  This file is part of elogind.

  Copyright 2017-2018 Sven Eden

  elogind is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  elogind is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with elogind; If not, see <http://www.gnu.org/licenses/>.
***/

#include "exec-util.h"

extern const gather_stdout_callback_t gather_output[_STDOUT_CONSUME_MAX];

#endif // ELOGIND_SRC_BASIC_EXEC_ELOGIND_H_INCLUDED
